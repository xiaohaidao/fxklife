
#ifndef AV_CAPTUREAUDIO_H
#define AV_CAPTUREAUDIO_H

#include <vector>

class CaptureAudio {
public:
  virtual ~CaptureAudio() {}

  virtual int capture(std::vector<unsigned char> &image, unsigned int &width,
                      unsigned int &height) = 0;

}; // class CaptureAudio

#endif // AV_CAPTUREAUDIO_H
