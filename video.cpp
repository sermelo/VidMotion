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

#define CV_NO_BACKWARD_COMPATIBILITY

#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <ctype.h>

#include <assert.h>
#include <unistd.h>
#include <malloc.h>

#include "mouse.hpp"


coordinates mouseDown;
coordinates mouseUp;
int checkROI;
IplImage* imgTemplate;

IplImage *getThreshold(IplImage *original)
{
    IplImage* imgHSV = cvCreateImage(cvGetSize(original), 8, 3);
    cvCvtColor(original, imgHSV, CV_BGR2HSV);
    IplImage* imgThreshed = cvCreateImage(cvGetSize(original), 8, 1);
    cvInRangeS(imgHSV, cvScalar(0,30,60), cvScalar(20, 150, 255), imgThreshed);
    return imgThreshed;
}

coordinates getMoments(IplImage *img)
{
    coordinates pos;
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

void mouseHandler(int event, int x, int y, int flags, void *param)
{
    switch(event) {
        /* left button down */
        case CV_EVENT_LBUTTONDOWN:
	    mouseDown.x=x;
	    mouseDown.y=y;
	    mouseUp.x=x;
	    mouseUp.y=y;
	    checkROI=1;
            fprintf(stdout, "Left button down (%d, %d).\n", x, y);
            break;

       case CV_EVENT_LBUTTONUP:
	    mouseUp.x=x;
	    mouseUp.y=y;
	    checkROI=-1;
            fprintf(stdout, "Left button up (%d, %d).\n", x, y);
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

IplImage *saveTemplate(CvCapture* capture )
{
  checkROI=0;
  IplImage* temFrame=0;
  IplImage* imgTemplate=NULL;
  int c;
  cvNamedWindow( "Object", 1 );
  cvNamedWindow( "Template", 1 );
  cvSetMouseCallback( "Object", mouseHandler, NULL );
  temFrame = cvQueryFrame( capture );
    if( !temFrame ){
       return imgTemplate;
    }
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
	  fprintf(stdout, "Points: (%f, %f, %f, %f).\n",mouseDown.x, mouseDown.y,mouseUp.x, mouseUp.y);
	  fprintf(stdout, "El tamaño es (%f, %f).\n",mouseDown.x-mouseUp.x, mouseDown.y- mouseUp.y);
	  cvSetImageROI(temFrame, cvRect(mouseDown.x, mouseDown.y,mouseUp.x-mouseDown.x, mouseUp.y- mouseDown.y));
          imgTemplate = cvCreateImage(cvGetSize(temFrame), temFrame->depth, temFrame->nChannels);
          cvCopy(temFrame, imgTemplate, NULL);
          cvResetImageROI(temFrame);
	  
	  
	  
	  cvShowImage("Template", imgTemplate);
 
	  break;
	}

	cvShowImage("Object", temFrame);
	c = cvWaitKey(30);
        if( (char) c == 27 )
            break;
    }
    cvDestroyWindow("Template");
    cvDestroyWindow("Object");
    return imgTemplate;
}

coordinates getNewPosition(IplImage * frame, IplImage *imgTemplate)
{
    IplImage* imgResult=NULL;
    double min_val=0, max_val=0;
    CvPoint min_loc, max_loc;
    CvSize resolution;
    coordinates pos;
    
    	/*Color filter code
	 IplImage* imgFiltered;
         IplImage* imgThreshed;
         imgFiltered = cvCreateImage(cvGetSize(frame), 8, 3);
	  imgThreshed=getThreshold(frame);
	 cvZero(imgFiltered);
         cvOrS(frame, CvScalar(),imgFiltered, imgThreshed);
         cvMatchTemplate(imgFiltered, imgTemplate, imgResult, CV_TM_CCORR_NORMED);*/
	
    resolution=cvGetSize(frame);
    imgResult = cvCreateImage(cvSize(resolution.width-imgTemplate->width+1,resolution.height-imgTemplate->height+1), IPL_DEPTH_32F, 1);
    cvZero(imgResult);
    cvMatchTemplate(frame, imgTemplate, imgResult, CV_TM_CCORR_NORMED);
    cvMinMaxLoc(imgResult, &min_val, &max_val, &min_loc, &max_loc);
    //printf("%f\n", max_val);
    if (max_val>=0.95)
    {
        pos.x=float(max_loc.x+(imgTemplate->width/2));
	pos.y=float(max_loc.y+(imgTemplate->height/2));
   }
   else 
   {
     pos.x=-1;
     pos.y=-1;
   }
   return pos;
}
int main_loop( CvCapture* capture, IplImage *imgTemplate )
{

    cvNamedWindow( "Tracking", 1 );
    CCursor mouse;

    coordinates pos,prevPos,auxPos;
    CvSize resolution;

    pos.x=0;
    pos.y=0;
    auxPos.x=0;
    auxPos.y=0;
    prevPos.x=0;
    prevPos.y=0;

    int c;
    IplImage* frame = 0;
    frame = cvQueryFrame( capture );
    if( !frame ){
       return -1;
    }
    else
    {
      resolution=cvGetSize(frame);
    }
  
    for(;;)
    {
        frame = cvQueryFrame( capture );
        prevPos.x = pos.x;
        prevPos.y = pos.y;
        auxPos=getNewPosition(frame, imgTemplate);
	if (auxPos.x!=-1){
	    prevPos.x = pos.x;
	    prevPos.y = pos.y;
	    pos.x=float(1)-float(auxPos.x)/resolution.width;
	    pos.y=float(auxPos.y)/resolution.height;
	    cvRectangle(frame, cvPoint(auxPos.x-(imgTemplate->width/2), auxPos.y-(imgTemplate->height/2)), cvPoint(auxPos.x+(imgTemplate->width/2), auxPos.y+(imgTemplate->height/2)), cvScalar(0), 1);
	    cvCircle(frame, cvPoint(auxPos.x, auxPos.y),5, cvScalar(0), -1);

	    //printf("Movemos a : %f,%f\n",auxPos.x,auxPos.y);
	    mouse.setAbsPercentPosition(pos);
	}
	c = cvWaitKey(30);
        if( (char) c == 27 )
            break;
	cvShowImage( "Tracking", frame );
    }
    cvDestroyWindow("Tracking");
    return 0;
}

int main( int argc, char** argv )
{
    CvCapture* capture = 0;

    if( argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
        capture = cvCaptureFromCAM( argc == 2 ? argv[1][0] - '0' : 0 );

    if( !capture )
    {
        fprintf(stderr,"Could not initialize capturing...\n");
        return -1;
    }
    
    IplImage *object=saveTemplate(capture);
    if( object )
    {
       main_loop(capture,object);
    }
    
    cvReleaseCapture( &capture );
    return 0;
}
