## saveVideo:  Capturing videos from Grashopper USB3 Cameras under Linux

SaveVideo is a small tool to capture and encoding videos. Supports simultaneous capturing from multiple Point Grey USB cameras. See this [TechNote](https://www.ptgrey.com/support/downloads/10398) how to install the ptGrey cameras under linux.

### Installation

Needs OpenCV, FlyCapSDK, ncurses to be installed.  Edit the Makefile to your liking. Then

~~~~
make
~~~~


### Usage
~~~~
./saveVideo inputvideoName [CODEC WAITMSEC] 
~~~~
Supported codecs (via OpenCV): X264 MPEG DIVX 

If WAITMSEC=0 no display is shown. Otherwise to number of msec to wait before refreshing. Press "q" (while pointing in the video) to exit and stop the video recording. 

### Example
~~~~
./saveVideo test.avi X264 500
~~~~


### Installation for only OpenCV camera support
Alternatively the capturing and writing can be used by OpenCV compatible cameras (such as webcams) without the FlyCapture SDK. For this one needs only OpenCV to be installed. The tool is called **saveVideoBase**

#### Compilation

~~~~
make -f MakefileBase
~~~~

#### Usage

~~~~
./saveVideoBase camidx inputvideoname [CODEC WAITSEC]
~~~~

where camidx is the camera index to be used (usually 0 for the first camera installed on the system). 




