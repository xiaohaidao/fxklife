
#ifndef HEADUNIT_H
#define HEADUNIT_H

#include <array>

#include "AOAService.h"
#include "VideoService.h"
#include "carlife/CarlifeHeadUnit.h"
#include "proactor/operation/TcpStreamOp.h"
#include "proactor/operation/UdpSocketOp.h"

class Proactor;

class HeadUnit {
public:
  HeadUnit(Proactor &p);
  ~HeadUnit();

  void start();
  void stop();

private:
  void begin_tcp_service(const SocketAddr &target);
  void begin_aoa_service();

  void stop_tcp_service();
  void stop_aoa_service();

  void register_aoa_func();
  void register_tcp_func();
  void register_func();

  void first_send();
  void service_dicovery();

private:
  class DeviceSocket {
  public:
    void set(const TcpStreamOp &stream);
    void start(const SocketAddr &target, CarlifeHeadUnit &hu,
               EChannelType channel, std::function<void()> first_callback,
               std::function<void()> service_start);
    void start_recv(CarlifeHeadUnit &hu, EChannelType channel,
                    std::function<void()> service_start);
    void stop();

    int send(const char *buff, size_t buff_size);

  private:
    char buff_[4096];
    TcpStreamOp op_;
  };

  class BindUdpSocket {
  public:
    explicit BindUdpSocket(Proactor &context) : op_(context) {}
    void start(int bind_port,
               std::function<void(const SocketAddr &)> bind_callback);

    void stop();

  private:
    char buff_[512];
    UdpSocketOp op_;
  };

  bool is_aoa_service_;
  AOAService aoa_;
  BindUdpSocket sd_;
  // VideoService vs_;
  CarlifeHeadUnit hu_;
  std::array<DeviceSocket, CHANNEL_NUMBER> connects_;

  static const std::array<int, CHANNEL_NUMBER> MD_PORTS;

}; // class HeadUnit

#endif // HEADUNIT_H
