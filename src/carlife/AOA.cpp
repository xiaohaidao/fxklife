
#include "carlife/AOA.h"

#include <functional>

#include "libusb-1.0/libusb.h"
#include "utils/log.h"

// windows driver control:
// https://github.com/pbatard/libwdi/releases/download/v1.5.0/zadig-2.8.exe

/*
#define USB_READ_MAX_SIZE 1024

#define USB_VENDOR_GOOGLE 0x18d1
#define USB_PRODUCT_NEXUS_S 0x4e22

class OpenAccessory {

private:
  libusb_device_handle *dev;
  const static uint8_t in_addr = 0x85;
  const static uint8_t out_addr = 0x07;
  // #ifndef _MSC_VER
  //   const static array<uint32_t, 4> _pid {{0x2D00, 0x2D01, 0x2D04, 0x2D05}};
  // #else
  const static uint32_t _pid[];
  const static uint32_t _pid_size = 4;
  // #endif
  static bool lib_load;

  string man;
  string model;
  string desc;
  string ver;
  string url;
  string serial;
  uint32_t dev_vid;
  uint32_t dev_pid;

  void load_accessory(void);
  void change_device(void);
  void find_device(void);
  OpenAccessory(const OpenAccessory &rhs){};

public:
  static bool check_for_accessory(uint16_t vid, uint16_t pid);
  static void init(void);
  static void de_init(void);
  OpenAccessory(string manufacture, string model_name, string description,
                string version, string uri, string serial_number, uint32_t vid,
                uint32_t pid);
  void send_data(vector<uint8_t> data);
  void read_data(vector<uint8_t> &data);
  ~OpenAccessory();
};

bool OpenAccessory::lib_load = false;

namespace Until {
void sleep_ms(int ms) {
#ifdef WIN32
  Sleep(ms);
#else
  usleep(ms * 1000);
#endif
}
} // namespace Until

void OpenAccessory::init(void) {
  int ret;
  ret = libusb_init(NULL);
  if (ret != 0) {
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  lib_load = true;
}

OpenAccessory::OpenAccessory(string manufacture, string model_name,
                             string description, string version, string uri,
                             string serial_number, uint32_t vid, uint32_t pid)
    : man(manufacture), model(model_name), desc(description), ver(version),
      url(uri), serial(serial_number), dev_vid(vid), dev_pid(pid) {
  if (!lib_load) {
    throw(OpenAccessoryException("Lib not initzialised"));
  }
  try {
    load_accessory();
  } catch (OpenAccessoryException &e) {
    find_device();
    change_device();
    Until::sleep_ms(2000);
    load_accessory();
  }
}

bool OpenAccessory::check_for_accessory(uint16_t vid, uint16_t pid) {
  if (!lib_load) {
    throw(OpenAccessoryException("Lib not initzialised"));
  }
  libusb_device_handle *handle;
  if ((handle = libusb_open_device_with_vid_pid(NULL, vid, pid)) != NULL) {
    return false;
  }
  libusb_close(handle);
  return true;
}

void OpenAccessory::load_accessory(void) {
  bool set = false;
  // #ifndef _MSC_VER
  //   for(auto& p: _pid){

  // #else
  for (int i = 0; i < _pid_size; i++) {
    uint32_t p = _pid[i];
    // #endif

    if (set)
      continue;
    if ((dev = libusb_open_device_with_vid_pid(NULL, 0x18D1, p)) != NULL) {
      set = true;
    }
  }

  if (!set) {
    throw(OpenAccessoryException("set No Device Found"));
  }
  int ret;
  ret = libusb_claim_interface(dev, 0);
  if (ret != 0) {
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  Until::sleep_ms(10);
}

void OpenAccessory::change_device(void) {
  uint8_t buffer[2];
  int ret = libusb_control_transfer(dev, 0xC0, 51, 0, 0, buffer, 2, 0);
  if (ret < 0) {
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  Until::sleep_ms(20);
  if ((ret = libusb_control_transfer(
           dev, 0x40, 52, 0, 0, (uint8_t *)man.c_str(), man.size(), 0)) < 0) {
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }

  if ((ret = libusb_control_transfer(dev, 0x40, 52, 0, 1,
                                     (uint8_t *)model.c_str(), model.size() + 1,
                                     0)) < 0) {
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  if ((ret =
           libusb_control_transfer(dev, 0x40, 52, 0, 2, (uint8_t *)desc.c_str(),
                                   desc.size() + 1, 0)) < 0) {
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  if ((ret =
           libusb_control_transfer(dev, 0x40, 52, 0, 3, (uint8_t *)ver.c_str(),
                                   ver.size() + 1, 0)) < 0) {
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  if ((ret =
           libusb_control_transfer(dev, 0x40, 52, 0, 4, (uint8_t *)url.c_str(),
                                   url.size() + 1, 0)) < 0) {
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  if ((ret = libusb_control_transfer(dev, 0x40, 52, 0, 5,
                                     (uint8_t *)serial.c_str(),
                                     serial.size() + 1, 0)) < 0) {
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  if ((ret = libusb_control_transfer(dev, 0x40, 53, 0, 0, NULL, 0, 0)) < 0) {
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
#ifdef __linux__
  if (dev != NULL) {
    libusb_release_interface(dev, 0);
  }
#endif
  Until::sleep_ms(20);
}

void OpenAccessory::find_device(void) {
  if ((dev = libusb_open_device_with_vid_pid(NULL, dev_vid, dev_pid)) == NULL) {
    throw(OpenAccessoryException("find device No Device Found"));
  }
#ifdef __linux__
  int ret;
  libusb_detach_kernel_driver(dev, 0);
  ret = libusb_claim_interface(dev, 0);
  if (ret != 0) {
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
#endif
}

void OpenAccessory::send_data(vector<uint8_t> data) {
  int ret = 0;
  int sent = 0;
  ret = libusb_bulk_transfer(dev, out_addr, data.data(), data.size(), &sent, 0);
  if (ret < 0 || (unsigned int)ret != data.size()) {
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
}

void OpenAccessory::read_data(vector<uint8_t> &data) {
  int ret = 0;
  int size = 0;
  uint8_t buff[USB_READ_MAX_SIZE];
  data.clear();
  ret = libusb_bulk_transfer(dev, in_addr, buff, USB_READ_MAX_SIZE, &size, 0);
  cout << size << endl;
  if (ret < 0) {
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  data.assign(buff, buff + size);
}

OpenAccessory::~OpenAccessory() {
  libusb_release_interface(dev, 0);
  libusb_close(dev);
}

const uint32_t OpenAccessory::_pid[] = {0x2D00, 0x2D01, 0x2D04, 0x2D05};
*/

