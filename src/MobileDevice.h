
#ifndef MOBILEDEVICE_H
#define MOBILEDEVICE_H

#include "carlife/CarlifeMobileDevice.h"
#include "proactor/operation/TcpListenerOp.h"
#include "proactor/operation/TcpStreamOp.h"

class Proactor;

class MobileDevice {
public:
  MobileDevice(Proactor &p);
  ~MobileDevice();

  void start();
  void stop();

private:
  void register_func();

private:
  class DeviceSocket {
  public:
    void set(const TcpStreamOp &stream);
    void start(CarlifeMobileDevice &mobile_device, int channel);
    void stop();

  private:
    char buff_[3074];
    TcpStreamOp op_;
  };

  class BindSocket {
  public:
    BindSocket() {}
    explicit BindSocket(Proactor &context) : op_(context) {}
    void start(CarlifeMobileDevice &mobile_device, DeviceSocket &client,
               int bind_port, int channel);
    void stop();

  private:
    TcpListenerOp op_;
  };

  CarlifeMobileDevice md_;
  std::array<BindSocket, CHANNEL_NUMBER> channels_;
  std::array<DeviceSocket, CHANNEL_NUMBER> connects_;

  static const std::array<int, CHANNEL_NUMBER> MD_PORTS;

};     // class MobileDevice

#endif // MOBILEDEVICE_H
