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

CTemplate::CTemplate(CvCapture* capture, bool colorFilter)
{
  checkROI=0;
  IplImage* temFrame=NULL;
  imgTemplate=NULL;
  int c;
  activeColorFilter=colorFilter;
  cvNamedWindow( "Camera", 1 );
  cvSetMouseCallback( "Camera", mouseHandler, NULL );
    for(;;)
    {
        temFrame = cvQueryFrame( capture );
	if (checkROI==1){
	    cvRectangle(temFrame,
		    cvPoint(mouseDown.x, mouseDown.y),
		    cvPoint(mouseUp.x, mouseUp.y),
		    cvScalar(0, 0, 255, 0), 2, 8, 0);
	    
	}
	else if (checkROI==-1)
	{
	  PRINT(mouseDown.x);
	  PRINT(mouseDown.y);
	  PRINT(mouseUp.x);
	  PRINT(mouseUp.y);
	  cvSetImageROI(temFrame, cvRect(mouseDown.x, mouseDown.y,mouseUp.x-mouseDown.x, mouseUp.y- mouseDown.y));
          imgTemplate = cvCreateImage(cvGetSize(temFrame), temFrame->depth, temFrame->nChannels);

          if (activeColorFilter)
          {
               IplImage* imgFiltered;
               imgFiltered = getFilteredImage(temFrame);
               imgTemplate=imgFiltered;
               //cvCopy(imgFiltered, imgTemplate, NULL);
          }
          else
          {
              cvCopy(temFrame, imgTemplate, NULL);
          }
          cvResetImageROI(temFrame);
	  break;
	}
	cvShowImage("Camera", temFrame);
	c = cvWaitKey(30);
        if( (char) c == 27 )
            break;
    }
    //Debug (Show template)
    CREATE_WINDOW("Template")
    PUBLISH_WINDOW("Template",imgTemplate)
    cvDestroyWindow("Camera");
}

CTemplate::~CTemplate()
{
  //Debug close template window
  CLOSE_WINDOW("Template")
  
  cvReleaseImage(&imgTemplate);
}

IplImage *CTemplate::getFilteredImage(IplImage *original)
{
    IplImage* imgHSV = cvCreateImage(cvGetSize(original), 8, 3);
    IplImage* imgFiltered = cvCreateImage(cvGetSize(original), 8, 3);
    IplImage* imgThreshed = cvCreateImage(cvGetSize(original), 8, 1);

    cvZero(imgFiltered);

    cvCvtColor(original, imgHSV, CV_BGR2HSV);
    cvInRangeS(imgHSV, cvScalar(0,30,60), cvScalar(20, 150, 255), imgThreshed);
    cvOrS(original, CvScalar(),imgFiltered, imgThreshed);

    cvReleaseImage(&imgHSV);
    cvReleaseImage(&imgThreshed);
    return imgFiltered;
}

//Obsolete funcion of the pre-first version
position CTemplate::getMoments(IplImage *img)
{
    position pos;
    double moment01, moment10, area;
    CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
    cvMoments(img, moments, 1);
    
    moment10=cvGetSpatialMoment(moments, 1, 0);
    moment01=cvGetSpatialMoment(moments, 0, 1);

    area = cvGetCentralMoment(moments, 0, 0);
   
    pos.x = moment10/area;
    pos.y = moment01/area;

    return pos;
}

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



CvSize CTemplate::getSize(){
  return cvGetSize(imgTemplate);
}

position CTemplate::getNewPosition(IplImage * frame)
{
    IplImage* imgResult=NULL;
    double min_val=0, max_val=0;
    CvPoint min_loc, max_loc;
    CvSize resolution;
    position pos;
    resolution=cvGetSize(frame);

    imgResult = cvCreateImage(cvSize(resolution.width-imgTemplate->width+1,resolution.height-imgTemplate->height+1), IPL_DEPTH_32F, 1);
    cvZero(imgResult);
  
    //Color filter code
    if (activeColorFilter)
    {
	IplImage* imgFiltered;
        imgFiltered = getFilteredImage(frame);
        cvMatchTemplate(imgFiltered, imgTemplate, imgResult, CV_TM_CCORR_NORMED);
        cvReleaseImage(&imgFiltered);
    }
    else{	
        cvMatchTemplate(frame, imgTemplate, imgResult, CV_TM_CCORR_NORMED);
    }
    cvMinMaxLoc(imgResult, &min_val, &max_val, &min_loc, &max_loc);
    PRINT(max_val);
    //printf("%f\n", max_val);
    if ((max_val>=0.95 && !activeColorFilter) || (max_val>=0.40 && activeColorFilter))
    {
        pos.x=float(max_loc.x+(imgTemplate->width/2));
	pos.y=float(max_loc.y+(imgTemplate->height/2));
   }
   else 
   {
     pos.x=-1;
     pos.y=-1;
   }
   cvReleaseImage(&imgResult);
   return pos;
}




