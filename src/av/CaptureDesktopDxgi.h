
#ifndef AV_CAPTUREDESKTOPGXGI_H
#define AV_CAPTUREDESKTOPGXGI_H

#include "CaptureDesktop.h"

class ID3D11Device;
class ID3D11DeviceContext;
class IDXGIOutputDuplication;
class ID3D11Texture2D;

class CaptureDesktopDxgi : public CaptureDesktop {
public:
  CaptureDesktopDxgi();
  ~CaptureDesktopDxgi();
  virtual unsigned int width() const;
  virtual unsigned int height() const;

  // image format BGRA, size is w * h * 4
  virtual int capture(std::vector<unsigned char> &image, unsigned int &width,
                      unsigned int &height);

private:
  bool InitD3D11Device();
  bool InitDuplication();
  bool CreateSharedTexture();

  unsigned int width_;
  unsigned int height_;
  ID3D11Device *device_;
  ID3D11DeviceContext *deviceContext_;
  IDXGIOutputDuplication *duplication_;
  ID3D11Texture2D *gdi_texture_;
  ID3D11Texture2D *rgba_texture_;

}; // class CaptureDesktopDxgi

#endif // AV_CAPTUREDESKTOPGXGI_H
