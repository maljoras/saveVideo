

#include "SaveVideoClassBase.h"

#define TIMER_ELAPSED ((double) ( std::clock() - timer ) / (double) CLOCKS_PER_SEC)
#define TIMER_START timer = std::clock();
#define TIMER_INIT std::clock_t timer; timer = std::clock();

#define sleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x)) 
#define M_TIMER_ELAPSED ((double)(( std::clock() - m_timer ) / (double) CLOCKS_PER_SEC))



VideoSaver::VideoSaver()
{
  m_KeepWritingAlive = false; // not yet in capture mode
  m_KeepThreadAlive = false; // not yet in capture mode
  m_WritingFinished = true;
  m_GrabbingFinished = true;
  m_writing = false;
  m_capturing = false;
  m_newFrameAvailable = false;
  m_isBGR = false;
  m_writingFrameNumber = 0;
  m_frameNumber = 0;

};

/****************************************************************************************/
VideoSaver::~VideoSaver()
{
  close();
}

int VideoSaver::close()
{
  stopWriting();
  stopCapturing();
  sleep(500);
  return 0;
}

/****************************************************************************************/
int VideoSaver::stopCapturing() {

  if (m_Capture.isOpened()) {    
    m_Capture.release();
  }
  return 0;
}

/****************************************************************************************/
int VideoSaver::init(int camIdx)
{

  // Connect the camera
  m_Capture.open(camIdx);
  if (!m_Capture.isOpened()){
    std::cout << "Failed to connect to camera" << std::endl; 
    return -1;
  }
	
  // get frame rate 
  double fps = (double) m_Capture.get(cv::CAP_PROP_FPS);
  
  if (!fps) {
    // property not supported. manually check frame rate
    TIMER_INIT;
    int nframes = 50;
    cv::Mat frame;
    for (int i=0; i<nframes;i++) {
      if (!m_Capture.read(frame)) {
        std::cout << "Cannot establish frame rate of camera" << std::endl;
        m_Capture.release(); 
        return -1;
        break; 
      }
    }
    fps = ((double) nframes)/TIMER_ELAPSED;
  }
 
  // Set the frame rate.
  // Note that the actual recording frame rate may be slower,
  // depending on the bus speed and disk writing speed.
  m_FrameRateToUse = fps;
	
  std::cout << "Using frame rate " <<  m_FrameRateToUse << std::endl;

  //get the width and height
  double height = (double) m_Capture.get(cv::CAP_PROP_FRAME_HEIGHT);
  double width  = (double) m_Capture.get(cv::CAP_PROP_FRAME_WIDTH);
  m_FrameSize =  cv::Size(width,height);

  m_isBGR = false;
  return 0;
}

/****************************************************************************************/
int VideoSaver::stopWriting() 
{

  m_KeepWritingAlive = false;
  m_KeepThreadAlive = false;

  while ((!m_GrabbingFinished) || (!m_WritingFinished))
    sleep(100);

  if (m_capturing) {
    m_captureThread->join();
    delete(m_captureThread);
    m_captureThread = NULL;
    m_capturing = false;
  }

  if (m_writing) {
    m_writingThread->join();
    delete(m_writingThread);
    m_writingThread = NULL;
    m_writing = false;
  }

  return 0;
}

/****************************************************************************************/
cv::Size VideoSaver::getFrameSize() {
  return m_FrameSize;
}
/****************************************************************************************/
int VideoSaver::getCurrentFrameNumber() {
  if (!m_GrabbingFinished) {
    return m_frameNumber;
  } else {
    std::cout << "Warning: grabbing finished!!" << std::endl;
    return -1;
  }
}

/****************************************************************************************/
int VideoSaver::getLostFrameNumber() {
  return m_frameNumber-m_writingFrameNumber;
}

/****************************************************************************************/
int VideoSaver::getFrame(cv::Mat * pFrame ,double * pTimeStamp, int *pFrameNumber) 
{
  if (!m_GrabbingFinished) {
    waitForNewFrame();
    
    {
      std::unique_lock<std::mutex> lock(m_FrameMutex);
      
      if (m_Frame.size().width==0) {
	*pFrame = cv::Mat::zeros(m_FrameSize,CV_8UC3);
      }
      else {
	m_Frame.copyTo(*pFrame);
      }
      *pTimeStamp = m_TimeStamp;
      *pFrameNumber = m_frameNumber;
      m_newFrameAvailable = false;
    }
    return 0;
  }
  else {
    std::cout << "WARNING getFrame  Failed!" << std::endl;
    return -1;
  }
}



/****************************************************************************************/
double VideoSaver::getFPS() 
{
  return (double) m_FrameRateToUse;
}

/****************************************************************************************/
bool VideoSaver::isFinished() 
{
  return m_WritingFinished && m_GrabbingFinished;
}


/****************************************************************************************/
bool VideoSaver::isInit() 
{
  return (m_Capture.isOpened() && (!m_capturing));
}


