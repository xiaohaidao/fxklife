
#ifndef TRANSFERPORT_H
#define TRANSFERPORT_H

#include <array>

#include "carlife/CarlifeMobileDevice.h"
#include "proactor/operation/TcpListenerOp.h"
#include "proactor/operation/TcpStreamOp.h"

class Proactor;

class TransferPort {
public:
  TransferPort(Proactor &p);
  ~TransferPort();

  void start(const char *from, const char *to, bool is_from_md);
  void stop();

private:
  class DeviceSocket {
  public:
    DeviceSocket() {}
    explicit DeviceSocket(Proactor &context) : op_(&context) {}

    void start(const SocketAddr &target, DeviceSocket &to);
    void stop();

  private:
    char buff_[3074];
    TcpStreamOp op_;
  };

  Proactor *proactor_;
  std::array<DeviceSocket, CHANNEL_NUMBER> from_;
  std::array<DeviceSocket, CHANNEL_NUMBER> to_;

  static const std::array<int, CHANNEL_NUMBER> MD_PORTS;
  static const std::array<int, CHANNEL_NUMBER> HU_PORTS;

}; // class TransferPort

#endif // TRANSFERPORT_H
