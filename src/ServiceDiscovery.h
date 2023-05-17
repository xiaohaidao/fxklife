
#ifndef SERVICEDISCOVERY_H
#define SERVICEDISCOVERY_H

#include "proactor/operation/TimerOp.h"
#include "sockets/UdpSocket.h"

class ServiceDiscovery {
public:
  explicit ServiceDiscovery(Proactor &context);
  ~ServiceDiscovery();
  void start();
  void stop();

private:
  TimerOp timer_;
  UdpSocket broad_udp_;

}; // ServiceDiscovery

#endif // SERVICEDISCOVERY_H
