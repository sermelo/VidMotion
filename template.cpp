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

#include "template.hpp"

position CTemplate::mouseDown;
position CTemplate::mouseUp;
int CTemplate::checkROI;

//Function to allow user to select a region to track later
CTemplate::CTemplate(VideoCapture capture, bool colorFilter)
{
  checkROI=0;
  Mat temFrame, tmpROI;
  int c,aux;
  activeColorFilter=colorFilter;
  cvNamedWindow( "Camera", 1 );
  cvSetMouseCallback( "Camera", mouseHandler, NULL );
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
		    cvScalar(0, 0, 255, 0), 2, 8, 0);
	    
	}
        //Case of finished to select a region
	else if (checkROI==-1)
	{
	  rectangle(temFrame,
		    Point(mouseDown.x, mouseDown.y),
		    Point(mouseUp.x, mouseUp.y),
		    cvScalar(0, 0, 255, 0), 2, 8, 0);
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
          if (activeColorFilter)
          {
               createHSVFilter(temFrame);
               imgTemplate = getFilteredImage(imgTemplate);
          }
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

CTemplate::~CTemplate()
{

}

//Create a customize HSV filter
void CTemplate::createHSVFilter(Mat img)
{
  int LHue=0;
  int LSat=0;
  int LVal=0;
  int HHue=255;
  int HSat=255;
  int HVal=255;
  int c;
  Mat modifiedImg;
  //modifiedImg=cvCreateImage(cvGetSize(img), img->depth, img->nChannels);

  namedWindow( "HSV Filter", 0 );
  cvResizeWindow("HSV Filter", 1200, 680);
  createTrackbar("Min Hue", "HSV Filter", &LHue, 255, NULL);
  createTrackbar("Min Sat", "HSV Filter", &LSat, 255, NULL);
  createTrackbar("Min Val", "HSV Filter", &LVal, 255, NULL);
  createTrackbar("Max Hue", "HSV Filter", &HHue, 255, NULL);
  createTrackbar("Max Sat", "HSV Filter", &HSat, 255, NULL);
  createTrackbar("Max Val", "HSV Filter", &HVal, 255, NULL);
  while(1){
      //change the brightness of the image

      modifiedImg=getFilteredImage(img,Scalar(LHue, LSat, LVal),Scalar(HHue, HSat, HVal));
      imshow("HSV Filter", modifiedImg);
      c=waitKey(100);
      if((char) c==10){      
	  break;
      }
  }
  lowFilter=Scalar(LHue, LSat, LVal);
  highFilter=Scalar(HHue, HSat, HVal);
  CLOSE_WINDOW("HSV Filter")
}

//This function apply a customize HSV filter
Mat CTemplate::getFilteredImage(Mat original,Scalar low, Scalar high)
{
    Mat imgHSV;
    Mat imgFiltered;
    Mat imgThreshed;

    cvtColor(original, imgHSV, CV_BGR2HSV);
    inRange(imgHSV, low, high, imgThreshed);
    original.copyTo(imgFiltered, imgThreshed);
    return imgFiltered;
}

//This function apply a HSV filter saved previously
Mat CTemplate::getFilteredImage(Mat original)
{
    Mat imgHSV;
    Mat imgFiltered;
    Mat imgThreshed;

    cvtColor(original, imgHSV, CV_BGR2HSV);
    inRange(imgHSV, lowFilter, highFilter, imgThreshed);
    original.copyTo(imgFiltered, imgThreshed);

    return imgFiltered;
}


//This function filter an image using skin filter(depending o the ligh and the skin
Mat CTemplate::getFilteredImageSkin(Mat original)
{
    Mat imgHSV;
    Mat imgFiltered;
    Mat imgThreshed;

    cvtColor(original, imgHSV, CV_BGR2HSV);
    inRange(imgHSV, Scalar(0,30,60), Scalar(20, 150, 255), imgThreshed);
    original.copyTo(imgFiltered, imgThreshed);
    
    return imgFiltered;
}

//Mouse callback
void CTemplate::mouseHandler(int event, int x, int y, int flags, void *param)
{
    switch(event) {
        /* left button down */
        case CV_EVENT_LBUTTONDOWN:
	    CTemplate::mouseDown.x=x;
	    CTemplate::mouseDown.y=y;
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
Size CTemplate::getSize(){
  return Size(imgTemplate.cols,imgTemplate.rows);
  //return cvGetSize(imgTemplate);
}

//Look for in the image the center of the region that fits best with the template
position CTemplate::getNewPosition(Mat frame)
{
    Mat imgResult, imgFiltered;
    double min_val=0, max_val=0;
    Point min_loc, max_loc;
    Size resolution;
    position pos;
    resolution=Size(frame.cols,frame.rows);
    //Color filter code
    if (activeColorFilter)
    {
	imgFiltered= getFilteredImage(frame);
        matchTemplate(imgFiltered, imgTemplate, imgResult, CV_TM_CCORR_NORMED);
    }
    else{	
        matchTemplate(frame, imgTemplate, imgResult, CV_TM_CCORR_NORMED);
    }
    PUBLISH_WINDOW( "Result", imgResult);
    minMaxLoc(imgResult, &min_val, &max_val, &min_loc, &max_loc);
    PRINT(max_val);
    if ((max_val>=0.9 && !activeColorFilter) || (max_val>=0.40 && activeColorFilter))
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



//Look for in the image the center of the region that fits best with the template. It will look for in only in a region
position CTemplate::getNewPosition(Mat frame, Rect region)
{
    Mat imgResult, imgRegion, tmpROI, imgFiltered;
    double min_val=0, max_val=0;
    Point min_loc, max_loc;
    Size resolution;
    position pos;
    
    resolution=Size(frame.cols,frame.rows);
    
    tmpROI=frame(region);
    tmpROI.copyTo(imgRegion);
	   
    //Color filter code
    if (activeColorFilter)
    {
	imgFiltered= getFilteredImage(imgRegion);
        matchTemplate(imgFiltered, imgTemplate, imgResult, CV_TM_CCORR_NORMED);
    }
    else{	
        matchTemplate(imgRegion, imgTemplate, imgResult, CV_TM_CCORR_NORMED);
    }
    PUBLISH_WINDOW( "Result", imgResult);
    minMaxLoc(imgResult, &min_val, &max_val, &min_loc, &max_loc);
    PRINT(max_val);
    //printf("%f\n", max_val);
    if ((max_val>=0.9 && !activeColorFilter) || (max_val>=0.40 && activeColorFilter))
    {
        pos.x=float(max_loc.x+(imgTemplate.cols/2)+region.x);
	pos.y=float(max_loc.y+(imgTemplate.rows/2)+region.y);
   }
   else 
   {
     pos.x=-1;
     pos.y=-1;
   }
   return pos;
}


position CTemplate::getNewPositionSurf(Mat img_1, Mat img_2)
{ 

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


}

