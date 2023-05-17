
#include "CaptureAudioWasapi.h"

#include <audioclient.h>
#include <mmdeviceapi.h>

CaptureAudioWasapi::CaptureAudioWasapi()
    : enumerator_(nullptr), device_(nullptr), audio_(nullptr), pwfx_(nullptr),
      capture_(nullptr), num_frames_avaliable_(0), num_frames_size_(0),
      type_(Audio), is_audio_(true), is_capture_(true) {

  static HRESULT first = CoInitializeEx(NULL, COINIT_MULTITHREADED);
  HRESULT hr =
      CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                       __uuidof(IMMDeviceEnumerator), (void **)&enumerator_);

  if (FAILED(hr)) {
    fprintf(stderr, "CoCreateInstance error %#x\n", hr);
  }
  // // if mic
  // hr = enumerator_->GetDefaultAudioEndpoint(eCapture, eConsole, &device_);
  // if audio
  hr = enumerator_->GetDefaultAudioEndpoint(is_audio_ ? eRender : eCapture,
                                            eConsole, &device_);

  if (FAILED(hr)) {
    fprintf(stderr, "GetDefaultAudioEndpoint error %#x\n", hr);
  }
  hr = device_->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                         (void **)&audio_);

  if (FAILED(hr)) {
    fprintf(stderr, "Activate error %#x\n", hr);
  }
  hr = audio_->GetMixFormat(&pwfx_);
  if (FAILED(hr)) {
    fprintf(stderr, "GetMixFormat error %#x\n", hr);
  }
  // hr = audio_->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, pwfx_,
  //                         nullptr);
  REFERENCE_TIME hnsRequestedDuration = 10000000;
  hr =
      audio_->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK,
                         hnsRequestedDuration, 0, pwfx_, nullptr);
  if (FAILED(hr)) {
    fprintf(stderr, "Initialize error %#x\n", hr);
  }

  // UINT32 bufferFrameCount = 0;
  hr = audio_->GetBufferSize(&num_frames_size_);
  if (FAILED(hr)) {
    fprintf(stderr, "GetBufferSize error %#x\n", hr);
  }
  hr = audio_->GetService(!is_capture_ ? __uuidof(IAudioRenderClient)
                                       : __uuidof(IAudioCaptureClient),
                          (void **)&capture_);
  if (FAILED(hr)) {
    fprintf(stderr, "GetService error %#x\n", hr);
  }
  // hr = audio_->Start();

  // loop

  // UINT32 packet_length = 0;
  // UINT32 num_frames_avaliable = 0;
  // // BYTE *data_;
  // DWORD flags;
  // hr = capture_->GetNextPacketSize(&packet_length);
  // while (true) {
  //   hr = capture_->GetBuffer(&data_, &num_frames_avaliable, &flags, nullptr,
  //                            nullptr);
  //   hr = capture_->ReleaseBuffer(num_frames_avaliable);
  //   hr = capture_->GetNextPacketSize(&packet_length);
  // }

  // stop
  // hr = audio_->Stop();
}

CaptureAudioWasapi::~CaptureAudioWasapi() {
  stop();

  if (capture_) {
    is_capture_ ? capture_->Release()
                : reinterpret_cast<IAudioRenderClient *>(capture_)->Release();
  }
  if (pwfx_)
    CoTaskMemFree(pwfx_);
  if (audio_)
    audio_->Release();
  if (device_)
    device_->Release();
  if (enumerator_)
    enumerator_->Release();
  capture_ = nullptr;
  pwfx_ = nullptr;
  audio_ = nullptr;
  device_ = nullptr;
  enumerator_ = nullptr;
}

