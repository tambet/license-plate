/**********************************************************************
 * --------------------------------------------------------------------
 * Automatic Licence Plate Recognition
 * --------------------------------------------------------------------
 *
 * File:        shear.hpp
 * Author:	Tambet Masik
 * Created:	2009.05.17
 * Description: Vertical and horizontal shearing
 *
 **********************************************************************/
 
// Vertical shear of an image
// map matrix source: http://www.leptonica.com/affine.html
void cvVerticalShear( IplImage *src, IplImage *dst, double angle ){
  angle *= CV_PI/180;
  double x = src->width / 2;
  double theta = tan(angle);

   // map matrix for vertical shear
  CvMat* map_matrix	= cvCreateMat(2, 3, CV_32FC1);
  cvmSet(map_matrix, 0, 0, 1);
  cvmSet(map_matrix, 0, 1, 0);
  cvmSet(map_matrix, 0, 2, 0);
  cvmSet(map_matrix, 1, 0, -theta);
  cvmSet(map_matrix, 1, 1, 1);
  cvmSet(map_matrix, 1, 2, x*theta);

  cvWarpAffine(src, dst, map_matrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, CVX_WHITE);
}

// Horizontal shear of an image
void cvHorizontalShear( IplImage *src, IplImage *dst, double angle ){
  angle *= CV_PI/180;
  double y = src->height / 2;
  double theta = tan(angle);

   // map matrix for vertical shear
  CvMat* map_matrix	= cvCreateMat(2, 3, CV_32FC1);
  cvmSet(map_matrix, 0, 0, 1);
  cvmSet(map_matrix, 0, 1, -theta);
  cvmSet(map_matrix, 0, 2, y*theta);
  cvmSet(map_matrix, 1, 0, 0);
  cvmSet(map_matrix, 1, 1, 1);
  cvmSet(map_matrix, 1, 2, 0);

  cvWarpAffine(src, dst, map_matrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, CVX_WHITE);
}

