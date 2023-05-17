
#ifndef CARLIFE_AOA_H
#define CARLIFE_AOA_H

#include <string>
#include <vector>

struct libusb_device_handle;

class AOA {
public:
  AOA();
  ~AOA();

  void enter_aoa();
  std::vector<std::pair<int, int>> list_aoa_device();

  void connect(const std::pair<int, int> &aoa_index);
  void close();

  int read(char *buff, size_t buff_size);
  int write(const char *buff, size_t buff_size);

  bool has_device() const { return handle_ != nullptr; }

private:
  libusb_device_handle *handle_;
};

#endif // CARLIFE_AOA_H
