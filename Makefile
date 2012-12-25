
GCC=g++
#CFLAGS=-Wall -c -DNDEBUG
CFLAGS=-Wall -c
X11_FLAGS= -lX11
OPENCV_FLAGS=`pkg-config --cflags opencv`
OPENCV_LFLAGS=`pkg-config --libs opencv`
SOURCES=mouse.cpp template.cpp video.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXEC=control

all: $(OBJECTS) $(EXEC)
mouse.o: mouse.cpp
	$(GCC) $(CFLAGS) mouse.cpp $(X11_FLAGS)

template.o: template.cpp
	$(GCC) $(CFLAGS) $(OPENCV_FLAGS) $(OPENCV_LFLAGS) template.cpp

video.o: video.cpp
	$(GCC) $(CFLAGS) $(OPENCV_FLAGS) $(OPENCV_LFLAGS) video.cpp 

control: 
	$(GCC) $(OBJECTS) $(OPENCV_LFLAGS) -o $(EXEC)
clean:
	rm -rf *.o control

