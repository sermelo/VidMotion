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
    Mat imgResult, imgFiltered;
    double min_val=0, max_val=0;
    Point min_loc, max_loc;
    Size resolution;
    position pos;
    resolution=Size(frame.cols,frame.rows);
    //Color filter code
	
        matchTemplate(frame, imgTemplate, imgResult, CV_TM_CCORR_NORMED);

    PUBLISH_WINDOW( "Result", imgResult);
    minMaxLoc(imgResult, &min_val, &max_val, &min_loc, &max_loc);
    PRINT(max_val);
    if ((max_val>=0.9))
    {
        pos.x=float(max_loc.x+(imgTemplate.cols/2));
	pos.y=float(max_loc.y+(imgTemplate.rows/2));
   }
   else 
   {
     pos.x=-1;
     pos.y=-1;
   }
   PRINT(pos.x);
   PRINT(pos.y);
   return pos;
}


position CsurfTemplate::getNewPosition(Mat frame, Rect region)
{ 
  position pos;
  return pos;
/*
 //position CsurfTemplate::getNewPositionSurf(Mat img_1, Mat img_2)
    if( !img_1.data || !img_2.data )
    { 
	std::cout<< " --(!) Error reading images " << std::endl; 

    }




    //-- Step 1: Detect the keypoints using SURF Detector
    int minHessian = 400;
    SurfFeatureDetector detector( minHessian );
    std::vector<KeyPoint> keypoints_1, keypoints_2;

    std::vector< DMatch > good_matches;

    do{ 

    detector.detect( img_1, keypoints_1 );
    detector.detect( img_2, keypoints_2 );

    //-- Draw keypoints

    Mat img_keypoints_1; Mat img_keypoints_2;
    drawKeypoints( img_1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
    drawKeypoints( img_2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT );

    //-- Step 2: Calculate descriptors (feature vectors)
    SurfDescriptorExtractor extractor;
    Mat descriptors_1, descriptors_2;
    extractor.compute( img_1, keypoints_1, descriptors_1 );
    extractor.compute( img_2, keypoints_2, descriptors_2 );


    //-- Step 3: Matching descriptor vectors using FLANN matcher
    FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match( descriptors_1, descriptors_2, matches );
    double max_dist = 0; 
    double min_dist = 100;

    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptors_1.rows; i++ )
    { 
	double dist = matches[i].distance;
    if( dist < min_dist )
	min_dist = dist;
    if( dist > max_dist ) 
	max_dist = dist;
    }

    printf("-- Max dist : %f \n", max_dist );
    printf("-- Min dist : %f \n", min_dist );

    //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
    //-- PS.- radiusMatch can also be used here.



    for( int i = 0; i < descriptors_1.rows; i++ )
    { 
	if( matches[i].distance < 2*min_dist )
	    { 
		    good_matches.push_back( matches[i]);
	    }
    }

    }while(good_matches.size()<50);

    //-- Draw only "good" matches
    Mat img_matches;
    drawMatches( img_1, keypoints_1, img_2, keypoints_2,good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
    vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    //-- Localize the object
    std::vector<Point2f> obj;
    std::vector<Point2f> scene;
    for( int i = 0; i < good_matches.size(); i++ )
    {
    //-- Get the keypoints from the good matches
    obj.push_back( keypoints_1[ good_matches[i].queryIdx ].pt );
    scene.push_back( keypoints_2[ good_matches[i].trainIdx ].pt );
    }

    // se non trova H....sarebbe da usare la vecchia H e disegnarla con un colore diverso
    Mat H = findHomography( obj, scene, CV_RANSAC );


    //-- Get the corners from the image_1 ( the object to be "detected" )
    std::vector<Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0,0); 
    obj_corners[1] = cvPoint( img_1.cols, 0 );
    obj_corners[2] = cvPoint( img_1.cols, img_1.rows ); 
    obj_corners[3] = cvPoint( 0, img_1.rows );
    std::vector<Point2f> scene_corners(4);


    perspectiveTransform( obj_corners, scene_corners, H);


    //-- Draw lines between the corners (the mapped object in the scene - image_2 )
      line( img_matches, scene_corners[0] + Point2f( img_1.cols, 0), scene_corners[1] + Point2f( img_1.cols, 0), Scalar(0, 255, 0), 4 );
      line( img_matches, scene_corners[1] + Point2f( img_1.cols, 0), scene_corners[2] + Point2f( img_1.cols, 0), Scalar( 0, 255, 0), 4 );
      line( img_matches, scene_corners[2] + Point2f( img_1.cols, 0), scene_corners[3] + Point2f( img_1.cols, 0), Scalar( 0, 255, 0), 4 );
      line( img_matches, scene_corners[3] + Point2f( img_1.cols, 0), scene_corners[0] + Point2f( img_1.cols, 0), Scalar( 0, 255, 0), 4 );

    //-- Show detected matches
    imshow( "Good Matches & Object detection", img_matches );

    for( int i = 0; i < good_matches.size(); i++ )
      { 
	printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx ); 
    }

    //waitKey(0);

*/
}