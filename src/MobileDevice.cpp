
#include "MobileDevice.h"

#include "proactor/Proactor.h"
#include "utils/log.h"

// the queue is same the EChannelType
const std::array<int, CHANNEL_NUMBER> MobileDevice::MD_PORTS = {
    CMD_SOCKET_PORT_MD, VIDEO_SOCKET_PORT_MD, MEDIA_SOCKET_PORT_MD,
    TTS_SOCKET_PORT_MD, VR_SOCKET_PORT_MD,    TOUCH_SOCKET_PORT_MD,
};

MobileDevice::MobileDevice(Proactor &p) {
  for (size_t i = 0; i < channels_.size(); ++i) {
    channels_[i] = BindSocket(p);
  }
  register_func();
}
MobileDevice::~MobileDevice() { stop(); }

void MobileDevice::start() {
  for (size_t i = 0; i < channels_.size(); ++i) {
    auto &service = channels_[i];
    auto &client = connects_[i];

    service.start(md_, client, MD_PORTS[i], i + 1); // i + 1 is channel enum
  }
}

void MobileDevice::stop() {
  for (size_t i = 0; i < channels_.size(); ++i) {
    channels_[i].stop();
    connects_[i].stop();
  }
}

void MobileDevice::register_func() {
  md_.callback_version([this](int major, int minor) {
    LOG_INFO("HU version is %d:%d", major, minor);
    md_.send_match_ver(true);
  });

  md_.callback_hu_info(
      [this](const std::string &os, const std::string &manufacturer,
             const std::string &model, const std::string &release,
             const std::string &btaddress, const std::string &btname) {
        LOG_INFO("HU info is os:%s, manufacturer:%s, model:%s, release:%s, "
                 "btaddress:%s, btname:%s",
                 os.c_str(), manufacturer.c_str(), model.c_str(),
                 release.c_str(), btaddress.c_str(), btname.c_str());
      });
}

void MobileDevice::BindSocket::start(CarlifeMobileDevice &mobile_device,
                                     DeviceSocket &client, int bind_port,
                                     int channel) {

  // bind port
  std::error_code ec;
  char port[32] = {};
  snprintf(port, sizeof(port), "%d", bind_port);
  op_.bind(port, ec);
  if (ec) {
    LOG_ERROR("bind port %s error : (%d)%s", port, ec.value(),
              ec.message().c_str());

    return;
  }

  // accept
  auto &md = mobile_device;
  auto accept_callback = [&client, &md, bind_port, channel,
                          this](const std::error_code &re_ec,
                                const std::pair<TcpStreamOp, SocketAddr> &ac) {
    if (re_ec) {
      LOG_ERROR("accept error (%d)%s", re_ec.value(), re_ec.message().c_str());
    }
    client.set(ac.first);
    LOG_INFO("accept client from %s:%d", ac.second.get_ip(),
             ac.second.get_port());

    client.start(md, channel);
    start(md, client, bind_port, channel); // this
  };
  op_.async_accept(accept_callback, ec);

  if (ec) {
    LOG_ERROR("async accept port %s error : (%d)%s", port, ec.value(),
              ec.message().c_str());

    ec.clear();
  }
}

void MobileDevice::BindSocket::stop() {
  if (op_.native_handle() == -1) {
    return;
  }
  std::error_code ec;
  op_.close(ec);
  if (ec) {
    LOG_ERROR("close MobileDevice error %d : (%d)%s", ec.value(),
              ec.message().c_str());
  }
}

void MobileDevice::DeviceSocket::start(CarlifeMobileDevice &mobile_device,
                                       int channel) {
  std::error_code ec;
  op_.async_read(
      buff_, sizeof(buff_),
      [&mobile_device, channel, this](const std::error_code &re_ec,
                                      size_t data_size) {
        if (re_ec) {
          LOG_ERROR("channel %d client recv error: (%d)%s", channel,
                    re_ec.value(), re_ec.message().c_str());
        }
        if (data_size == 0) {
          LOG_WARN("channel %d client remote close", channel);
          return;
        }
        mobile_device.recv_data((EChannelType)channel, buff_, data_size);
        start(mobile_device, channel);
      },
      ec);

  if (ec) {
    LOG_ERROR("client async read channel %d error (%d)%s", channel, ec.value(),
              ec.message().c_str());
  }
}

void MobileDevice::DeviceSocket::stop() {
  if (op_.native_handle() == -1) {
    return;
  }
  std::error_code ec;
  op_.close(ec);
  if (ec) {
    LOG_ERROR("client close error (%d)%s", ec.value(), ec.message().c_str());
  }
}

void MobileDevice::DeviceSocket::set(const TcpStreamOp &stream) {
  if (stream.native_handle() == -1 || stream.native_handle() == 0) {
    LOG_ERROR("Invaild socket");
    return;
  }
  stop();
  op_ = stream;
}
