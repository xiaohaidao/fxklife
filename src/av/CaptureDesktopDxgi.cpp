
#include "CaptureDesktopDxgi.h"

#include <stdio.h>

#include <d3d11.h>
#include <dxgi1_2.h>

// #pragma comment(lib, "d3d11.lib")
// #pragma comment(lib, "dxgi.lib")

CaptureDesktopDxgi::CaptureDesktopDxgi()
    : width_(0), height_(0), device_(nullptr), deviceContext_(nullptr),
      duplication_(nullptr), gdi_texture_(nullptr), rgba_texture_(nullptr) {

  InitD3D11Device();
  InitDuplication();
}

CaptureDesktopDxgi::~CaptureDesktopDxgi() {
  if (duplication_) {
    duplication_->Release();
  }
  if (device_) {
    device_->Release();
  }
  if (deviceContext_) {
    deviceContext_->Release();
  }
}

bool CaptureDesktopDxgi::InitD3D11Device() {
  D3D_DRIVER_TYPE DriverTypes[] = {
      D3D_DRIVER_TYPE_HARDWARE,
      D3D_DRIVER_TYPE_WARP,
      D3D_DRIVER_TYPE_REFERENCE,
  };
  UINT NumDriverTypes = ARRAYSIZE(DriverTypes);

  D3D_FEATURE_LEVEL FeatureLevels[] = {
      D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
      D3D_FEATURE_LEVEL_9_1};
  UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);
  D3D_FEATURE_LEVEL FeatureLevel;

  for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes;
       ++DriverTypeIndex) {
    HRESULT hr =
        D3D11CreateDevice(nullptr, DriverTypes[DriverTypeIndex], nullptr, 0,
                          FeatureLevels, NumFeatureLevels, D3D11_SDK_VERSION,
                          &device_, &FeatureLevel, &deviceContext_);
    if (SUCCEEDED(hr)) {
      break;
    }
  }

  if (device_ == nullptr || deviceContext_ == nullptr) {
    return false;
  }

  return true;
}

bool CaptureDesktopDxgi::InitDuplication() {
  HRESULT hr = S_OK;

  IDXGIDevice *dxgiDevice = nullptr;
  hr = device_->QueryInterface(__uuidof(IDXGIDevice),
                               reinterpret_cast<void **>(&dxgiDevice));
  if (FAILED(hr)) {
    return false;
  }

  IDXGIAdapter *dxgiAdapter = nullptr;
  hr = dxgiDevice->GetAdapter(&dxgiAdapter);
  dxgiDevice->Release();
  if (FAILED(hr)) {
    return false;
  }

  UINT output = 0;
  IDXGIOutput *dxgiOutput = nullptr;
  while (true) {
    hr = dxgiAdapter->EnumOutputs(output++, &dxgiOutput);
    if (hr == DXGI_ERROR_NOT_FOUND) {
      return false;
    } else {
      // DXGI_OUTPUT_DESC desc;
      // dxgiOutput->GetDesc(&desc);
      // int width = desc.DesktopCoordinates.right -
      // desc.DesktopCoordinates.left; int height =
      // desc.DesktopCoordinates.bottom - desc.DesktopCoordinates.top;
      break;
    }
  }
  dxgiAdapter->Release();

  IDXGIOutput1 *dxgiOutput1 = nullptr;
  hr = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput1),
                                  reinterpret_cast<void **>(&dxgiOutput1));
  dxgiOutput->Release();
  if (FAILED(hr)) {
    return false;
  }

  hr = dxgiOutput1->DuplicateOutput(device_, &duplication_);
  dxgiOutput1->Release();
  if (FAILED(hr)) {
    return false;
  }
  DXGI_OUTDUPL_DESC desc;
  duplication_->GetDesc(&desc);
  width_ = desc.ModeDesc.Width;
  height_ = desc.ModeDesc.Height;

  if (!CreateSharedTexture()) {
    return false;
  }

  return true;
}

unsigned int CaptureDesktopDxgi::width() const { return width_; }

unsigned int CaptureDesktopDxgi::height() const { return height_; }

int CaptureDesktopDxgi::capture(std::vector<unsigned char> &image,
                                unsigned int &width, unsigned int &height) {

  auto get_frame = [this]() -> bool {
    HRESULT hr = S_OK;
    DXGI_OUTDUPL_FRAME_INFO frameInfo = {};
    IDXGIResource *resource = nullptr;
    ID3D11Texture2D *acquireFrame = nullptr;

    if (!duplication_) {
      return false;
    }
    duplication_->ReleaseFrame();
    hr = duplication_->AcquireNextFrame(0, &frameInfo, &resource);
    if (FAILED(hr)) {
      if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
        return true;
      } else {
        return false;
      }
    }

    hr = resource->QueryInterface(__uuidof(ID3D11Texture2D),
                                  reinterpret_cast<void **>(&acquireFrame));

    deviceContext_->CopyResource(gdi_texture_, acquireFrame);

    IDXGISurface1 *surface1 = nullptr;
    hr = gdi_texture_->QueryInterface(__uuidof(IDXGISurface1),
                                      reinterpret_cast<void **>(&surface1));
    if (FAILED(hr)) {
      return false;
    }

    CURSORINFO cursorInfo = {0};
    cursorInfo.cbSize = sizeof(CURSORINFO);
    if (GetCursorInfo(&cursorInfo)) {
      if (cursorInfo.flags == CURSOR_SHOWING) {
        auto cursorPosition = cursorInfo.ptScreenPos;
        auto lCursorSize = cursorInfo.cbSize;
        HDC hdc;
        surface1->GetDC(FALSE, &hdc);
        DrawIconEx(hdc, cursorPosition.x /* - monitor_.left */,
                   cursorPosition.y /* - monitor_.top*/, cursorInfo.hCursor, 0,
                   0, 0, 0, DI_NORMAL | DI_DEFAULTSIZE);
        surface1->ReleaseDC(nullptr);
      }
    }

    deviceContext_->CopyResource(rgba_texture_, gdi_texture_);

    acquireFrame->Release();
    resource->Release();
    if (FAILED(hr)) {
      return false;
    }

    return true;
  };
  if (!get_frame()) {
    return -1;
  }

  D3D11_MAPPED_SUBRESOURCE mappedResource = {};
  deviceContext_->Map(rgba_texture_, 0, D3D11_MAP_READ, 0, &mappedResource);

  // size_t imageSize = width_ * height_  * 3 / 2; // YUV420 size
  size_t imageSize = width_ * height_ * 4; // BGRA size
  image.resize(imageSize);
  unsigned char *p_data = (unsigned char *)(mappedResource.pData);
  memcpy(image.data(), p_data, imageSize);
  width = width_;
  height = height_;
  return 0;
}

bool CaptureDesktopDxgi::CreateSharedTexture() {
  HRESULT hr = S_OK;

  D3D11_TEXTURE2D_DESC desc = {0};
  desc.Width = width_;
  desc.Height = height_;
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  // desc.Format = DXGI_FORMAT_NV12;
  desc.BindFlags = 0;
  desc.SampleDesc.Count = 1;

  desc.Usage = D3D11_USAGE_STAGING;
  desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
  desc.MiscFlags = 0;

  hr = device_->CreateTexture2D(&desc, nullptr, &rgba_texture_);
  if (FAILED(hr)) {
    return false;
  }

  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.CPUAccessFlags = 0;
  desc.BindFlags = D3D11_BIND_RENDER_TARGET;
  desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
  desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

  hr = device_->CreateTexture2D(&desc, nullptr, &gdi_texture_);
  if (FAILED(hr)) {
    return false;
  }
  return true;
}