
#include "AOAService.h"

#include <limits.h>

#include "utils/log.h"

struct AOAMessage {
  uint32_t channel;
  uint32_t msg_len;
};

AOAService::AOAService(Proactor &context) : timer_(context) {}

AOAService::~AOAService() { stop(); }

void AOAService::start(std::function<void()> &&f) {
  aoa_.enter_aoa();
  std::error_code ec;
  timer_.set_timeout(1000, 3000);
  timer_.async_wait(
      [this, f](const std::error_code &re_ec, size_t timeout_num) {
        if (aoa_.has_device()) {
          f();
          return;
        }
        auto list = aoa_.list_aoa_device();
        if (list.size() == 0) {
          aoa_.enter_aoa();
          return;
        }

        aoa_.connect(list[0]);
        timer_.set_timeout(50, 50);
      },
      ec);

  if (ec) {
    LOG_ERROR("AOA service start error %d:%s", ec.value(),
              ec.message().c_str());
  }
}

void AOAService::stop() {
  std::error_code ec;
  timer_.close(ec);
  if (ec) {
    LOG_ERROR("AOA service stop error %d:%s", ec.value(), ec.message().c_str());
  }
  aoa_.close();
}

int AOAService::write(const char *data, size_t data_size, int channel) {
  AOAMessage msg;
  msg.channel = channel;
  msg.msg_len = data_size;
  int ret = aoa_.write((char *)&msg, sizeof(msg));
  if (ret != 0) {
    return ret;
  }
  return aoa_.write(data, data_size);
}

int AOAService::read(size_t &size, int &channel) {
  AOAMessage msg = {};
  int ret = aoa_.read((char *)&msg, sizeof(msg));
  size = msg.msg_len;
  channel = msg.channel;

  return ret;
}

int AOAService::read(char *data, size_t data_size) {
  return aoa_.read(data, data_size);
}