struct Construct {
  Construct() { libusb_init(nullptr); }
  ~Construct() { libusb_exit(nullptr); }
} construct;

class usb {
public:
  static constexpr uint8_t IN_ADDR = 0x81;  // LIBUSB_ENDPOINT_IN;  // 0x85;
  static constexpr uint8_t OUT_ADDR = 0x01; // LIBUSB_ENDPOINT_OUT; // 0x07;

  usb() : dev_handle_(nullptr) {}
  usb(libusb_device_handle *handle) : dev_handle_(handle) {}
  ~usb() {}

  static const char *error(int error_no) { return libusb_strerror(error_no); }
  static const char *err_name(int error_no) {
    return libusb_error_name(error_no);
  }

  int open(uint16_t vid, uint16_t pid) {
    if (dev_handle_) {
      close();
    }
    dev_handle_ = libusb_open_device_with_vid_pid(nullptr, vid, pid);
    if (!dev_handle_) {
      return -1;
    }
    return 0;
  }

  void close() {
    if (!dev_handle_) {
      return;
    }
    libusb_release_interface(dev_handle_, 0);
    libusb_close(dev_handle_);
    dev_handle_ = nullptr;
  }

  void init(const char *manufacture, const char *model_name,
            const char *description, const char *version, const char *uri,
            const char *serial_number, uint32_t vid, uint32_t pid) {}

  struct RequestType {
    union {
      uint8_t type;
      struct {
        uint8_t recipient : 5; // libusb_request_recipient
        uint8_t re_type : 2;   // libusb_request_type
        uint8_t dir : 1;       // libusb_endpoint_direction
      };
    };
  };
  int control(uint8_t type, uint8_t req, uint16_t wv, uint16_t wi,
              unsigned char *data, uint16_t len, unsigned int timeout_ms) {

    return libusb_control_transfer(dev_handle_, type, req, wv, wi, data, len,
                                   timeout_ms);
  }

