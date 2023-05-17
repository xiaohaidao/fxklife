
#ifndef AV_CAPTUREDESKTOP_H
#define AV_CAPTUREDESKTOP_H

#include <vector>

class CaptureDesktop {
public:
  virtual ~CaptureDesktop() {}
  virtual unsigned int width() const = 0;
  virtual unsigned int height() const = 0;

  virtual int capture(std::vector<unsigned char> &image, unsigned int &width,
                      unsigned int &height) = 0;

}; // class CaptureDesktop

#endif // AV_CAPTUREDESKTOP_H
