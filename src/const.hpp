/**********************************************************************
 * --------------------------------------------------------------------
 * Automatic Licence Plate Recognition
 * --------------------------------------------------------------------
 *
 * File:        const.hpp
 * Author:      Tambet Masik
 * Created:     2008.04.27
 * Description: System constants
 *
 **********************************************************************/
 
#include <float.h>
 
//#define DEBUG               1
#define ESC                 27
#define FALSE               0
#define TRUE                1 

#define NO_USERDATA         0
#define CHANNELS_GRAY       1
#define NO_SCALE            1
#define UPDATE_HEADER       1
#define MIN_SKEW_ANGLE      1
#define RESIZE_MULTIPLIER   2
#define	CHANNELS_RGB        3
#define	RECT_VERTICES       4
#define FILENAME_SIZE       25
#define BLOCK_SIZE          45
#define CANNY_THRESHOLD     50
#define MIN_GLYPH_AREA      50
#define ADAPTIVE_BLOCK      71
#define ADAPTIVE_OFFSET     15
#define THRESHOLD_LEVEL     110
#define PLATE_MIN_AREA      450
#define PLATE_MAX_AREA      25000
#define MIN_AREA            2000    

#define MIN_ANGLE           0.98
#define HEIGHT_WIDTH_RATIO  0.30

// Color definitions
#define CVX_RED     CV_RGB(0xff,0x00,0x00)
#define CVX_GREEN   CV_RGB(0x00,0xff,0x00)
#define CVX_BLUE    CV_RGB(0x00,0x00,0xff)
#define CVX_BLACK   CV_RGB(0x00,0x00,0x00)
#define CVX_WHITE   CV_RGB(0xff,0xff,0xff)

