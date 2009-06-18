/**********************************************************************
* --------------------------------------------------------------------
  * Automatic Licence Plate Recognition
  * --------------------------------------------------------------------
  *
  * File:         stretch_contrast.hpp
  * Author:	  Unknown, Tambet Masik
  * Created:	  2009.04.20
  * Description:  Performs a contrast-stretching
  *
  **********************************************************************/
#include <map> 
#include <string>
#include <iostream>

IplImage * StretchContrast(IplImage * src){

  CvHistogram *hist;
  uchar lut[256];
  CvMat* lut_mat;
  int hist_size = 256;
  float range_0[]={0,256};
  float* ranges[] = { range_0 };

  int high=0;
  int low =0;
  int index;	
  float hist_value;

  IplImage * dest = cvCloneImage(src);

  IplImage * GRAY = cvCreateImage(cvGetSize(src),src->depth,1);

  if (src->nChannels ==3){
    cvCvtColor(src,GRAY,CV_RGB2GRAY);
  }
  else{
    cvCopyImage(src,GRAY);	
  }

  hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
  lut_mat = cvCreateMatHeader( 1, 256, CV_8UC1 );
  cvSetData( lut_mat, lut, 0 );
  cvCalcHist( &GRAY, hist, 0, NULL );
  
  // The algorithm is from
  // http://vision.ee.pusan.ac.kr/~kgnam/ip/ch2-5.html

  //FIND LOW 
  for(  index = 0; index < hist_size; index++ ){
    hist_value = cvQueryHistValue_1D(hist,index);
    if (hist_value != 0){
      low = index;
      break;
    }
  }

  //FIND HIGH
  for(  index = hist_size-1; index >= 0; index-- ){
    hist_value = cvQueryHistValue_1D(hist,index);
    if (hist_value != 0) {
      high = index;
      break;
    }
  }


  //CALCULATE THE NEW LUT

  float scale_factor;

  scale_factor = 255.0f / (float) (high-low);
  for (index=0; index<256;index++){
    if ((index>=low)&&(index<=high)){
      lut[index]= (unsigned char)((float)(index-low)*scale_factor);
    }
    if (index>high) lut[index]=255;
  }

  if (src->nChannels ==3){

    IplImage * R = cvCreateImage(cvGetSize(src),src->depth,1);
    IplImage * G = cvCreateImage(cvGetSize(src),src->depth,1);
    IplImage * B = cvCreateImage(cvGetSize(src),src->depth,1);

    cvCvtPixToPlane(src,R,G,B,NULL);

    // PERFORM IT ON EVERY CHANNEL
    cvLUT( R, R, lut_mat );
    cvLUT( G, G, lut_mat );
    cvLUT( B, B, lut_mat );

    cvCvtPlaneToPix(R,G,B,NULL,dest);

    cvReleaseImage(&R);
    cvReleaseImage(&G);
    cvReleaseImage(&B);

  }
  else{
    //PERFORM IT ON THE CHANNEL
    cvLUT( GRAY, dest, lut_mat );		
  }

  cvReleaseImage(&GRAY);
  cvReleaseMat( &lut_mat);
  cvReleaseHist(&hist);
  return dest;
}

