FLYCAPTUREINCLUDE ?= /usr/include/flycapture

CC=gcc
CFLAGS=-c -Wall -I$(FLYCAPTUREINCLUDE) -I/usr/include/ -I/usr/local/include/opencv2   
LDFLAGS=   -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio  -lstdc++ -lncurses -lflycapture -lpthread
SOURCES=SaveVideoClassBase.cpp SaveVideoClass.cpp SaveVideo.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=saveVideo


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(DEBUG) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm SaveVideoClass.o SaveVideoClassBass.o SaveVideo.o savevideo
