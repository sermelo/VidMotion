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
#include "surfTemplate.hpp"

position CsurfTemplate::mouseDown;
position CsurfTemplate::mouseUp;
int CsurfTemplate::checkROI;

//Function to allow user to select a region to track later
CsurfTemplate::CsurfTemplate(VideoCapture capture, bool colorFilter)
{
  checkROI=0;
  Mat temFrame, tmpROI;
  int c,aux;
  namedWindow( "Camera", 1 );
  setMouseCallback( "Camera", mouseHandler, NULL );
    for(;;)
    {
        capture >> temFrame; // get a new frame from camera
        //In case that selected region have 0 width or 0 height
        if ((mouseUp.x==mouseDown.x || mouseUp.y==mouseDown.y) && checkROI==-1){
        	checkROI=0;
	}
        //Case mouse still not button up
        
	else if (checkROI==1){
	    rectangle(temFrame,
		    Point(mouseDown.x, mouseDown.y),
		    Point(mouseUp.x, mouseUp.y),
		    Scalar(0, 0, 255, 0), 2, 8, 0);
	}
        //Case of finished to select a region
	else if (checkROI==-1)
	{
	  rectangle(temFrame,
		    Point(mouseDown.x, mouseDown.y),
		    Point(mouseUp.x, mouseUp.y),
		    Scalar(0, 0, 255, 0), 2, 8, 0);
          if (mouseUp.x<mouseDown.x)
          {
              aux=mouseUp.x;
              mouseUp.x=mouseDown.x;
              mouseDown.x=aux;
          }
          if (mouseUp.y<mouseDown.y)
          {
              aux=mouseUp.y;
              mouseUp.y=mouseDown.y;
              mouseDown.y=aux;
          }
	  PRINT(mouseDown.x);
	  PRINT(mouseDown.y);
	  PRINT(mouseUp.x);
	  PRINT(mouseUp.y);
	  Rect myROI(mouseDown.x, mouseDown.y, mouseUp.x-mouseDown.x, mouseUp.y-mouseDown.y);
	  tmpROI=temFrame(myROI);
	  tmpROI.copyTo(imgTemplate);
	  templateSurf=createSurfData(imgTemplate);
	  break;
	}
	imshow("Camera", temFrame);
	c = waitKey(30);
        if( (char) c == 27 )
            break;
    }
    //Debug (Show template)
    PUBLISH_WINDOW("Template",imgTemplate);
    CLOSE_WINDOW("Camera")
}

CsurfTemplate::~CsurfTemplate()
{

}

//Create surf points
surfData CsurfTemplate::createSurfData(Mat image)
{
  int minHessian = 400;
  surfData result;
  Mat descriptors;
  vector<KeyPoint> imgPoints;
  SurfFeatureDetector surf( minHessian );
  SurfDescriptorExtractor extractor;
  
  surf.detect( image, imgPoints );
  extractor.compute(image,imgPoints,descriptors);
  result.keypoints=imgPoints;
  result.descriptors=descriptors;
  return result;
}

//Mouse callback
void CsurfTemplate::mouseHandler(int event, int x, int y, int flags, void *param)
{
    switch(event) {
        /* left button down */
        case CV_EVENT_LBUTTONDOWN:
	    CsurfTemplate::mouseDown.x=x;
	    CsurfTemplate::mouseDown.y=y;
	    mouseUp.x=x;
	    mouseUp.y=y;
	    checkROI=1;
	    PRINT(mouseDown.x);
	    PRINT(mouseDown.y);
            break;
       /* left button up */
       case CV_EVENT_LBUTTONUP:
	    mouseUp.x=x;
	    mouseUp.y=y;
	    checkROI=-1;
	    PRINT(mouseUp.x);
	    PRINT(mouseUp.y);
            break;

	    
        case CV_EVENT_MOUSEMOVE:
	    if (checkROI!=0)
	    {
	        mouseUp.x=x;
	        mouseUp.y=y;
	    }

            break;
    }
}


//Return template size
Size CsurfTemplate::getSize(){
  return Size(imgTemplate.cols,imgTemplate.rows);
}

//Look for in the image the center of the region that fits best with the template
position CsurfTemplate::getNewPosition(Mat frame)
{
    position pos;
    surfData frameData;
    frameData=createSurfData(frame);
    FlannBasedMatcher matcher;

    vector< DMatch > matches;
    matcher.match( templateSurf.descriptors, frameData.descriptors, matches );

    double max_dist = 0; double min_dist = 100;

  for( int i = 0; i < templateSurf.descriptors.rows; i++ )
  { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }
  std::vector< DMatch > good_matches;

  for( int i = 0; i < templateSurf.descriptors.rows; i++ )
  { 

    if( matches[i].distance < 3*min_dist )
    { 
      good_matches.push_back( matches[i]); 
    }
  }

  Mat img_matches;
  /*drawMatches( imgTemplate, templateSurf.keypoints, frame, frameData.keypoints,
               good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
*/

  std::vector<Point2f> obj;
  std::vector<Point2f> scene;

  for( int i = 0; i < good_matches.size(); i++ )
  {
    obj.push_back( templateSurf.keypoints[ good_matches[i].queryIdx ].pt );
    scene.push_back( frameData.keypoints[ good_matches[i].trainIdx ].pt );
  }
  if (good_matches.size()>=4)
  {
    Mat H = findHomography( obj, scene, CV_RANSAC );


    std::vector<Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( imgTemplate.cols, 0 );
    obj_corners[2] = cvPoint( imgTemplate.cols, imgTemplate.rows ); obj_corners[3] = cvPoint( 0, imgTemplate.rows );
    std::vector<Point2f> scene_corners(4);

    perspectiveTransform( obj_corners, scene_corners, H);

    pos.x=(scene_corners[2].x+scene_corners[0].x)/2;
    pos.y=(scene_corners[2].y+scene_corners[0].y)/2;
    PRINT(pos.x);
    PRINT(pos.y);
  }
  else
  {
    pos.x=-1;
    pos.y=-1;
  }
  return pos;

}


position CsurfTemplate::getNewPosition(Mat frame, Rect region)
{ 
   Mat imgRegion, tmpROI;
   position pos;  
   
   tmpROI=frame(region);
   tmpROI.copyTo(imgRegion);

   pos=getNewPosition(imgRegion);
   if (pos.x>=0){
   pos.x=pos.x+region.x;
   pos.y=pos.y+region.y;
   }
   return pos;
}