  int bulk_before() {
    int ret = 0;
    // if ((ret = libusb_kernel_driver_active(dev_handle_, 0)) != 0) {
    //   LOG_TRACE("libusb_kernel_driver_active error (%s)%s", err_name(ret),
    //             error(ret));
    // }
    // if ((ret = libusb_detach_kernel_driver(dev_handle_, 0)) != 0) {
    //   LOG_TRACE("libusb_detach_kernel_driver error (%s)%s", err_name(ret),
    //             error(ret));
    // }
    if ((ret = libusb_claim_interface(dev_handle_, 0)) < 0) {
      LOG_TRACE("libusb_claim_interface error (%s)%s", err_name(ret),
                error(ret));
    }
    return ret;
  }

  int bulk(uint8_t endpoint, char *data, size_t data_size) {
    int len = 0;
    int ret = 0;
    if ((ret =
             libusb_bulk_transfer(dev_handle_, endpoint, (unsigned char *)data,
                                  (int)data_size, &len, 0)) < 0) {
      return ret;
    }
    return len;
  }

public: // hotplug
  static bool has_hotplug() {
    return libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG);
  }

  int register_callback(uint16_t vid, uint16_t pid,
                        libusb_hotplug_callback_fn callback,
                        libusb_hotplug_callback_handle &handle) {

    int ret = 0;
    if ((ret = libusb_hotplug_register_callback(
             nullptr,
             LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED |
                 LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
             0, vid, pid, LIBUSB_HOTPLUG_MATCH_ANY, callback, nullptr,
             &handle)) != LIBUSB_SUCCESS) {
      return ret;
    }
    return 0;
  }

  void unregister_callback(libusb_hotplug_callback_handle handle) {
    libusb_hotplug_deregister_callback(NULL, handle);
  }

  int run_once() { return libusb_handle_events_completed(nullptr, nullptr); }

public:
  void list_device(std::function<bool(libusb_device *)> callback) {
    libusb_device **devs;
    if (libusb_get_device_list(NULL, &devs) < 0) {
      // LOG_TRACE("libusb_get_device_list error\n");
      return;
    }

    libusb_device *dev;
    int i = 0;
    while ((dev = devs[i++]) != NULL) {
      if (callback(dev)) {
        break;
      }
    }

    libusb_free_device_list(devs, 1);
  }

  // auto print_list = [](const std::vector<libusb_device *> &list) {
  //   LOG_TRACE("...........\n");
  //   // auto list = list_device();
  //   for (auto const &dev : list) {
  //     struct libusb_device_descriptor desc;
  //     int r = libusb_get_device_descriptor(dev, &desc);
  //     if (r < 0) {
  //       LOG_TRACE("failed to get device descriptor: %s", libusb_strerror(r));
  //       return;
  //     }

  //     printf(
  //         "vid:%04x,pid:%04x (bus %d, device %d class type %x sub type %x)",
  //         desc.idVendor, desc.idProduct, libusb_get_bus_number(dev),
  //         libusb_get_device_address(dev), desc.bDeviceClass,
  //         desc.bDeviceSubClass);
  //     printf("usb version %x", desc.bcdUSB);

  //     uint8_t path[8];
  //     r = libusb_get_port_numbers(dev, path, sizeof(path));
  //     if (r > 0) {
  //       printf(" path: %d", path[0]);
  //       for (int j = 1; j < r; j++)
  //         printf(".%d", path[j]);
  //     }
  //     printf("\n");
  //   }
  // };

  libusb_device_handle *native_handle() const { return dev_handle_; }

private:
  libusb_device_handle *dev_handle_;
};

