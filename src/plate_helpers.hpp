/**********************************************************************
 * --------------------------------------------------------------------
 * Automatic Licence Plate Recognition
 * --------------------------------------------------------------------
 *
 * File:        plate_helpers.hpp
 * Author:	Tambet Masik
 * Created:	2009.04.27
 * Description: Helper methods for LPR
 *
 **********************************************************************/

#include <cv.h>
#include <stdio.h>
#include <highgui.h>

#include <string>
#include <fstream>
#include <iostream>

#include "const.hpp"
#include "shear.hpp"
#include "compare.hpp"
#include "syntactical_analysis.hpp"

using namespace std;

string cvOcrImage( const CvArr* image);

void cvBinaryProcess(IplImage* img_binary);
void cvDeskewImage(IplImage *srcImg, double angle);
void showImage(const char* winname, IplImage* img_src);
void cvRotateImage(IplImage *src, IplImage *dst, double angle);
void cvVerticalShear( IplImage *src, IplImage *dst, double angle );
void cvHorizontalShear( IplImage *src, IplImage *dst, double angle );
void cvCharacterRecognition(IplImage* img_binary, CvMemStorage* storage);
void cvSubstractBackground(IplImage* image, CvSeq* contour, CvPoint offset);

// Finds maximum contour by area
void cvFindMaxContour(CvSeq* contours, CvSeq* &max_contour){
  double area, max_area = 0.0;

  while( contours ){
    area = fabs( cvContourArea(contours, CV_WHOLE_SEQ) );
    if( area > MIN_AREA && area > max_area){
      max_area     = area;
      max_contour  = contours;
    }
    contours = contours->h_next;
  }
}

// Euclidean distance between P1 and P2.
double distance(CvPoint p1, CvPoint p2){
  double x = p1.x - p2.x, y = p1.y - p2.y;
  return (double) sqrt ( x*x + y*y);
}


// Combines similar contours by clustering
CvSeq* cvCombineContours( CvSeq* contours, CvMemStorage* storage){
  CvSeq *contour = NULL, *combined_contours = NULL, *idx_seq = NULL;
  
  combined_contours = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvSeq*), storage );

  // Perform clustering of extracted quads
  // Same quad can be extracted from different binarization levels and channels
  int cluster_count = cvSeqPartition( contours, storage, &idx_seq, cvQuadCmp, 0 );
  
  for( int i = 0; i < cluster_count; i++ ){
    //extract smallest quad in group
    double min_area = DBL_MAX;
    CvSeq* min_contour = NULL;
    for( int j = 0; j < contours->total; j++ ){
      int index = *(int*)cvGetSeqElem(idx_seq, j);
      if(index != i) continue;
      CvContour* contour = *(CvContour**)cvGetSeqElem(contours, j);
      if( fabs(cvContourArea(contour, CV_WHOLE_SEQ)) < min_area){
        min_area = fabs(cvContourArea(contour, CV_WHOLE_SEQ));
        min_contour = (CvSeq*)contour;
      }
    }
    cvSeqPush( combined_contours, &min_contour);
  }
  return combined_contours;
}

// Takes gray image as input and returns plate number
void cvPlateRecognition(IplImage *img_src, CvMemStorage* storage){
  CvSeq *contours = NULL, *max_contour = NULL;
  IplImage *img_thres = NULL, *img_adaptive = NULL;

  img_thres    = cvCreateImage( cvGetSize(img_src), IPL_DEPTH_8U, CHANNELS_GRAY);
  img_adaptive = cvCreateImage( cvGetSize(img_src), IPL_DEPTH_8U, CHANNELS_GRAY);

  //cvThreshold( img_src, img_thres, THRESHOLD_LEVEL, 255, CV_THRESH_BINARY );
  cvAdaptiveThreshold(img_src, img_thres, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, ADAPTIVE_BLOCK, ADAPTIVE_OFFSET);
  cvAdaptiveThreshold(img_src, img_adaptive, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, ADAPTIVE_BLOCK, ADAPTIVE_OFFSET);

  
  #ifdef DEBUG
    showImage("Thres", img_thres);
  #endif

  // Finds max contour
  cvFindContours(img_thres, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL);
  cvFindMaxContour(contours, max_contour);
  if(max_contour == NULL){
    return;
  }

  // Deskews image
  CvBox2D boundBox = cvMinAreaRect2( max_contour, storage );
  cvSubstractBackground(img_adaptive, max_contour, cvPoint( 0, 0 ));
  cvDeskewImage(img_adaptive, boundBox.angle);
  
  #ifdef DEBUG
    showImage("Deskewed", img_adaptive);
  #endif

  cvCharacterRecognition(img_adaptive, storage);

  cvReleaseImage(&img_thres);
  cvReleaseImage(&img_adaptive);
}

