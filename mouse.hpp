#ifndef MOUSE_H
#define MOUSE_H

#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <assert.h>
//#include <unistd.h>
#include <malloc.h>

struct coordinates {
   double x;
   double y;
};
class CCursor{
   private:
       Window *root_windows;
       int number_of_screens;
       Display *display;
       int height;
       int width;
   public:
       CCursor();
       ~CCursor();
       coordinates getPosition();
       int setRelPosition(coordinates pos);
       int setAbsPosition(coordinates pos);
       int setAbsPercentPosition(coordinates pos);
};
#endif
