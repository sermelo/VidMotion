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
