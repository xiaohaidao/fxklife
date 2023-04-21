
#ifndef CARLIFE_CHANNEL_CHANNELBASE_H
#define CARLIFE_CHANNEL_CHANNELBASE_H

#include <functional>

#include "carlife/channel/CarlifeType.h"
#include "carlife/channel/ChannelBaseMacro.h"
#include "sockets/SocketAddr.h"

namespace google {
namespace protobuf {
class Message;
} // namespace protobuf
} // namespace google

class ChannelBase {
public:
  void register_send(std::function<int(const char *, size_t)> f);
  virtual void recv_data(const char *data, size_t data_size) = 0;

protected:
  void send(const char *head, size_t head_size,
            const ::google::protobuf::Message &cmd);
  void send(const char *data, size_t data_size);

private:
  std::function<int(const char *, size_t)> send_;

};     // class ChannelBase

#endif // CARLIFE_CHANNEL_CHANNELBASE_H
