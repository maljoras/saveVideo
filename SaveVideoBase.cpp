#include "SaveVideoClassBase.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <iostream>
#include <sstream>
#include <string>   // for strings
//#include <ncurses.h>

#define MINDELAY 250  // waiting time in msec between video frames for the display

using namespace std;

//using namespace cv;



static void help()
{
    cout
        << "------------------------------------------------------------------------------" << endl
        << "Capturing of videos from the Grashopper. Codecs: X264 MPEG DIVX               "       << endl
        << "if WAITMSEC=0 no display is shown. Otherwise to number of msec to wait. "       << endl
        << "Usage:"                                                                         << endl
        << "         ./savevideo camIDX inputvideoName [CODEC WAITMSEC] "                          << endl
        << "Example: ./savevideo 0 test.avi X264 500"                                         << endl
        << "------------------------------------------------------------------------------" << endl
        << endl;
};




int main(int argc, const char *argv[]) {

  //parse inputs
  int waitMsecs = MINDELAY;
  int camIdx;

  if ((argc <3) || (argc >5)) {
 	  cout << "Not enough parameters or too many" << endl;
    help(); 
	  return -1;
  } else if ((argc > 2) && (!(istringstream(argv[1]) >> camIdx))) {
      cout << "First parameter needs to be an int" << endl;
      help();         
      return -1; 
  } else if (argc > 4) {
      if (!(istringstream(argv[4]) >> waitMsecs)) {
 	      cout << "Forth parameter needs to be an int" << endl;
        help();         
	      return -1;  
      } 
  }

  std::string fourcc;
  if (argc<4) {
    fourcc = string("X264");
  } else {
    fourcc = argv[3];
    if (fourcc.length()!=4) {
      help(); 
      cout << "Third parameter needs to be an FOURCC code" << endl;
      return -1;  
    }
  }

  

  // init 
  VideoSaver saver;

  if (!saver.init(camIdx)) {
    cout<< "Error: cannot find camera with index" << camidx << endl;
    help();
    return -1;
  }
  
  const string fname = argv[2];
	string local_fname= std::string(fname);
	
	if (saver.startCaptureAndWrite(local_fname,fourcc)!=0) {
	  return -1;
  }

  // wait and display loop
  char key = 0;
  int waitAmount  = waitMsecs > MINDELAY? waitMsecs: MINDELAY;
  cv::Mat frame;
  string imname;
  cv::Mat smallFrame;
  cv::Size size(800,800);

  while(key != 'q') {

	  key = cv::waitKey(waitAmount); 
	  if (waitMsecs>0) {
	      //usleep(delayFound*1000);
	
	      //plot
	    imname= "Captured video";
	    cv::Mat frame;
	    double timeStamp;
	    int frameNumber;
	    saver.getFrame(&frame,&timeStamp,&frameNumber);
	    cv::resize(frame,smallFrame,size);
	    cv::cvtColor(smallFrame,smallFrame,CV_RGB2BGR);	
	    cv::imshow(imname.c_str(), smallFrame);
	  }

    if (saver.isFinished()) {
      break;
    }
	}    
	
  saver.close();

  return 0;

}
  

