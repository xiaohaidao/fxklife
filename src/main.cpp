
#include <stdio.h>

#include "MobileDevice.h"
#include "proactor/Proactor.h"
#include "utils/log.h"

int main(int args, char **argv) {
  std::error_code ec;
  Proactor p(ec);
  if (ec) {
    LOG_ERROR("Proactor start error");
    ec.clear();
  }
  MobileDevice md(p);
  md.start();

  p.run();

  return 0;
}