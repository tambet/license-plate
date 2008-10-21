////////////////////////////////////////////////////////////////////////
//
// vertical.cpp
//
// This program compute vertical projection of an raster image.
//
////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

#define CV_VERTICAL		0
#define CV_HORIZONTAL	1

int main(int argc, char *argv[]){
	IplImage *img = 0, *dest_img=0; 
	int height,width,step,channels;
	uchar *data;
	int i,j,k;

	// load an image  
	img = cvLoadImage(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	if(!img){
		printf("Could not load image file: %s\n",argv[1]);
		exit(0);
	}

  // get the image data
  height    = img->height;
  width     = img->width;
  step      = img->widthStep;
  channels  = img->nChannels;

  printf("widthStep: %i\n",step);
  
	// cvAdaptiveThreshold( img, img, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 3, 5 );

	// create destination image
	dest_img = cvCreateImage(cvSize(img->width, 1), IPL_DEPTH_8U, 1);

  // calculate vertical projection
	cvReduce( img, dest_img, CV_VERTICAL, CV_REDUCE_AVG );
	
	data = (uchar *)dest_img->imageData;
	for(i = 0; i < dest_img->width; i++){
		printf("%i\n", data[i]);
	} 
		
  // create a window
  cvNamedWindow("mainWin", CV_WINDOW_AUTOSIZE); 

  // show the image
  cvShowImage("mainWin", img );

  // wait for a key
  cvWaitKey(0);

  // release the image
  cvReleaseImage(&img );

  return 0;
}
