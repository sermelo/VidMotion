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

#include "mouse.hpp"



static int XlibErrorHandler(Display *display, XErrorEvent *event) {
    fprintf(stderr, "An error occured detecting the mouse position\n");
    return True;
}

CCursor::CCursor() {
    display = XOpenDisplay(NULL);
    assert(display);
    XSetErrorHandler(XlibErrorHandler);
    number_of_screens = XScreenCount(display);
    fprintf(stderr, "There are %d screens available in this X session\n", number_of_screens);
    root_windows = static_cast<Window *>(malloc(sizeof(Window) * number_of_screens));
    for (int i = 0; i < number_of_screens; i++) {
        root_windows[i] = XRootWindow(display, i);
    }
        //Display *displayMain = XOpenDisplay(NULL);
    XWindowAttributes gwa;
    Window root = DefaultRootWindow(display);
    XGetWindowAttributes(display, root, &gwa);
    width = gwa.width;
    height = gwa.height;
}

CCursor::~CCursor() {
  XCloseDisplay(display);
  free(root_windows);
}

coordinates CCursor::getPosition() {
    bool result;
    Window window_returned;
    int root_x, root_y;
    int win_x, win_y;
    unsigned int mask_return;
    coordinates pos;
    pos.x=-1;
    pos.y=-1;
    for (int i = 0; i < number_of_screens; i++) {
        result = XQueryPointer(display, root_windows[i], &window_returned,
                &window_returned, &root_x, &root_y, &win_x, &win_y,
                &mask_return);
        if (result == True) {
            break;
        }
    }
    if (result != True) {
        fprintf(stderr, "No mouse found.\n");
        return pos;
    }
//    printf("Mouse is at (%d,%d)\n", root_x, root_y);
    pos.x=root_x;
    pos.y=root_y;
//    free(root_windows);
//    mouseMove(root_x, root_y);


    
    
    return pos;
}

int CCursor::setRelPosition(coordinates pos) {
    //Display *displayMain = XOpenDisplay(NULL);
    if(display == NULL)
    {
        fprintf(stderr, "Error Opening the Display !!!\n");
        return -1;
    }
    XWarpPointer(display, None, None, 0, 0, 0, 0, pos.x, pos.y);
    //XCloseDisplay(displayMain);
    return 0;
}

int CCursor::setAbsPosition(coordinates pos) {
    coordinates mousePos=getPosition();
    pos.x=pos.x-mousePos.x;
    pos.y=pos.y-mousePos.y;
    return setRelPosition(pos);
}

int CCursor::setAbsPercentPosition(coordinates pos) {
    pos.x=pos.x*width;
    pos.y=pos.y*height;
    return setAbsPosition(pos);

}


