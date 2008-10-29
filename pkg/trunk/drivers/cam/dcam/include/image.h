/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

#ifndef IMAGE_H
#define IMAGE_H

#include <stdlib.h>
#include <malloc.h>
#include <stdarg.h>
#include <math.h>
#include <ctype.h>

#ifdef WIN32
#pragma warning(disable:4996)	// get rid of POSIX deprecation errors
#include <time.h>
#include "pstdint.h"		// MSVC++ doesn't have stdint.h
#else
#include <sys/time.h>
#include <stdint.h>
#endif

#include <cv.h>
#include <cxmisc.h>
#include <cvaux.h>
#include <highgui.h>


// alignment on allocation
#define MEMALIGN(x) memalign(16,x)
#define MEMFREE free

// Pixel raw modes
// Videre stereo:
//   Mono has left/right pixels interlaced
//   Color has left/right pixels interlace, bayer pixels
//   STOC modes have rectified images, raw encodings, disparity, etc
//

#ifndef PIXEL_CODING_T
typedef enum {
  VIDERE_STEREO_MONO = 2000,
  VIDERE_STEREO_RGGB,
  VIDERE_STEREO_GRBG,
  VIDERE_STEREO_BGGR,
  VIDERE_STOC_RECT_RECT,	// left and right rectified mono
  VIDERE_STOC_RECT_DISP,	// left rectified mono, right disparity
  VIDERE_STOC_RAW_DISP_MONO,	// left raw mono, right disparity
  VIDERE_STOC_RAW_DISP_RGGB,	// left raw color, right disparity
  VIDERE_STOC_RAW_RAW_MONO,	// left and right raw, mono
  VIDERE_STOC_RAW_RAW_RGGB,	// left and right raw, color
  PIXEL_CODING_NONE		// no image info
} pixel_coding_t;
#define PIXEL_CODING_T
#endif

#ifndef COLOR_CODING_T
typedef enum {
  COLOR_CODING_MONO8 = 3000,
  COLOR_CODING_MONO16,
  COLOR_CODING_BAYER8_RGGB,
  COLOR_CODING_BAYER8_BGGR,
  COLOR_CODING_BAYER8_GBRG,
  COLOR_CODING_BAYER8_GRBG,
  COLOR_CODING_BAYER16_RGGB,
  COLOR_CODING_BAYER16_BGGR,
  COLOR_CODING_BAYER16_GBRG,
  COLOR_CODING_BAYER16_GRBG,
  COLOR_CODING_RGB8,		// RGB order
  COLOR_CODING_RGBA8,		// RGBA order
  COLOR_CODING_RGB16,		// RGB order
  COLOR_CODING_RGBA16,		// RGBA order
  COLOR_CODING_NONE		// no image info
} color_coding_t;
#define COLOR_CODING_T
#endif



namespace cam
{

  class StereoData;		// forward reference

  // monocular data structure
  // generally, all images should be on 16-byte alignment

  class ImageData
  {
    friend class StereoData;

  public:
    ImageData();
    ~ImageData();

    // image parameters
    int imWidth;
    int imHeight;

    // image data
    // these can be NULL if no data is present
    uint8_t *imRaw;		// raw image
    pixel_coding_t imRawType;	// type of raw data
    uint8_t *im;		// monochrome image
    color_coding_t imType;
    uint8_t *imColor;		// color image, always RGB32
    color_coding_t imColorType;
    uint8_t *imRect;		// rectified monochrome image
    color_coding_t imRectType;
    uint8_t *imRectColor;	// rectified color image, always RGB32
    color_coding_t imRectColorType;

    // timing
    uint64_t im_time;		// us time when the frame finished DMA into the host

    // calibration parameters
    // row major order
    double D[5];		// distortion: k1, k2, t1, t2, k3
    double K[9];		// original camera matrix
    double R[9];		// rectification matrix
    double P[12];		// projection/camera matrix 

    // raw parameter string
    char *params;		// on-camera parameters

    // buffers
    void releaseBuffers();	// get rid of all buffers

    // rectification
    bool hasRectification;	// true if valid rectification present
    bool doRectify();		// try to rectify images
    bool initRectify();		// initializes the rectification internals from the
                                //   calibration parameters

  protected:
    // rectification arrays from OpenCV
    bool initRect;		// whether arrays are initialized or not
    CvMat *rK;
    CvMat *rD;
    CvMat *rR;
    CvMat *rKp;

    CvMat* rMapxy;		// rectification table, integer format
    CvMat* rMapa;
    CvMat* mx,* my;
    IplImage* srcIm;		// temps for rectification
    IplImage* dstIm;
  };


  // stereo data structure

  class StereoData
  {
  public:
    StereoData();
    ~StereoData();

    // image parameters
    int imWidth;
    int imHeight;
    void setSize(int width, int height); // sets individual image sizes

    // left and right image data
    ImageData *imLeft;
    ImageData *imRight;

    // rectification
    bool hasRectification;

    // disparity data
    uint16_t *imDisp;		// disparity image
    int dpp;			// disparity units per pixel, e.g., 16 is 1/16 pixel per disparity
    bool hasDisparity;		// true if disparity present
    int numDisp;		// number of disparities, in pixels

    // valid stereo data rectangle
    int imDtop, imDleft;
    int imDwidth, imDheight;
    void setDispOffsets();	// reset them, based on stereo processing params

    // external parameters for undistorted images
    double T[3];		// pose of right camera in left camera coords
    double Om[3];		// rotation vector

    // reprojection matrix
    double RP[12];

    // buffers
    void releaseBuffers();	// get rid of all buffers

    // parameters
    void extractParams(char *params); // extracts params from string and puts in vars

    // stereo processing params
    int corrSize;		// correlation window size, assumed square
    int filterSize;		// size of prefilter window, assumed square (0 if none)
    int horOffset;		// horopter offset
    
    // filter thresholds
    int textureThresh;		// units???
    int uniqueThresh;		// units???

  }; 

}


#endif	// IMAGE_H