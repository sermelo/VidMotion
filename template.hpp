/*
    This file is part of VidMotion.

    VidMotion is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VidMotion is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VidMotion.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TEMPLATE_H
#define TEMPLATE_H

#define CV_NO_BACKWARD_COMPATIBILITY


#ifndef NDEBUG
#define PRINT(x)
#else
#define PRINT(x) \
std::cout << #x << ":\t" << x << std::endl;
#endif

#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <ctype.h>

#include <assert.h>
#include <unistd.h>
#include <malloc.h>

struct position {
   int x;
   int y;
};

class CTemplate{
   private:
      static position mouseDown;
      static position mouseUp;
      static int checkROI;
      IplImage* imgTemplate;
      
      IplImage *getThreshold(IplImage *original);
      position getMoments(IplImage *img);
      static void mouseHandler(int event, int x, int y, int flags, void *param);
      
   public:
      CTemplate(CvCapture* capture );
      position getNewPosition(IplImage * frame);
      CvSize getSize();
};
#endif