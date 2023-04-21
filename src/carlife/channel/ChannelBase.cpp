
#include "carlife/channel/ChannelBase.h"

#include <google/protobuf/message.h>

void ChannelBase::register_send(std::function<int(const char *, size_t)> f) {
  send_ = f;
}

void ChannelBase::send(const char *head, size_t head_size,
                       const ::google::protobuf::Message &cmd) {
  std::string send_data = cmd.SerializeAsString();
  if (send_) {
    send_(head, head_size);
    send_(send_data.c_str(), send_data.size());
  }
}

void ChannelBase::send(const char *data, size_t data_size) {
  if (send_) {
    send_(data, data_size);
  }
}