int run_aoa(usb &s, int vid, int pid) {
  int ret = 0;
  if ((ret = s.open(vid, pid)) < 0) {
    LOG_TRACE("open vid %x pid %x fail:%s!\n", vid, pid, usb::error(ret));
    return -1;
  }

  usb::RequestType type = {};
  type.type = LIBUSB_RECIPIENT_DEVICE | LIBUSB_REQUEST_TYPE_VENDOR |
              LIBUSB_ENDPOINT_IN; // type.re_type = 2; type.dir = 1;
  LOG_TRACE("request type dir %x type %x recipient %x", type.dir, type.re_type,
            type.recipient);
  uint8_t buffer[2] = {};
  if ((ret = s.control(type.type /*0xc0*/, 51, 0, 0, buffer, sizeof(buffer),
                       0)) < 0) {
    LOG_TRACE("control 0xc0 51 error: %s\n", usb::error(ret));
    return -1;
  }
  type.type = LIBUSB_REQUEST_TYPE_VENDOR; // type.dir = 0;
  LOG_TRACE("AOA version %d \n", buffer[1] << 8 | buffer[0]);
  uint8_t man[] = "Baidu";
  if ((ret = s.control(type.type /*0x40*/, 52, 0, 0, man, sizeof(man), 0)) <
      0) {
    LOG_TRACE("control 0x40 52 0 error: %s\n", usb::error(ret));
    return -1;
  }
  uint8_t model[] = "CarLife";
  if ((ret = s.control(type.type, 52, 0, 1, model, sizeof(model), 0)) < 0) {
    LOG_TRACE("control 0x40 52 1 error: %s\n", usb::error(ret));
    return -1;
  }
  uint8_t desc[] = "Baidu	CarLife";
  if ((ret = s.control(type.type, 52, 0, 2, desc, sizeof(desc), 0)) < 0) {
    LOG_TRACE("control 0x40 52 2 error: %s\n", usb::error(ret));
    return -1;
  }
  uint8_t ver[] = "1.0.0";
  if ((ret = s.control(type.type, 52, 0, 3, ver, sizeof(ver), 0)) < 0) {
    LOG_TRACE("control 0x40 52 3 error: %s\n", usb::error(ret));
    return -1;
  }
  uint8_t url[] = "http://carlife.baidu.com/";
  if ((ret = s.control(type.type, 52, 0, 4, url, sizeof(url), 0)) < 0) {
    LOG_TRACE("control 0x40 52 4 error: %s\n", usb::error(ret));
    return -1;
  }
  uint8_t serial[] = "0720SerialNo.";
  if ((ret = s.control(type.type, 52, 0, 5, serial, sizeof(serial), 0)) < 0) {
    LOG_TRACE("control 0x40 52 5 error: %s\n", usb::error(ret));
    return -1;
  }

  if ((ret = s.control(type.type, 53, 0, 0, nullptr, 0, 0)) < 0) {
    LOG_TRACE("control 0x40 53 0 error: %s\n", usb::error(ret));
    return -1;
  }
  return 0;
}

// see https://github.com/libusb/libusb/blob/master/examples/testlibusb.c
// {
static void print_endpoint_comp(
    const struct libusb_ss_endpoint_companion_descriptor *ep_comp) {
  printf("      USB 3.0 Endpoint Companion:\n");
  printf("        bMaxBurst:           %u\n", ep_comp->bMaxBurst);
  printf("        bmAttributes:        %02xh\n", ep_comp->bmAttributes);
  printf("        wBytesPerInterval:   %u\n", ep_comp->wBytesPerInterval);
}

static void print_endpoint(const struct libusb_endpoint_descriptor *endpoint) {
  int i, ret;

  printf("      Endpoint:\n");
  printf("        bEndpointAddress:    %02xh\n", endpoint->bEndpointAddress);
  printf("        bmAttributes:        %02xh\n", endpoint->bmAttributes);
  printf("        wMaxPacketSize:      %u\n", endpoint->wMaxPacketSize);
  printf("        bInterval:           %u\n", endpoint->bInterval);
  printf("        bRefresh:            %u\n", endpoint->bRefresh);
  printf("        bSynchAddress:       %u\n", endpoint->bSynchAddress);

  for (i = 0; i < endpoint->extra_length;) {
    if (LIBUSB_DT_SS_ENDPOINT_COMPANION == endpoint->extra[i + 1]) {
      struct libusb_ss_endpoint_companion_descriptor *ep_comp;

      ret =
          libusb_get_ss_endpoint_companion_descriptor(NULL, endpoint, &ep_comp);
      if (LIBUSB_SUCCESS != ret)
        continue;

      print_endpoint_comp(ep_comp);

      libusb_free_ss_endpoint_companion_descriptor(ep_comp);
    }

    i += endpoint->extra[i];
  }
}

