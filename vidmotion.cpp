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

#include "mouse.hpp"
#include "template.hpp"


int main_loop( CvCapture* capture, CTemplate Pattern, CCursor Mouse )
{

    cvNamedWindow( "Tracking", 1 );

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
        auxPos=Pattern.getNewPosition(frame);
	if (auxPos.x!=-1){
	    prevPos.x = pos.x;
	    prevPos.y = pos.y;
	    pos.x=float(1)-float(auxPos.x)/resolution.width;
	    pos.y=float(auxPos.y)/resolution.height;
	    cvRectangle(frame, cvPoint(auxPos.x-(patternSize.width/2), auxPos.y-(patternSize.height/2)), cvPoint(auxPos.x+(patternSize.width/2), auxPos.y+(patternSize.height/2)), cvScalar(0), 1);
	    cvCircle(frame, cvPoint(auxPos.x, auxPos.y),5, cvScalar(0), -1);

	    //printf("Movemos a : %f,%f\n",auxPos.x,auxPos.y);
	    Mouse.setAbsPercentPosition(pos);
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
    //Init mouse object
    CCursor Mouse;
    //Init object patern
    CTemplate Pattern(capture);
    //Start main loop
    main_loop(capture,Pattern,Mouse );

    cvReleaseCapture( &capture );
    return 0;
}
