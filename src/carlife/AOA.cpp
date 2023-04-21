
#include "carlife/AOA.h"

#include "libusb-1.0/libusb.h"

int f() {
  if (libusb_init(/*ctx=*/NULL)) {
    printf("libusb_init_context error\n");
    return 1;
  }

  libusb_device **devs;
  if (libusb_get_device_list(NULL, &devs) < 0) {
    libusb_exit(NULL);
    printf("libusb_get_device_list error\n");
    return 1;
  }

  libusb_device *dev;
  int i = 0, j = 0;
  uint8_t path[8];

  while ((dev = devs[i++]) != NULL) {
    struct libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
      fprintf(stderr, "failed to get device descriptor");
      return 1;
    }

    printf("vid:%04x,pid:%04x (bus %d, device %d)", desc.idVendor,
           desc.idProduct, libusb_get_bus_number(dev),
           libusb_get_device_address(dev));

    r = libusb_get_port_numbers(dev, path, sizeof(path));
    if (r > 0) {
      printf(" path: %d", path[0]);
      for (j = 1; j < r; j++)
        printf(".%d", path[j]);
    }
    printf("\n");
  }

  libusb_free_device_list(devs, 1);

  libusb_exit(NULL);
}

AOA::AOA() {}
