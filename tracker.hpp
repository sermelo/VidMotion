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
#ifndef TRACKER_H
#define TRACKER_H

#define CV_NO_BACKWARD_COMPATIBILITY

//Defining debug functions
#ifndef NDEBUG
#define PRINT(x)
#define CREATE_WINDOW(x)
#define PUBLISH_WINDOW(x,y)
#define CLOSE_WINDOW(x)
#else
#define PRINT(x) \
std::cout << #x << ":\t" << x << std::endl;
#define CREATE_WINDOW(x) \
namedWindow( x, 1 );
#define PUBLISH_WINDOW(x,y) \
imshow(x, y);
#define CLOSE_WINDOW(x) \
destroyWindow(x);
#endif

#include "cv.h"
#include "highgui.h"

using namespace cv;
using namespace std;

struct position {
   int x;
   int y;
};

class CTracker {
   protected:
       Mat imgTemplate;
	
   public:
      virtual position getNewPosition(Mat frame)=0;
      virtual position getNewPosition(Mat frame, Rect region)=0;
      virtual Size getSize()=0;
};
#endif
