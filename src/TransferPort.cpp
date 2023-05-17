
#include "TransferPort.h"

#include "proactor/Proactor.h"
#include "utils/log.h"

// the queue is same the EChannelType
const std::array<int, CHANNEL_NUMBER> TransferPort::MD_PORTS = {
    CMD_SOCKET_PORT_MD, VIDEO_SOCKET_PORT_MD, MEDIA_SOCKET_PORT_MD,
    TTS_SOCKET_PORT_MD, VR_SOCKET_PORT_MD,    TOUCH_SOCKET_PORT_MD,
};
const std::array<int, CHANNEL_NUMBER> TransferPort::HU_PORTS = {
    CMD_SOCKET_PORT_HU, VIDEO_SOCKET_PORT_HU, MEDIA_SOCKET_PORT_HU,
    TTS_SOCKET_PORT_HU, VR_SOCKET_PORT_HU,    TOUCH_SOCKET_PORT_HU,
};

TransferPort::TransferPort(Proactor &p) {
  for (size_t i = 0; i < from_.size(); ++i) {
    from_[i] = DeviceSocket(p);
    to_[i] = DeviceSocket(p);
  }
}
TransferPort::~TransferPort() { stop(); }

void TransferPort::start(const char *from, const char *to, bool is_from_md) {

  const std::array<int, CHANNEL_NUMBER> &from_port =
      is_from_md ? MD_PORTS : HU_PORTS;
  const std::array<int, CHANNEL_NUMBER> &to_port =
      is_from_md ? HU_PORTS : MD_PORTS;

  for (size_t i = 0; i < from_.size(); ++i) {
    SocketAddr from_addr(from, std::to_string(from_port[i]).c_str());
    SocketAddr to_addr(from, std::to_string(to_port[i]).c_str());
    auto &from_service = from_[i];
    auto &to_service = to_[i];

    from_service.start(from_addr, to_service);
    to_service.start(to_addr, from_service);
  }
}

void TransferPort::stop() {
  for (size_t i = 0; i < from_.size(); ++i) {
    from_[i].stop();
    to_[i].stop();
  }
}

void TransferPort::DeviceSocket::start(const SocketAddr &from,
                                       DeviceSocket &to) {
  std::error_code ec;
  op_.async_connect(
      from,
      [this, from, &to](const std::error_code &re_ec, size_t) {
        if (re_ec) {
          LOG_ERROR("client %s:%d connect error: (%d)%s", from.get_ip(),
                    from.get_port(), re_ec.value(), re_ec.message().c_str());
          to.stop();
          return;
        }

        std::error_code ec;
        op_.async_read(
            buff_, sizeof(buff_),
            [this, from, &to](const std::error_code &re_ec, size_t data_size) {
              if (re_ec) {
                LOG_ERROR("client %s:%d recv error: (%d)%s", from.get_ip(),
                          from.get_port(), re_ec.value(),
                          re_ec.message().c_str());
                to.stop();
              }
              if (data_size == 0) {
                LOG_WARN("client %s:%d close", from.get_ip(), from.get_port());
                to.stop();
                return;
              }
              std::error_code ec;
              to.op_.write(buff_, data_size, ec);
              if (ec) {
                LOG_ERROR("client %s:%d transfer message send error: (%d)%s",
                          from.get_ip(), from.get_port(), re_ec.value(),
                          re_ec.message().c_str());
              }
            },
            ec);

        if (ec) {
          LOG_ERROR("client asyn read %s:%d error (%d)%s", from.get_ip(),
                    from.get_port(), ec.value(), ec.message().c_str());
        }
      },
      ec);
  if (ec) {
    LOG_ERROR("client connect %s:%d error (%d)%s", from.get_ip(),
              from.get_port(), ec.value(), ec.message().c_str());
    to.stop();
  }
}

void TransferPort::DeviceSocket::stop() {
  if (op_.native_handle() == -1) {
    return;
  }
  std::error_code ec;
  op_.close(ec);
  // if (ec) {
  //   LOG_ERROR("client close error (%d)%s", ec.value(), ec.message().c_str());
  // }
}
