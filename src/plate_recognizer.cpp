/**********************************************************************
 * --------------------------------------------------------------------
 * Automatic Licence Plate Recognition
 * --------------------------------------------------------------------
 *
 * File:        plate_recognizer.cpp
 * Author:	Tambet Masik
 * Created:	2008.07.03
 * Description: Licence plate detection module
 *
 **********************************************************************/


#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <string.h>
#include "const.hpp"
#include "plate_helpers.hpp"

int main(int argc, char** argv){
  int pressed_key;
  IplImage *img = NULL, *img_gray = NULL;
  CvMemStorage* storage = NULL;

  // create memory storage that will contain all the dynamic data
  storage = cvCreateMemStorage(0);

  for( int i = 1; i < argc; i++ ){
    if( (img = cvLoadImage( argv[i], CV_LOAD_IMAGE_COLOR )) == 0 ){
      printf("Couldn't load %s\n", argv[i] );
      continue;
    }

    // convert input image to grayscale
    img_gray = cvCreateImage(cvGetSize( img ), IPL_DEPTH_8U, CHANNELS_GRAY);
    cvCvtColor( img, img_gray, CV_BGR2GRAY );

    // find all squares
    CvSeq* squares = cvFindPlateCandidates( img, storage );
    CvSeq* unified = cvCombineContours(squares, storage);
    CvSeq* plates  = cvPlateSegmentation( img_gray, unified, storage );

    // draw all squares
    drawSquares( img, unified );
    pressed_key = cvWaitKey();

    // release images, clear memory storage
    cvReleaseImage( &img );
    cvReleaseImage( &img_gray );
    cvClearMemStorage( storage );

    if( (char)pressed_key == ESC )
      break;
  }

  return 0;
}

