
#ifndef VIDEOSERVICE_H
#define VIDEOSERVICE_H

#include <memory>

#include "proactor/operation/TimerOp.h"

class CaptureDesktop;
class Encoder;

class VideoService {
public:
  explicit VideoService(Proactor &context);
  ~VideoService();
  void start(uint32_t interval);
  void stop();

  void init_video(uint32_t width, uint32_t height, int framerate, int gop_size);

  typedef std::function<void(const std::vector<unsigned char> &)> Callback;
  void register_send(const Callback &f);

  uint32_t screen_width() const;
  uint32_t screen_height() const;

private:
  TimerOp timer_;
  std::shared_ptr<CaptureDesktop> capture_;
  Encoder *encoder_;

  Callback f_;

}; // VideoService

#endif // VIDEOSERVICE_H
