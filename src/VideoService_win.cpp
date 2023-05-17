
#ifdef _WIN32 // {

#include "VideoService.h"

#include "av/CaptureDesktopDxgi.h"
#include "av/Encoder.h"

#include "utils/log.h"

static std::shared_ptr<CaptureDesktopDxgi> CAPTURE_DESKTOP;

VideoService::VideoService(Proactor &context)
    : timer_(context),
      capture_(CAPTURE_DESKTOP ? CAPTURE_DESKTOP
                               : (CAPTURE_DESKTOP =
                                      std::make_shared<CaptureDesktopDxgi>())),
      encoder_(new Encoder) {}

VideoService::~VideoService() {
  stop();
  delete encoder_;
  encoder_ = nullptr;
}

void VideoService::start(uint32_t interval) {
  std::error_code ec;
  timer_.set_timeout(interval, interval);
  timer_.async_wait(
      [this](const std::error_code &re_ec, size_t timeout_num) {
        std::vector<unsigned char> image, output;
        unsigned int width, height;
        if (capture_->capture(image, width, height)) {
          LOG_ERROR("capture screen error!");
          return;
        }

        encoder_->encoder(image, width, height, output);
        if (output.size() > 0 && f_) {
          f_(output);
        }
      },
      ec);
}

void VideoService::register_send(const Callback &f) { f_ = f; }

void VideoService::stop() {
  std::error_code ec;
  timer_.close(ec);
  if (ec) {
    LOG_ERROR("video service stop error %d:%s", ec.value(),
              ec.message().c_str());
  }
}

void VideoService::init_video(uint32_t width, uint32_t height, int framerate,
                              int gop_size) {

  encoder_->init(width, height, framerate, framerate, gop_size, "libx264",
                 {{"preset", "ultrafast"}, {"tune", "zerolatency"}});
}

uint32_t VideoService::screen_width() const { return capture_->width(); }

uint32_t VideoService::screen_height() const { return capture_->height(); }

#endif // _WIN32 }