static void
print_altsetting(const struct libusb_interface_descriptor *interface) {
  uint8_t i;

  printf("    Interface:\n");
  printf("      bInterfaceNumber:      %u\n", interface->bInterfaceNumber);
  printf("      bAlternateSetting:     %u\n", interface->bAlternateSetting);
  printf("      bNumEndpoints:         %u\n", interface->bNumEndpoints);
  printf("      bInterfaceClass:       %u\n", interface->bInterfaceClass);
  printf("      bInterfaceSubClass:    %u\n", interface->bInterfaceSubClass);
  printf("      bInterfaceProtocol:    %u\n", interface->bInterfaceProtocol);
  printf("      iInterface:            %u\n", interface->iInterface);

  for (i = 0; i < interface->bNumEndpoints; i++)
    print_endpoint(&interface->endpoint[i]);
}

static void
print_2_0_ext_cap(struct libusb_usb_2_0_extension_descriptor *usb_2_0_ext_cap) {
  printf("    USB 2.0 Extension Capabilities:\n");
  printf("      bDevCapabilityType:    %u\n",
         usb_2_0_ext_cap->bDevCapabilityType);
  printf("      bmAttributes:          %08xh\n", usb_2_0_ext_cap->bmAttributes);
}

static void print_ss_usb_cap(
    struct libusb_ss_usb_device_capability_descriptor *ss_usb_cap) {
  printf("    USB 3.0 Capabilities:\n");
  printf("      bDevCapabilityType:    %u\n", ss_usb_cap->bDevCapabilityType);
  printf("      bmAttributes:          %02xh\n", ss_usb_cap->bmAttributes);
  printf("      wSpeedSupported:       %u\n", ss_usb_cap->wSpeedSupported);
  printf("      bFunctionalitySupport: %u\n",
         ss_usb_cap->bFunctionalitySupport);
  printf("      bU1devExitLat:         %u\n", ss_usb_cap->bU1DevExitLat);
  printf("      bU2devExitLat:         %u\n", ss_usb_cap->bU2DevExitLat);
}

static void print_bos(libusb_device_handle *handle) {
  struct libusb_bos_descriptor *bos;
  uint8_t i;
  int ret;

  ret = libusb_get_bos_descriptor(handle, &bos);
  if (ret < 0)
    return;

  printf("  Binary Object Store (BOS):\n");
  printf("    wTotalLength:            %u\n", bos->wTotalLength);
  printf("    bNumDeviceCaps:          %u\n", bos->bNumDeviceCaps);

  for (i = 0; i < bos->bNumDeviceCaps; i++) {
    struct libusb_bos_dev_capability_descriptor *dev_cap =
        bos->dev_capability[i];

    if (dev_cap->bDevCapabilityType == LIBUSB_BT_USB_2_0_EXTENSION) {
      struct libusb_usb_2_0_extension_descriptor *usb_2_0_extension;

      ret = libusb_get_usb_2_0_extension_descriptor(NULL, dev_cap,
                                                    &usb_2_0_extension);
      if (ret < 0)
        return;

      print_2_0_ext_cap(usb_2_0_extension);
      libusb_free_usb_2_0_extension_descriptor(usb_2_0_extension);
    } else if (dev_cap->bDevCapabilityType ==
               LIBUSB_BT_SS_USB_DEVICE_CAPABILITY) {
      struct libusb_ss_usb_device_capability_descriptor *ss_dev_cap;

      ret = libusb_get_ss_usb_device_capability_descriptor(NULL, dev_cap,
                                                           &ss_dev_cap);
      if (ret < 0)
        return;

      print_ss_usb_cap(ss_dev_cap);
      libusb_free_ss_usb_device_capability_descriptor(ss_dev_cap);
    }
  }

  libusb_free_bos_descriptor(bos);
}

static void print_interface(const struct libusb_interface *interface) {
  int i;

  for (i = 0; i < interface->num_altsetting; i++)
    print_altsetting(&interface->altsetting[i]);
}

static void print_configuration(struct libusb_config_descriptor *config) {
  uint8_t i;

  printf("  Configuration:\n");
  printf("    wTotalLength:            %u\n", config->wTotalLength);
  printf("    bNumInterfaces:          %u\n", config->bNumInterfaces);
  printf("    bConfigurationValue:     %u\n", config->bConfigurationValue);
  printf("    iConfiguration:          %u\n", config->iConfiguration);
  printf("    bmAttributes:            %02xh\n", config->bmAttributes);
  printf("    MaxPower:                %u\n", config->MaxPower);

  for (i = 0; i < config->bNumInterfaces; i++)
    print_interface(&config->interface[i]);
}

