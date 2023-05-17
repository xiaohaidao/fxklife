
#ifndef AOASERVICE_H
#define AOASERVICE_H

#include <memory>

#include "carlife/AOA.h"
#include "proactor/operation/TimerOp.h"

class AOAService {
public:
  explicit AOAService(Proactor &context);
  ~AOAService();
  void start(std::function<void()> &&f);
  void stop();

  int write(const char *data, size_t data_size, int channel);
  int read(size_t &size, int &channel);
  int read(char *data, size_t data_size);

private:
  AOA aoa_;
  TimerOp timer_;

}; // AOAService

#endif // AOASERVICE_H