/****************************************************************************************/
int VideoSaver::startCapture() {

  if (isFinished() && (isInit())) {
    // start thread to begin capture and populate Mat frame
    
    //start the grabbing thread
    m_KeepWritingAlive = false;  // not to be started
    m_WritingFinished = true;
    m_newFrameAvailable = false;
    std::cout <<  "Start video grabbing .." << std::endl;
    
    m_captureThread = new std::thread(&VideoSaver::captureThread,this);

    m_capturing = true;
    // wait for startup
    sleep(500);
    waitForNewFrame();

    return 0;

  } else {
    if (isInit()) {
        std::cout << "Warning: capture not yet finished !" << std::endl;    
      } else {
        std::cout << "Warning: camera not available!" << std::endl;    
      }
    return -1;    
  };      
}

void VideoSaver::waitForNewFrame() {

  std::unique_lock<std::mutex> lock(m_FrameMutex);

  while (!m_newFrameAvailable) {
    m_newFrameAvailableCond.wait(lock);
  } 
}


/****************************************************************************************/
int VideoSaver::startCaptureAndWrite(const string inFname, string codec)
{
  // start the capture 
  if (startCapture()!=0) 
    return -1;

  // open file stream for the tpoints
  string fname = string(inFname);
  string txtfname;
  txtfname = fname + ".txt";

  m_OutputFile.open(txtfname.c_str(), std::ios::out );

  if (!m_OutputFile.is_open()) 
    {
      std::cout  << "Could not open the output text for write: " << txtfname << std::endl;
      return -1;
    }

  //start the video stream
  m_Video = cv::VideoWriter(fname,CV_FOURCC(codec[0],codec[1],codec[2],codec[3]),m_FrameRateToUse, m_FrameSize ,true);

  if (!m_Video.isOpened())
    {
      std::cout  << "Could not open the output video for write: " << fname << std::endl;
      return -1;
    }

  
  // start the writing thread
  std::cout <<  "Start video saving.." << std::endl;
  m_writing = true;
  m_writingThread = new std::thread(&VideoSaver::captureAndWriteThread,this);
  
  return 0;

}



/****************************************************************************************/
void VideoSaver::captureThread()
{

  m_GrabbingFinished = false;
  m_KeepThreadAlive = true;
  m_frameNumber = 0;
  m_newFrameAvailable = false;

  m_timer= std::clock();

	  
  while (m_KeepThreadAlive) {

    double localtimestamp = -1.;
    
    cv::Mat frame;
    if (m_Capture.grab()) {
      localtimestamp = M_TIMER_ELAPSED;  
      m_Capture.retrieve(frame);
    } else {
      std::cout<< "Error: a grabbing error occured" << std::endl;
      break;
    }
   
    frame.convertTo(frame,CV_8UC3); // necessary ?


    // copy to frame variable and update times
    {  std::unique_lock<std::mutex> lock(m_FrameMutex); 

      m_Frame.release();
      m_Frame = frame.clone();
      
      
      m_TimeStamp =localtimestamp; 
      m_frameNumber++;
      m_newFrameAvailable = true;
      m_newFrameAvailableCond.notify_one();
    }

  } 
  m_newFrameAvailableCond.notify_one();
    
  // stop the capturing
  stopCapturing();

  m_GrabbingFinished  = true;
}


/****************************************************************************************/
void VideoSaver::captureAndWriteThread()
{
  m_WritingFinished = false;

  // capture loop
  m_writingFrameNumber=0;
  m_KeepWritingAlive = true;


  int delayFound = 0;
  int grabbedFrameNumber;
  cv::Mat frame;
  double localTimeStamp;
  int frameNumber;
    
  while(m_KeepWritingAlive) {
    
    const double currentTime =  M_TIMER_ELAPSED;

    {
      std::unique_lock<std::mutex> lock(m_FrameMutex);
      if (isBGR()) {
	cv::cvtColor(m_Frame,frame,CV_RGB2BGR);
      } else {
	m_Frame.copyTo(frame);
      }
      localTimeStamp = m_TimeStamp;
      grabbedFrameNumber = m_frameNumber;
      frameNumber  = m_writingFrameNumber++;
    }
      
    m_Video.write(frame); // slow, thus out of the lock

    m_OutputFile << frameNumber 
      << "\t" << grabbedFrameNumber <<"\t" 
      <<  std::fixed << std::setprecision(5) 
      << localTimeStamp << std::endl;


    const double thisTime = M_TIMER_ELAPSED;
    const double seconds = thisTime - currentTime;	
    delayFound = static_cast<int> (1000./m_FrameRateToUse - 1000*seconds);
    if (delayFound>0) {
	     sleep(delayFound);
    }
  }

  while (!m_GrabbingFinished)
    sleep(1000);
    
  //close the files
  m_Video.release();
  m_OutputFile.close();


  std::cout << "Finished writing" << std::endl;    
  m_WritingFinished = true;
};