static void print_device(libusb_device *dev, libusb_device_handle *handle) {
  struct libusb_device_descriptor desc;
  unsigned char string[256];
  const char *speed;
  int ret;
  uint8_t i;
  bool verbose = true;

  switch (libusb_get_device_speed(dev)) {
  case LIBUSB_SPEED_LOW:
    speed = "1.5M";
    break;
  case LIBUSB_SPEED_FULL:
    speed = "12M";
    break;
  case LIBUSB_SPEED_HIGH:
    speed = "480M";
    break;
  case LIBUSB_SPEED_SUPER:
    speed = "5G";
    break;
  case LIBUSB_SPEED_SUPER_PLUS:
    speed = "10G";
    break;
  default:
    speed = "Unknown";
  }

  ret = libusb_get_device_descriptor(dev, &desc);
  if (ret < 0) {
    fprintf(stderr, "failed to get device descriptor");
    return;
  }

  printf("Dev (bus %u, device %u): %04X - %04X speed: %s\n",
         libusb_get_bus_number(dev), libusb_get_device_address(dev),
         desc.idVendor, desc.idProduct, speed);

  if (!handle)
    libusb_open(dev, &handle);

  if (handle) {
    if (desc.iManufacturer) {
      ret = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer,
                                               string, sizeof(string));
      if (ret > 0)
        printf("  Manufacturer:              %s\n", (char *)string);
    }

    if (desc.iProduct) {
      ret = libusb_get_string_descriptor_ascii(handle, desc.iProduct, string,
                                               sizeof(string));
      if (ret > 0)
        printf("  Product:                   %s\n", (char *)string);
    }

    if (desc.iSerialNumber && verbose) {
      ret = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber,
                                               string, sizeof(string));
      if (ret > 0)
        printf("  Serial Number:             %s\n", (char *)string);
    }
  }

  if (verbose) {
    for (i = 0; i < desc.bNumConfigurations; i++) {
      struct libusb_config_descriptor *config;

      ret = libusb_get_config_descriptor(dev, i, &config);
      if (LIBUSB_SUCCESS != ret) {
        printf("  Couldn't retrieve descriptors\n");
        continue;
      }

      print_configuration(config);

      libusb_free_config_descriptor(config);
    }

    if (handle && desc.bcdUSB >= 0x0201)
      print_bos(handle);
  }

  if (handle)
    libusb_close(handle);
}
// }

void aoa_read_write(usb &s, int vid, int pid) {
  int ret = 0;
  if ((ret = s.open(vid, pid)) < 0) {
    LOG_TRACE("open vid %x pid %x fail:(%s)%s!\n", vid, pid,
              libusb_error_name(ret), usb::error(ret));
    return;
  }

  char data[1024] = {};
  s.bulk_before();
  while (1) {
    if ((ret = s.bulk(usb::IN_ADDR, data, sizeof(data))) < 0) {
      LOG_TRACE("bulk %x error fail:(%s)%s!\n", usb::IN_ADDR,
                libusb_error_name(ret), usb::error(ret));
      return;
    }
    LOG_TRACE("recv data size %d", ret);
  }
}

void enter_aoa1() {
  usb s;
  LOG_TRACE("has hotplug %d", s.has_hotplug());
  auto turn_aoa = [&s](libusb_device *dev) -> bool {
    struct libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
      LOG_TRACE("failed to get device descriptor: %s", libusb_strerror(r));
      return false;
    }
    int ret = run_aoa(s, desc.idVendor, desc.idProduct);
    if (ret == 0) {
      LOG_TRACE("vid %x pid %x enter aoa", desc.idVendor, desc.idProduct);
    }
    return ret == 0;
  };
  s.list_device(turn_aoa);
  s.close();
}

void print_devices() {
  usb s;
  s.list_device([](libusb_device *dev) -> bool {
    print_device(dev, nullptr);
    return false;
  });
  s.close();
}

