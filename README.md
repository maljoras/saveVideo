## saveVideo:  Capturing of videos from the Grashopper USB3 under Linux

SaveVideo is a small tool to capture and encoding videos. Supports simultaneous capturing from multiple Point Grey USB cameras. 

### Installation

Needs OpenCV, FlyCapSDK, Glibmm-2.4, Glib-2.0, sigc++-2.0 to be installed. 

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





