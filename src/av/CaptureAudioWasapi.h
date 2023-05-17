
#ifndef AV_CAPTUREAUDIOWASAPI_H
#define AV_CAPTUREAUDIOWASAPI_H

#include <cstdio>
#include <vector>

struct IMMDeviceEnumerator;
struct IMMDevice;
struct IAudioClient;
typedef struct tWAVEFORMATEX WAVEFORMATEX;
// struct WAVEFORMATEX;
struct IAudioCaptureClient;

class CaptureAudioWasapi {
public:
  CaptureAudioWasapi();
  ~CaptureAudioWasapi();

  // the audio type is pcm
  int capture(std::vector<unsigned char> &audio);

  void start();
  void stop();

private:
  IMMDeviceEnumerator *enumerator_;
  IMMDevice *device_;
  IAudioClient *audio_;
  WAVEFORMATEX *pwfx_;
  IAudioCaptureClient *capture_;
  unsigned int num_frames_avaliable_;
  unsigned int num_frames_size_;

  enum Type {
    Audio,
    Render,
  } type_;
  bool is_audio_;
  bool is_capture_;

}; // class CaptureAudioWasapi

#endif // AV_CAPTUREAUDIOWASAPI_H
