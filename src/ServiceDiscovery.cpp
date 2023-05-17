
#include "ServiceDiscovery.h"

#include "proactor/Proactor.h"
#include "utils/log.h"

ServiceDiscovery::ServiceDiscovery(Proactor &context) : timer_(context) {
  std::error_code ec;
  broad_udp_ = UdpSocket::create(kIpV4, ec);
  if (ec) {
    LOG_ERROR("broad udp create error (%d)%s", ec.value(),
              ec.message().c_str());
  }
}
ServiceDiscovery::~ServiceDiscovery() {
  stop();

  std::error_code ec;
  broad_udp_.close(ec);
  if (ec) {
    LOG_ERROR("broad udp close error (%d)%s", ec.value(), ec.message().c_str());
  }
}

void ServiceDiscovery::start() {
  timer_.set_timeout(3000, 8000);
  std::error_code ec;
  timer_.async_wait(
      [this](const std::error_code &re_ec, size_t) {
        if (re_ec) {
          LOG_ERROR("timer async wait error (%d)%s", re_ec.value(),
                    re_ec.message().c_str());
        }
        std::error_code ec;
        auto ips = SocketAddr::get_local_ip_mask(ec);
        if (ec) {
          LOG_ERROR("get local all ip broad error %d:%s", ec.value(),
                    ec.message().c_str());
          ec.clear();
        }
        for (auto &i : ips) {
          char send_buff[] = "CarlifeHost";
          SocketAddr &broad = std::get<2>(i);
          broad.set_port(7999);
          LOG_TRACE("send broad %s:%d", broad.get_ip(), broad.get_port());
          broad_udp_.send_to(send_buff, sizeof(send_buff) - 1, broad, ec);
          if (ec) {
            LOG_ERROR("send to broad %s:%d error %d", broad.get_ip(),
                      broad.get_port(), ec.value());
            ec.clear();
          }
        }
      },
      ec);

  if (ec) {
    LOG_ERROR("Timer start error (%d)%s", ec.value(), ec.message().c_str());
  }
}

void ServiceDiscovery::stop() {
  std::error_code ec;
  timer_.close(ec);
  if (ec) {
    LOG_ERROR("Timer close error (%d)%s", ec.value(), ec.message().c_str());
  }
}
