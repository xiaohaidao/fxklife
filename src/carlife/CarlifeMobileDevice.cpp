
#include "carlife/CarlifeMobileDevice.h"

#include "utils/log.h"

CarlifeMobileDevice::CarlifeMobileDevice() {
  cmd_.recv_data(0, 0);
  ctrl_.recv_data(0, 0);
  media_.recv_data(0, 0);
  tts_.recv_data(0, 0);
  video_.recv_data(0, 0);
  vr_.recv_data(0, 0);
}

CarlifeMobileDevice::~CarlifeMobileDevice() {}

void CarlifeMobileDevice::register_send(
    EChannelType channel, const std::function<int(const char *, size_t)> &f) {

  switch (channel) {
  case kCMD_CHANNEL:
    cmd_.register_send(f);
    break;
  case kVIDEO_CHANNEL:
    video_.register_send(f);
    break;
  case kMEDIA_CHANNEL:
    media_.register_send(f);
    break;
  case kTTS_CHANNEL:
    tts_.register_send(f);
    break;
  case kVR_CHANNEL:
    vr_.register_send(f);
    break;
  case kCTRL_CHANNEL:
    ctrl_.register_send(f);
    break;
  default:
    LOG_ERROR("Unkown channel %d", channel);
    break;
  }
}

void CarlifeMobileDevice::recv_data(EChannelType channel, const char *data,
                                    size_t data_size) {

  switch (channel) {
  case kCMD_CHANNEL:
    cmd_.recv_data(data, data_size);
    break;
  case kVIDEO_CHANNEL:
    video_.recv_data(data, data_size);
    break;
  case kMEDIA_CHANNEL:
    media_.recv_data(data, data_size);
    break;
  case kTTS_CHANNEL:
    tts_.recv_data(data, data_size);
    break;
  case kVR_CHANNEL:
    vr_.recv_data(data, data_size);
    break;
  case kCTRL_CHANNEL:
    ctrl_.recv_data(data, data_size);
    break;
  default:
    LOG_ERROR("Unkown channel %d", channel);
    break;
  }
}
