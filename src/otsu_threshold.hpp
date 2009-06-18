/**********************************************************************
* --------------------------------------------------------------------
  * Automatic Licence Plate Recognition
  * --------------------------------------------------------------------
  *
  * File:         otsu_threshold.cpp
  * Author:	  Ryan Dibble
  * Created:	  2009.05.08
  * Description:  OTSU global thresholding routine
  takes a 2D unsigned char array pointer, number of rows, and
  number of cols in the array. returns the value of the threshold
  *
  *********************************************************************/
  
static int AutoThrOstu(unsigned char *image, int rows, int cols, int x0, int y0, int dx, int dy){
  unsigned char *np;
  int thresholdValue = 1;
  int ihist[256];
  int i, j, k; // various counters
  int n, n1, n2, gmin, gmax;
  double m1, m2, sum, csum, fmax, sb;
  memset(ihist, 0, sizeof(ihist));
  gmin=255; gmax=0;

  for (i = y0; i < y0 + rows; i++){
    np = &image[i*cols + x0];
    for (j = x0; j < x0 + cols; j++){
      ihist[*np]++;
      if(*np > gmax) gmax=*np;
      if(*np < gmin) gmin=*np;
      np++; /* next pixel */
    }
  }

  // set up everything
  sum = csum = 0.0;
  n = 0;
  for (k = 0; k <= 255; k++){
    sum += (double) k * (double) ihist[k];
    n += ihist[k];
  }
  if (!n) {
    // if n has no value, there is problems...
    fprintf (stderr, "NOT NORMAL thresholdValue = 160 ");
    return (160);
  }
  
    // do the otsu global thresholding method
  fmax = -1.0;
  n1 = 0;
  for (k = 0; k < 255; k++){
    n1 += ihist[k];
    if (!n1){
      continue;
    }
    n2 = n - n1;
    if (n2 == 0){
      break;
    }
    csum += (double) k *ihist[k];
    m1 = csum / n1;
    m2 = (sum - csum) / n2;
    sb = (double) n1 *(double) n2 *(m1 - m2) * (m1 - m2);

    if (sb > fmax){
      fmax = sb;
      thresholdValue = k;
    }
  }

  return(thresholdValue);
}

int AutoThreshold(IplImage *img){
  return AutoThrOstu((unsigned char*)img->imageData, img->height, img->width, 0, 0, 1, 1);
}
