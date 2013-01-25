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
rectangle(x, y, z,Scalar(0, 0, 255, 0), 2, 8, 0);
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

#include "mouse.hpp"
#include "template.hpp"
#include "surfTemplate.hpp"



int main_loop(VideoCapture capture, CTracker * Pattern, CCursor Mouse, Rect region )
{
    Mat frame;
    coordinates pos,prevPos;
    position auxPos;
    Size resolution;
    Size patternSize;
    int c;
    
    patternSize=Pattern->getSize();
    pos.x=0;
    pos.y=0;
    auxPos.x=0;
    auxPos.y=0;
    prevPos.x=0;
    prevPos.y=0;

    capture >> frame;
    resolution=Size(frame.cols,frame.rows);
  
    for(;;)
    {
        capture >> frame;
        auxPos=Pattern->getNewPosition(frame, region);

	if (auxPos.x!=-1){
	    prevPos.x = pos.x;
	    prevPos.y = pos.y;
	    pos.x=float(1)-((float(auxPos.x)-region.x-(patternSize.width/2))/(region.width-(patternSize.width)));
	    pos.y=(float(auxPos.y)-region.y-(patternSize.height/2))/(region.height-(patternSize.height));
	    
	    rectangle(frame, Point(auxPos.x-(patternSize.width/2), auxPos.y-(patternSize.height/2)), Point(auxPos.x+(patternSize.width/2), auxPos.y+(patternSize.height/2)), Scalar(0), 1);
	    circle(frame, Point(auxPos.x, auxPos.y),5, Scalar(0), -1);
	    PRINT(pos.x);
	    PRINT(pos.y);
	    Mouse.setAbsPercentPosition(pos);
	}
	ADDRECT(frame,Point(region.x, region.y), Point(region.x+region.width, region.y+region.height));
	PUBLISH_WINDOW("Camera",frame);
	c = waitKey(30);
        if( (char) c == 27 )
            break;
    }
    CLOSE_WINDOW("Camera");
    return 0;
}



Rect autoRegion(VideoCapture capture, CTracker * Pattern)//CTemplate Pattern)//CTracker Pattern)
{
    Mat frame;
    Size resolution, patternSize;
    Rect region(0, 0, 0, 0);
    patternSize=Pattern->getSize();
    capture >> frame;
    resolution=Size(frame.cols,frame.rows);
    region.width=resolution.width;
    region.height=resolution.height;
    return region;
}

Rect chooseRegion(VideoCapture capture, CTracker * Pattern)
{
    Mat frame;
    int points=0,c;
    Rect region(0, 0, 0, 0);
    position pos,prevPos;
    position auxPos;
    Size resolution, patternSize;
    
    patternSize=Pattern->getSize();
    pos.x=0;
    pos.y=0;
    auxPos.x=0;
    auxPos.y=0;
    prevPos.x=0;
    prevPos.y=0;

    
    
    capture >> frame;
    resolution=Size(frame.cols,frame.rows);

    for(;;)
    {
        capture >> frame;
        auxPos=Pattern->getNewPosition(frame);
	if (auxPos.x!=-1){
	    prevPos.x = pos.x;
	    prevPos.y = pos.y;
	    pos.x=auxPos.x;
	    pos.y=auxPos.y;
	    rectangle(frame, Point(auxPos.x-(patternSize.width/2), auxPos.y-(patternSize.height/2)), Point(auxPos.x+(patternSize.width/2), auxPos.y+(patternSize.height/2)), Scalar(0), 1);
	    circle(frame, Point(auxPos.x, auxPos.y),5, Scalar(0), -1);
	}
	if (points==1)
	{
	   rectangle(frame, Point(region.x, region.y), Point(pos.x, pos.y),Scalar(0, 0, 255, 0), 2, 8, 0);
	}
	imshow("Region", frame);
	c = waitKey(30);
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
    destroyWindow("Region");
    return region;
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

int main( int argc, char** argv )
{
    int camera;
    bool filter;
    bool reg;
    Rect region(0, 0, 0, 0);
    //Read optional parameters
    filter=getFilterOption(argc,argv);
    camera=getCameraDevice(argc,argv);
    reg=getRegionOption(argc,argv);
    
    VideoCapture cap(camera);
    if(!cap.isOpened())// check camera connected succesfully
    {
        fprintf(stderr,"Could not initialize capturing...\n");
        return -1;
    }
    //Init mouse object
    CCursor Mouse;
    //Init object patern
//    CTemplate Pattern(cap,filter);
    CTracker * Pattern=new CTemplate(cap,filter);//=new CsurfTemplate(cap,filter);
    //Select region to explore
    if (reg)
    {
        region=chooseRegion(cap,Pattern);
    }
    else
    {
        region=autoRegion(cap,Pattern);
    }
    //Start main loop
    main_loop(cap,Pattern,Mouse,region);

    return 0;
}
