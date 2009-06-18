/**********************************************************************
 * --------------------------------------------------------------------
 * Automatic Licence Plate Recognition
 * --------------------------------------------------------------------
 *
 * File:        compare.hpp
 * Author:	Tambet Masik
 * Created:	2009.05.17
 * Description: Compare functions
 *
 **********************************************************************/
 
// sorts points by coordinates
static int cvPointCmp( const void* _a, const void* _b, void* userdata ){
  CvPoint* a = (CvPoint*)_a;
  CvPoint* b = (CvPoint*)_b;
  int y_diff = a->y - b->y;
  int x_diff = a->x - b->x;
  return y_diff ? y_diff : x_diff;
}

// sorts sequences by coordinates
static int cvSeqCmp( const void* _a, const void* _b, void* userdata ){
  CvRect a = (*((CvContour**)_a))->rect;
  CvRect b = (*((CvContour**)_b))->rect;
  return a.x - b.x;
}

// is_equal_quad
static int cvQuadCmp( const void* _a, const void* _b, void* userdata ){
  CvRect a = (*((CvContour**)_a))->rect;
  CvRect b = (*((CvContour**)_b))->rect;
  int dx =  MIN( b.x + b.width - 1, a.x + a.width - 1) - MAX( b.x, a.x);
  int dy =  MIN( b.y + b.height - 1, a.y + a.height - 1) - MAX( b.y, a.y);
  int w = (a.width + b.width) >> 1;
  int h = (a.height + b.height) >> 1;
  if( dx > w*0.75 && dy > h*0.75 && dx < w*1.25 && dy < h*1.25 ) return 1;
  return 0;
}

