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
#include "tracker.hpp"

struct surfData {
   vector< cv::KeyPoint > keypoints;
   Mat descriptors;
};

class CsurfTemplate : public CTracker{
   private:
      static position mouseDown;
      static position mouseUp;
      static int checkROI;
      Mat imgTemplate;
      static void mouseHandler(int event, int x, int y, int flags, void *param);
      
      surfData createSurfData(Mat image);
      surfData templateSurf;
   public:
      CsurfTemplate(VideoCapture capture,bool colorFilter=false );
      ~CsurfTemplate();
      position getNewPosition(Mat frame);
      position getNewPosition(Mat frame, Rect region);
      Size getSize();
};