int CaptureAudioWasapi::capture(std::vector<unsigned char> &audio) {
  if (!capture_) {
    return -1;
  }
  HRESULT hr = 0;
  UINT32 packet_length = 0;
  // UINT32 num_frames_avaliable = 0;
  // BYTE *data;
  DWORD flags;

  if (is_capture_) {
    IAudioCaptureClient *capture = capture_;
    unsigned char *data = nullptr;

    hr = capture->GetNextPacketSize(&packet_length);
    if (FAILED(hr)) {
      fprintf(stderr, "GetNextPacketSize error %#x\n", hr);
    }
    audio.clear();
    // int frame_size = (pwfx_->wBitsPerSample / 8) * pwfx_->nChannels;

    //   // Calculate the actual duration of the allocated buffer.
    // // REFERENCE_TIME time units per second and per millisecond
    // #define REFTIMES_PER_SEC 10000000
    // #define REFTIMES_PER_MILLISEC 10000
    //     double hnsActualDuration =
    //         (double)REFTIMES_PER_SEC * num_frames_size_ /
    //         pwfx_->nSamplesPerSec;
    //     // Sleep for half the buffer duration.
    //     Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);
    while (packet_length) {
      unsigned int num_frames_avaliable = 0;
      hr = capture->GetBuffer(&data, &num_frames_avaliable, &flags, nullptr,
                              nullptr);
      if (FAILED(hr)) {
        fprintf(stderr, "GetBuffer error %#x\n", hr);
      }
      int frame_size = pwfx_->nBlockAlign;
      if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
        // audio.resize(audio.size() + num_frames_avaliable * frame_size, 0);
        std::vector<unsigned char> empty(num_frames_avaliable * frame_size, 0);
        audio.insert(audio.end(), empty.begin(), empty.end());
      } else {
        // if (data) {
        audio.insert(audio.end(), data,
                     data + num_frames_avaliable * frame_size);
        // }
      }

      hr = capture->ReleaseBuffer(num_frames_avaliable);
      if (FAILED(hr)) {
        fprintf(stderr, "ReleaseBuffer error %#x\n", hr);
      }

      hr = capture->GetNextPacketSize(&packet_length);
      if (FAILED(hr)) {
        fprintf(stderr, "GetNextPacketSize error %#x\n", hr);
      }
      data = nullptr;
    }
  } else {
    IAudioRenderClient *capture = (IAudioRenderClient *)capture_;
    if (num_frames_avaliable_ > 0 && capture) {
      hr = capture->ReleaseBuffer(num_frames_size_ - num_frames_avaliable_,
                                  AUDCLNT_BUFFERFLAGS_SILENT);
      if (FAILED(hr)) {
        fprintf(stderr, "ReleaseBuffer error %#x\n", hr);
      }
      num_frames_avaliable_ = 0;

      // hr = audio_->GetCurrentPadding(&num_frames_avaliable_);
      // if (FAILED(hr) || num_frames_avaliable_ == num_frames_size_) {
      //   fprintf(stderr, "GetCurrentPadding error %#x\n", hr);
      // }
    }

    // hr = capture->GetBuffer(num_frames_size_ - num_frames_avaliable_, data);
    if (FAILED(hr)) {
      fprintf(stderr, "GetBuffer error %#x\n", hr);
    }
  }
  return 0;
}

void CaptureAudioWasapi::start() {
  if (audio_) {
    auto hr = audio_->Start();
    if (FAILED(hr)) {
      fprintf(stderr, "Start error %#x\n", hr);
    }
  }
}

void CaptureAudioWasapi::stop() {
  if (num_frames_avaliable_ > 0 && capture_) {
    auto hr =
        is_capture_
            ? capture_->ReleaseBuffer(num_frames_avaliable_)
            : reinterpret_cast<IAudioRenderClient *>(capture_)->ReleaseBuffer(
                  num_frames_avaliable_, AUDCLNT_BUFFERFLAGS_SILENT);
    if (FAILED(hr)) {
      fprintf(stderr, "ReleaseBuffer error %#x\n", hr);
    }
    num_frames_avaliable_ = 0;
  }
  if (audio_) {
    auto hr = audio_->Stop();
    if (FAILED(hr)) {
      fprintf(stderr, "Stop error %#x\n", hr);
    }
  }
}