# saveVideo

## Capturing of videos from the Grashopper USB3 under linux. 

Small tool to capture and encoding videos. Supports simultanous capturing from multiple Point Grey USB cameras. 

### Installation

Needs OpenCV, FlyCapSDK, Glibmm and codecs to be installed. 

~~~~
make
~~~~

### Usage:
~~~~
         ./savevideo inputvideoName [CODEC WAITMSEC] 
~~~~

### Example: 
~~~~
./savevideo test.avi X264 500
~~~~

Supported codecs (via OpenCV): X264 MPEG DIVX 

If WAITMSEC=0 no display is shown. Otherwise to number of msec to wait before refreshing. Press "q" (while pointing in the video) to exit and stop the video recording. 



