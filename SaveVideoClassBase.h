#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/video.hpp"

#include <iostream>
#include <string>   // for strings
#include <sstream>
#include <fstream>   
#include <iomanip>


using namespace std;


/**
 * SaveVideo Class.  Saves and displays a video captured from a camaera.
 */ 
class VideoSaver  
{
public:
    /** Constructor. */
    VideoSaver();

    /** Destructor. */
    virtual ~VideoSaver();

    /**
     * Captures and writes the video to the file
     */ 
    int startCaptureAndWrite(const string fname, string codec);

    /**
     * Captures without writing to video file
     */ 
    int startCapture();

    /**
     * makes the current frame available in "Frame"
     */ 
    int getFrame(cv::Mat * pFrame ,double * pTimeStamp, int *pFrameNumber);
    /**
     * returns the current framecounter
     */ 
    int getCurrentFrameNumber();
    
    int close();

    int init(int camIdx);

    /**
     * returns frame size 
     */
    cv::Size getFrameSize();

    /**
     * returns the theoretical FPS set (usually set by the camera)
     */
    double getFPS();

    /**
     * Asks whether writing is finished
     */ 
    bool isFinished();

    bool isBGR() {return m_bgr;};

private:

   void _stopWriting();
   void _captureThread();
   void _captureAndWriteThread();
   void waitForNewFrame();
   
protected:

    std::clock_t m_timer;  

    bool m_newFrameAvailable;
    bool m_KeepThreadAlive;
    bool m_KeepWritingAlive;
    bool m_WritingFinished;
    bool m_GrabbingFinished;
    bool m_writing;
    bool m_capturing;
    bool m_bgr;
    
    std::mutex m_FrameMutex;
    std::condition_variable m_newFrameAvailableCond;
    std::thread * m_captureThread;
    std::thread * m_writingThread;
	
    cv::VideoCapture m_Capture;
    float m_FrameRateToUse;
    cv::Size m_FrameSize;
    
    
    cv::Mat m_Frame;
    double m_TimeStamp;

    int m_frameNumber;

    std::fstream m_OutputFile;
    cv::VideoWriter m_Video;
};
