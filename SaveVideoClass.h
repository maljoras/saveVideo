#include "FlyCapture2.h"
#include "SaveVideoClassBase.h"

using namespace std;


/**
 * SaveVideo Class.  Saves and displays a video captured from a camaera.
 */ 
class VideoSaverFlyCapture: public VideoSaver  
{
public:
  /** Constructor. */
  VideoSaverFlyCapture();

  /** Destructor. */
  virtual ~VideoSaverFlyCapture();

  /**
   */
  bool isFlycapture() {
    return m_isFlycapture;
  };

	
  /**
   * Captures and writes the video to the file
   */ 
  virtual int startCapture();

  virtual int close();

  virtual int init(int camIdx);
  
  int init(FlyCapture2::PGRGuid camIdx);

  virtual bool isInit();

private:

  FlyCapture2::Camera m_Camera;
  bool m_isFlycapture;
  
protected:

  virtual int stopCapturing();
  virtual void captureThread();	

};
