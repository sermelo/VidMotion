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

#ifndef NDEBUG
#define ADDRECT(x,y,z)
#define PRINT(x)
#define CREATE_WINDOW(x)
#define PUBLISH_WINDOW(x,y)
#define CLOSE_WINDOW(x)
#else
#define ADDRECT(x,y,z) \
cvRectangle(x, y, z,cvScalar(0, 0, 255, 0), 2, 8, 0);
#define PRINT(x) \
std::cout << #x << ":\t" << x << std::endl;
#define CREATE_WINDOW(x) \
cvNamedWindow( x, 1 );
#define PUBLISH_WINDOW(x,y) \
cvShowImage(x, y);
#define CLOSE_WINDOW(x) \
cvDestroyWindow(x);
#endif


#include "cv.h"
#include "highgui.h"

#include "mouse.hpp"
#include "template.hpp"



int main_loop( CvCapture* capture, CTemplate Pattern, CCursor Mouse, CvRect region )
{
    CREATE_WINDOW("Tracking");
    coordinates pos,prevPos;
    position auxPos;
    CvSize resolution;
    CvSize patternSize;
    int c;
    
    patternSize=Pattern.getSize();
    pos.x=0;
    pos.y=0;
    auxPos.x=0;
    auxPos.y=0;
    prevPos.x=0;
    prevPos.y=0;

    
    
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
        auxPos=Pattern.getNewPosition(frame, region);

	if (auxPos.x!=-1){
	    prevPos.x = pos.x;
	    prevPos.y = pos.y;
	    pos.x=float(1)-((float(auxPos.x)-region.x-(patternSize.width/2))/(region.width-(patternSize.width)));
	    pos.y=(float(auxPos.y)-region.y-(patternSize.height/2))/(region.height-(patternSize.height));
	    
	    cvRectangle(frame, cvPoint(auxPos.x-(patternSize.width/2), auxPos.y-(patternSize.height/2)), cvPoint(auxPos.x+(patternSize.width/2), auxPos.y+(patternSize.height/2)), cvScalar(0), 1);
	    cvCircle(frame, cvPoint(auxPos.x, auxPos.y),5, cvScalar(0), -1);

	    //printf("Movemos a : %f,%f\n",auxPos.x,auxPos.y);
	    PRINT(pos.x);
	    PRINT(pos.y);
	    Mouse.setAbsPercentPosition(pos);
	}
	c = cvWaitKey(30);
        if( (char) c == 27 )
            break;
	ADDRECT(frame,cvPoint(region.x, region.y), cvPoint(region.x+region.width, region.y+region.height));
	PUBLISH_WINDOW("Tracking",frame);
    }
    CLOSE_WINDOW("Tracking");
    
    return 0;
}

bool getFilterOption(int argc, char** argv )
{
    bool result=false;
    for (int i=0;i<argc;i++)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            result=true;
            break;
        }
    }
    return result;
}

int getCameraDevice(int argc, char** argv )
{
    int result=0;
    for (int i=0;i<argc;i++)
    {
        if (strcmp(argv[i], "-c") == 0)
        {
            result=atoi(argv[i+1]);
            break;
        }
    }
    return result;
}

int getRegionOption(int argc, char** argv )
{
    int result=false;
    for (int i=0;i<argc;i++)
    {
        if (strcmp(argv[i], "-R") == 0)
        {
            result=true;
            break;
        }
    }
    return result;
}

CvRect autoRegion(CvCapture* capture, CTemplate Pattern)
{
    IplImage* frame = 0;
    CvSize resolution, patternSize;
    patternSize=Pattern.getSize();
    frame = cvQueryFrame( capture );
    if( !frame ){
       exit(-1);
    }
    else
    {
      resolution=cvGetSize(frame);
    }
    CvRect region=cvRect(0, 0, 0, 0);
    region.width=resolution.width;
    region.height=resolution.height;
    return region;
}

CvRect chooseRegion(CvCapture* capture, CTemplate Pattern)
{
    int points=0;
    CvRect region=cvRect(0, 0, 0, 0);
    cvNamedWindow( "Region", 1 );
    position pos,prevPos;
    position auxPos;
    CvSize resolution;
    CvSize patternSize;
    int c;
    
    patternSize=Pattern.getSize();
    pos.x=0;
    pos.y=0;
    auxPos.x=0;
    auxPos.y=0;
    prevPos.x=0;
    prevPos.y=0;

    
    
    IplImage* frame = 0;
    frame = cvQueryFrame( capture );
    if( !frame ){
       return region;
    }
    else
    {
      resolution=cvGetSize(frame);
    }

    for(;;)
    {
        frame = cvQueryFrame( capture );
        auxPos=Pattern.getNewPosition(frame);
	if (auxPos.x!=-1){
	    prevPos.x = pos.x;
	    prevPos.y = pos.y;
	    pos.x=auxPos.x;
	    pos.y=auxPos.y;
	    cvRectangle(frame, cvPoint(auxPos.x-(patternSize.width/2), auxPos.y-(patternSize.height/2)), cvPoint(auxPos.x+(patternSize.width/2), auxPos.y+(patternSize.height/2)), cvScalar(0), 1);
	    cvCircle(frame, cvPoint(auxPos.x, auxPos.y),5, cvScalar(0), -1);
	}
	if (points==1)
	{
	   cvRectangle(frame, cvPoint(region.x, region.y), cvPoint(pos.x, pos.y),cvScalar(0, 0, 255, 0), 2, 8, 0);
	}
	cvShowImage("Region", frame);
	c = cvWaitKey(30);
	//fprintf(stderr,"key captured: %d\n",(char) c);
        if( (char) c == 10 )
	{
	    if (points==0)
	    {
	        region.x=pos.x;
	        region.y=pos.y;
		points=1;
		
	    }
	    else
	    {
	        if (pos.x<region.x)
		{
		       region.width=region.x-pos.x;
		       region.x=pos.x;
		}
		else
		{
		       region.width=pos.x-region.x;
		}
		if (pos.y<region.y)
		{
		       region.height=region.y-pos.y;
		       region.y=pos.y;
		}
		else
		{
		       region.height=pos.y-region.y;
		}
		region.x=region.x-patternSize.width/2;
		region.y=region.y-patternSize.height/2;
		region.width=region.width+patternSize.width;
		region.height=region.height+patternSize.height;
		PRINT(region.x);
		PRINT(region.y);
		PRINT(region.width);
		PRINT(region.height)
                break;
	    }
        }
    }
    cvDestroyWindow("Region");
    return region;
}

int main( int argc, char** argv )
{
    int camera;
    bool filter;
    bool reg;
    CvRect region=cvRect(0, 0, 0, 0);
    //Read optional parameters
    filter=getFilterOption(argc,argv);
    camera=getCameraDevice(argc,argv);
    reg=getRegionOption(argc,argv);
    
    CvCapture* capture = 0;
    capture = cvCaptureFromCAM(camera);

    if( !capture )
    {
        fprintf(stderr,"Could not initialize capturing...\n");
        return -1;
    }
    //Init mouse object
    CCursor Mouse;
    //Init object patern
    CTemplate Pattern(capture,filter);
    //Select region to explore
    if (reg)
    {
        region=chooseRegion(capture,Pattern);
    }
    else
    {
        region=autoRegion(capture,Pattern);
    }
    //Start main loop
    main_loop(capture,Pattern,Mouse,region);

    //cvReleaseCapture( &capture );
    return 0;
}
