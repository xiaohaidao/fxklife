
#ifndef MOBILEDEVICE_H
#define MOBILEDEVICE_H

#include <array>

#include "AOAService.h"
#include "ServiceDiscovery.h"
#include "VideoService.h"
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
  void begin_tcp_service();
  void begin_aoa_service();

  void stop_tcp_service();
  void stop_aoa_service();

  void register_aoa_func();
  void register_tcp_func();
  void register_func();

private:
  class DeviceSocket {
  public:
    void set(const TcpStreamOp &stream);
    void start(CarlifeMobileDevice &mobile_device, EChannelType channel,
               std::function<void()> service_start);
    void stop();

    int send(const char *buff, size_t buff_size);

  private:
    char buff_[3074];
    TcpStreamOp op_;
  };

  class BindSocket {
  public:
    BindSocket() {}
    explicit BindSocket(Proactor &context) : op_(context) {}
    void start(int bind_port, EChannelType channel,
               std::function<void(const TcpStreamOp &)> bind_callback);
    void stop();

  private:
    TcpListenerOp op_;
  };

  bool is_aoa_service_;
  AOAService aoa_;
  ServiceDiscovery sd_;
  VideoService vs_;
  CarlifeMobileDevice md_;
  std::array<BindSocket, CHANNEL_NUMBER> channels_;
  std::array<DeviceSocket, CHANNEL_NUMBER> connects_;

  static const std::array<int, CHANNEL_NUMBER> MD_PORTS;

}; // class MobileDevice

#endif // MOBILEDEVICE_H