void showImage(const char* winname, IplImage* img_src){
  cvNamedWindow(winname, CV_WINDOW_AUTOSIZE);
  cvShowImage(winname, img_src );
  cvWaitKey();
  cvDestroyWindow(winname);
}

void cvCharacterRecognition(IplImage* img_binary, CvMemStorage* storage){
  double area;
  CvRect rect;
  CvSeq *contours = NULL, *contour = NULL;
  IplImage* img_tmp   = cvCloneImage(img_binary);
  IplImage* img_mask  = cvCreateImage(cvGetSize(img_binary), IPL_DEPTH_8U, CHANNELS_GRAY);
  IplImage* img_clean = cvCreateImage(cvGetSize(img_binary), IPL_DEPTH_8U, CHANNELS_GRAY);
  IplImage* img_final = cvCreateImage(cvGetSize(img_binary), IPL_DEPTH_8U, CHANNELS_GRAY);

  // Edge detection and dilate to remove potential holes between edge segments
  cvCanny( img_binary, img_tmp, 0, CANNY_THRESHOLD, 5);
  cvBinaryProcess(img_tmp);
  
  #ifdef DEBUG
    showImage("Canny", img_tmp);
  #endif
  
  CvContourScanner scanner = cvStartFindContours(img_tmp, storage, sizeof(CvContour), CV_RETR_EXTERNAL);
  while (contour = cvFindNextContour(scanner)){
    rect = cvBoundingRect(contour, UPDATE_HEADER );
    area = fabs( cvContourArea(contour, CV_WHOLE_SEQ) );
    
    if( area < MIN_GLYPH_AREA || ((double)rect.height/rect.width) < HEIGHT_WIDTH_RATIO) {
      cvSubstituteContour(scanner, NULL);
    }
  }
  contours = cvEndFindContours(&scanner);
  
  CvSeq* all_contours = cvTreeToNodeSeq( contours, sizeof(CvSeq), storage );
  cvSeqSort( all_contours, cvSeqCmp, NO_USERDATA );
  
  string plate_number;
  
  for(int i = 0; i < all_contours->total; i++){
    CvSeq *contour = *(CvSeq**)cvGetSeqElem( all_contours, i );  
    
    cvZero(img_mask);
    cvSet( img_clean, CVX_WHITE);

    CvPoint *pts = new CvPoint[contour->total];
    cvCvtSeqToArray(contour, pts, CV_WHOLE_SEQ);
    cvFillPoly( img_mask, &pts, &contour->total, 1, CVX_WHITE);

    cvCopy( img_binary, img_clean, img_mask);
    CvBox2D boundBox = cvMinAreaRect2( contour, storage );  
    
    if(boundBox.angle < 45){
      cvHorizontalShear(img_clean, img_final, boundBox.angle);  
    }else{
      cvCopy(img_clean, img_final);
    }
    
    plate_number += cvOcrImage(img_final);
    
    #ifdef DEBUG
      showImage("Mask", img_final);
    #endif
 
    delete pts;
  }
  

  if(cvValidPlate(plate_number)){
    cout << cvAnalyzePlate(plate_number) << endl;
  }
  
  cvReleaseImage(&img_tmp);
}

void cvBinaryProcess(IplImage* img_binary){
  //IplConvKernel* kernel = cvCreateStructuringElementEx( 3, 1, 1, 0, CV_SHAPE_RECT);
  IplConvKernel* kernel = cvCreateStructuringElementEx( 2, 2, 1, 1, CV_SHAPE_ELLIPSE);
  cvDilate(img_binary, img_binary, kernel, 1);
  cvErode(img_binary, img_binary, kernel, 1);
  cvReleaseStructuringElement(&kernel);
}

void cvDeskewImage(IplImage *img_src, double angle){
  if( angle > MIN_SKEW_ANGLE ){	
    // alpha - min angle between the horizontal axis and the first sidehorizontal axis
    // Limited to 45 degree deskewing only
    double alpha = (angle > 45.0) ? 90.0 - angle : -angle;
    IplImage *img_tmp = cvCloneImage( img_src );
    cvVerticalShear( img_src, img_tmp, alpha);
    cvCopy( img_tmp, img_src );
    cvReleaseImage( &img_tmp );
  }
}

