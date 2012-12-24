
GCC=g++
CFLAGS=-Wall -c 
X11_FLAGS= -lX11
OPENCV_FLAGS=`pkg-config --cflags opencv`
OPENCV_LFLAGS=`pkg-config --libs opencv`
SOURCES=mouse.cpp video.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXEC=control

all: $(OBJECTS) $(EXEC)
$(BUILDDIR)mouse.o: mouse.cpp
	$(GCC) $(CFLAGS) mouse.cpp $(X11_FLAGS)

video.o: video.cpp
	$(GCC) $(CFLAGS) $(OPENCV_FLAGS) $(OPENCV_LFLAGS) video.cpp 

control: 
	$(GCC) $(OBJECTS) $(OPENCV_LFLAGS) -o $(EXEC)
clean:
	rm -rf *.o control