void enter_aoa2() {
  usb s;
  auto print_list = [](libusb_device *dev) -> bool {
    struct libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
      LOG_TRACE("failed to get device descriptor: %s", libusb_strerror(r));
      return true;
    }

    printf("vid:%04x,pid:%04x (bus %d, device %d class type %x sub type %x)",
           desc.idVendor, desc.idProduct, libusb_get_bus_number(dev),
           libusb_get_device_address(dev), desc.bDeviceClass,
           desc.bDeviceSubClass);
    printf("usb version %x", desc.bcdUSB);

    uint8_t path[8];
    r = libusb_get_port_numbers(dev, path, sizeof(path));
    if (r > 0) {
      printf(" path: %d", path[0]);
      for (int j = 1; j < r; j++)
        printf(".%d", path[j]);
    }
    printf("\n");
    return false;
  };
  s.list_device(print_list); // true break retry, otherwise retry
  // s.list_device([](libusb_device *dev) -> bool {
  //   print_device(dev, nullptr);
  //   return false;
  // });
  LOG_TRACE("has hotplug %d", s.has_hotplug());
  int vid = 0, pid = 0, type = 0;
  printf("type vid pid:");
  scanf("%d %x %x", &type, &vid, &pid);
  LOG_TRACE("pid and vid %x:%x\n", vid, pid);

  if (type == 0) {
    run_aoa(s, vid, pid);
  } else {
    aoa_read_write(s, vid, pid);
  }
  s.close();
}

AOA::AOA() : handle_(nullptr) {
  // print_devices();
  // enter_aoa();
}

AOA::~AOA() { close(); }

void AOA::enter_aoa() {
  usb s;
  auto turn_aoa = [&s](libusb_device *dev) -> bool {
    struct libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
      LOG_TRACE("failed to get device descriptor: %s", libusb_strerror(r));
      return false;
    }
    int ret = run_aoa(s, desc.idVendor, desc.idProduct);
    if (ret == 0) {
      LOG_TRACE("vid %x pid %x enter aoa", desc.idVendor, desc.idProduct);
    }
    return ret == 0;
  };
  s.list_device(turn_aoa); // true break retry, otherwise retry
  s.close();
}

std::vector<std::pair<int, int>> AOA::list_aoa_device() {
  usb s;
  std::vector<std::pair<int, int>> list_device;
  auto list_aoa = [&s, &list_device](libusb_device *dev) -> bool {
    struct libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
      LOG_TRACE("failed to get device descriptor: %s", libusb_strerror(r));
      return false;
    }
    if (desc.idVendor == 0x18d1 &&
        (desc.idProduct >= 0x2d00 && desc.idProduct <= 0x2d05)) {

      list_device.push_back(std::make_pair(desc.idVendor, desc.idProduct));
    }
    return false;
  };
  s.list_device(list_aoa); // true break retry, otherwise retry
  s.close();

  return list_device;
}

void AOA::connect(const std::pair<int, int> &aoa_index) {
  if (handle_) {
    close();
  }
  usb s;
  s.open(aoa_index.first, aoa_index.second);
  handle_ = s.native_handle();
  s.bulk_before();
}

void AOA::close() {
  usb(handle_).close();
  handle_ = nullptr;
}

int AOA::read(char *buff, size_t buff_size) {
  usb s(handle_);
  int ret = 0;
  if ((ret = s.bulk(usb::IN_ADDR, (char *)buff, buff_size)) < 0) {
    LOG_TRACE("bulk %x error fail:(%s)%s!\n", usb::IN_ADDR,
              libusb_error_name(ret), usb::error(ret));
  }

  return 0;
}

int AOA::write(const char *buff, size_t buff_size) {
  usb s(handle_);
  int ret = 0;
  if ((ret = s.bulk(usb::OUT_ADDR, (char *)buff, buff_size)) < 0) {
    LOG_TRACE("bulk %x error fail:(%s)%s!\n", usb::OUT_ADDR,
              libusb_error_name(ret), usb::error(ret));
  }
  return ret;
}

// VID 0x18d1
// PID
//     0x2d00 supports accessory mode, 1 interface 2 bulk endpoints
//     0x2d01 supports accessory mode + adb, 2 interface,2 bulk endpoints
//     0x2d02 supports audio, control streaming
//     0x2d03 supports audio + adb, 2 audio 1 bulk
//     0x2d04 supports AOA 1.0 + audio, 2 audio 1 bulk
//     0x2d05 supports AOA 1.0 + audio + adb, 2 audio 2 bulk
//
// AD endpoint 0 write 51, return 1,2 or other
// AC write 52,
//     Manufacturer(Baidu),
//     Model	Name(Carlife),
//     Description(Baidu	CarLife),
//     Version(1.0.0),
//     URL(http://carlife.baidu.com/),
//     Serial	Number(0720SerialNo.)
// AD write 53,