// Rotates image by given angle
void cvRotateImage(IplImage *src, IplImage *dst, double angle){
  CvMat* map_matrix   = cvCreateMat(2, 3, CV_32FC1);
  CvPoint2D32f center = cvPoint2D32f(src->width/2, src->height/2);
  cv2DRotationMatrix(center, angle, NO_SCALE, map_matrix);
  cvWarpAffine(src, dst, map_matrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, CVX_WHITE);
}

// Substacts outer contour background
void cvSubstractBackground(IplImage* image, CvSeq* contour, CvPoint offset){
  int height, width, step, channels, x, y, k;
  height    = image->height;
  width     = image->width;
  step      = image->widthStep;
  channels  = image->nChannels;
  uchar* data   = (uchar *)image->imageData;

  for(y = 0; y < height; y++)
    for(x = 0; x < width; x++)
      for(k = 0; k < channels; k++)
        if (cvPointPolygonTest(contour, cvPoint2D32f(x + offset.x, y + offset.y), 0) <= 0){
          data[y*step+x*channels+k] = 255;
        }	
}

// finds a cosine of angle between vectors from pt1->pt2 and from pt3->pt4 
double cosAngle( CvPoint* pt1, CvPoint* pt2, CvPoint* pt3, CvPoint* pt4 ){
  double dx1, dx2, dy1, dy2;
  dx1 = pt2->x - pt1->x;
  dy1 = pt2->y - pt1->y;
  dx2 = pt4->x - pt3->x;
  dy2 = pt4->y - pt3->y;
  return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

double cosA(CvPoint* pt1, CvPoint* pt2, CvPoint* pt3, CvPoint* pt4 ){
  double dx1, dx2, dy1, dy2;
  dx1 = pt2->x - pt1->x;
  dy1 = pt2->y - pt1->y;
  dx2 = pt4->x - pt3->x;
  dy2 = pt4->y - pt3->y;
  return (dx1*dx2 + dy1*dy2) /(sqrt((dx1*dx1 + dy1*dy1)) * sqrt((dx2*dx2 + dy2*dy2)));
}

// reduces image noise with image pyramid
void cvRemoveNoise(IplImage * src){
  CvSize sz = cvSize( src->width & -2, src->height & -2 ); 
  IplImage* pyr = cvCreateImage( cvSize(sz.width/2, sz.height/2), src->depth, src->nChannels ); 

  cvPyrDown( src, pyr, 7 );
  cvPyrUp( pyr, src, 7 );
  cvReleaseImage(&pyr);
}

// performs OCR on specified image, does not support ROI
string cvOcrImage( const CvArr* image){
  char command[100], suffix[] = ".txt";
  char image_file[] = "/tmp/image.tif", result_file[] = "/tmp/result";

  sprintf(command, "tesseract %s %s 1>/dev/null 2>&1 \n", image_file, result_file );
  cvSaveImage(image_file, image);
  system(command);
  
  string result;
  ifstream file ("/tmp/result.txt");
  getline (file, result);
  file.close();
  return result;
}

CvSeq* cvPlateSegmentation( IplImage* image, CvSeq* squares, CvMemStorage* storage ){
  CvSeq* square;
  CvRect rect;
  IplImage *img_bound = NULL, *img_resized = NULL;

  for(int i = 0; i < squares->total; i++){
    square = *(CvSeq**)cvGetSeqElem( squares, i );
    rect = ((CvContour*)square)->rect;
    img_bound = cvCreateImage(cvSize(rect.width, rect.height), IPL_DEPTH_8U, CHANNELS_GRAY);	
    cvSetImageROI( image, rect );
    cvCopy( image, img_bound);		
    cvResetImageROI( image );
    
    // Image resize
    CvSize size = cvSize(img_bound->width * RESIZE_MULTIPLIER, img_bound->height * RESIZE_MULTIPLIER);
    IplImage* img_resized = cvCreateImage(size, IPL_DEPTH_8U, CHANNELS_GRAY);
    cvResize(img_bound, img_resized, CV_INTER_LINEAR);
    
    cvPlateRecognition(img_resized, storage);

    cvReleaseImage( &img_bound );
    cvReleaseImage( &img_resized );
  }
}

// Square contours should have 4 vertices after approximation,
// relatively large area (to filter out noisy contours) and be convex.
int verifyRect( CvSeq *cont ){
  return cont->total == RECT_VERTICES && 
    fabs(cvContourArea(cont, CV_WHOLE_SEQ)) > PLATE_MIN_AREA &&
    fabs(cvContourArea(cont, CV_WHOLE_SEQ)) < PLATE_MAX_AREA && 
    cvCheckContourConvexity(cont);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
CvSeq* cvFindPlateCandidates( IplImage* img, CvMemStorage* storage ){
  double s, t;
  int i, channel, l, N = 11;
  CvSeq *result, *contours;
  IplImage *img_gray_tmp = NULL;
  CvSize size = cvSize( img->width & -2, img->height & -2 );

  IplImage* img_tmp = cvCloneImage( img );
  IplImage* img_gray = cvCreateImage( size, IPL_DEPTH_8U, CHANNELS_GRAY );

  // create empty sequence that will contain square contours
  CvSeq* squareContours = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvSeq*), storage );

  // select the maximum ROI in the image with the width and height divisible by 2
  cvSetImageROI( img_tmp, cvRect( 0, 0, size.width, size.height ));  

  img_gray_tmp	= cvCreateImage(size, IPL_DEPTH_8U, CHANNELS_GRAY);

  // find squares in every color plane of the image
  for( channel = 0; channel < img_tmp->nChannels; channel++ ){
    // extract the c-th color plane
    cvSetImageCOI( img_tmp, channel + 1 );
    cvCopy( img_tmp, img_gray_tmp, 0 );

    // try several threshold levels
    for( l = 0; l < N; l++ ){

      // Using Apadtive instead of zero threshold to catch squares with gradient shading   
      if( l == 0 ){
        // Do we really need this?
        cvAdaptiveThreshold(img_gray_tmp, img_gray, 255, CV_ADAPTIVE_THRESH_MEAN_C, 
            CV_THRESH_BINARY, BLOCK_SIZE);
      }
      else{
        cvThreshold( img_gray_tmp, img_gray, (l+1)*255/N, 255, CV_THRESH_BINARY );
      }

      // find contours and store them all as a list
      cvFindContours( img_gray, storage, &contours, sizeof(CvContour), 
          CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

      // test each contour
      while( contours ){

        // approximate contour with accuracy proportional to the contour perimeter
        result = cvApproxPoly( contours, sizeof(CvContour), storage, 
            CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0 );

        if(verifyRect(result)){

          CvSeq* plate_candidate = cvCloneSeq( result, storage );
          cvSeqSort( plate_candidate, cvPointCmp, NO_USERDATA );

          CvPoint *pt1, *pt2, *pt3, *pt4;

          pt1 = (CvPoint*)cvGetSeqElem( plate_candidate, 0 );
          pt2 = (CvPoint*)cvGetSeqElem( plate_candidate, 1 );
          pt3 = (CvPoint*)cvGetSeqElem( plate_candidate, 2 );
          pt4 = (CvPoint*)cvGetSeqElem( plate_candidate, 3 );

          double angle		= cosA( pt1, pt2, pt3, pt4 );
          double revAngle	= cosA( pt1, pt3, pt2, pt4 );

          if( fabs(angle) >= MIN_ANGLE && fabs(revAngle) >= MIN_ANGLE ){
            cvSeqPush( squareContours, &result);
          }
        }
        contours = contours->h_next;
      }
    }
  }

  // release all the temporary images
  cvReleaseImage( &img_tmp );
  cvReleaseImage( &img_gray );
  cvReleaseImage( &img_gray_tmp );

  return squareContours;
}

// the function draws all the squares in the image
void drawSquares( IplImage* img, CvSeq* squares ){
  IplImage* img_copy = cvCloneImage( img );
  int i, count = RECT_VERTICES;

  for(i = 0; i < squares->total; i++){
    CvPoint pt[RECT_VERTICES], *rect = pt;
    CvSeq *cnt = *(CvSeq**)cvGetSeqElem( squares, i );			
    cvCvtSeqToArray(cnt, rect, CV_WHOLE_SEQ);
    cvPolyLine( img_copy, &rect, &count, 1, 1, CVX_BLACK, 2, CV_AA, 0 );
  }

  // show the resultant image
  showImage("Result", img_copy);
  cvReleaseImage( &img_copy );
}
