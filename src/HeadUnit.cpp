
#include "HeadUnit.h"

#include "proactor/Proactor.h"
#include "utils/log.h"

// the queue is same the EChannelType
const std::array<int, CHANNEL_NUMBER> HeadUnit::MD_PORTS = {
    CMD_SOCKET_PORT_MD,     VIDEO_SOCKET_PORT_MD, MEDIA_SOCKET_PORT_MD,
    TTS_SOCKET_PORT_MD,     VR_SOCKET_PORT_MD,    TOUCH_SOCKET_PORT_MD,
    UNKNOWN_SOCKET_PORT_MD,
};

HeadUnit::HeadUnit(Proactor &p) : is_aoa_service_(false), aoa_(p), sd_(p) {

  for (size_t i = 0; i < connects_.size(); ++i) {
    connects_[i].set(TcpStreamOp(&p));
  }
  // register_func();
}

HeadUnit::~HeadUnit() { stop(); }

void HeadUnit::start() { service_dicovery(); }

void HeadUnit::begin_aoa_service() {
  is_aoa_service_ = false;
  aoa_.start([this]() {
    if (!is_aoa_service_) {
      register_aoa_func();
    }
    is_aoa_service_ = true;

    int channel = 0;
    size_t size = 0;
    if (aoa_.read(size, channel) != 0) {
      LOG_ERROR("read message from aoa error");
      stop();
      start();
      return;
    }
    char data[1024] = {};
    int ret = 0;
    do {
      ret = aoa_.read(data, sizeof(data));

      size_t data_size = size > sizeof(data) ? sizeof(data) : size;
      hu_.recv_data((EChannelType)channel, data, data_size);
      size -= data_size;
    } while (ret == 0 && size > 0);
  });
}

void HeadUnit::begin_tcp_service(const SocketAddr &target) {
  auto discovery_start = [this]() { service_dicovery(); };
  auto discovery_stop = [this]() { sd_.stop(); };

  for (size_t i = 0; i < connects_.size(); ++i) {
    EChannelType channcel_enum = (EChannelType)(i + 1);
    auto &client = connects_[i];

    auto first_callback = [this]() {
      register_tcp_func(); /*first_send();*/
    };

    SocketAddr t = target;
    t.set_port(MD_PORTS[i]);
    if (channcel_enum == kCMD_CHANNEL) {
      client.start(t, hu_, channcel_enum, first_callback, discovery_start);
    } else {
      client.start(
          t, hu_, channcel_enum, []() {}, []() {});
    }
  }

  // stop aoa
  stop_aoa_service();
}

void HeadUnit::stop() {
  stop_tcp_service();
  stop_aoa_service();
}

void HeadUnit::stop_tcp_service() {
  sd_.stop();
  for (size_t i = 0; i < connects_.size(); ++i) {
    connects_[i].stop();
  }
}

void HeadUnit::stop_aoa_service() { aoa_.stop(); }

void HeadUnit::register_aoa_func() {
  stop_tcp_service();

  // register send function
  hu_.register_send(kCMD_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return aoa_.write(buff, buff_size, kCMD_CHANNEL);
                    });
  hu_.register_send(kVIDEO_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return aoa_.write(buff, buff_size, kVIDEO_CHANNEL);
                    });
  hu_.register_send(kMEDIA_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return aoa_.write(buff, buff_size, kMEDIA_CHANNEL);
                    });
  hu_.register_send(kTTS_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return aoa_.write(buff, buff_size, kTTS_CHANNEL);
                    });
  hu_.register_send(kVR_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return aoa_.write(buff, buff_size, kVR_CHANNEL);
                    });
  hu_.register_send(kCTRL_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return aoa_.write(buff, buff_size, kCTRL_CHANNEL);
                    });

  register_func();

  first_send();
}

void HeadUnit::register_tcp_func() {
  stop_aoa_service();

  // register send function
  hu_.register_send(kCMD_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return connects_[kCMD_CHANNEL - 1].send(buff, buff_size);
                    });
  hu_.register_send(
      kVIDEO_CHANNEL, [this](const char *buff, size_t buff_size) -> int {
        return connects_[kVIDEO_CHANNEL - 1].send(buff, buff_size);
      });
  hu_.register_send(
      kMEDIA_CHANNEL, [this](const char *buff, size_t buff_size) -> int {
        return connects_[kMEDIA_CHANNEL - 1].send(buff, buff_size);
      });
  hu_.register_send(kTTS_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return connects_[kTTS_CHANNEL - 1].send(buff, buff_size);
                    });
  hu_.register_send(kVR_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return connects_[kVR_CHANNEL - 1].send(buff, buff_size);
                    });
  hu_.register_send(kCTRL_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return connects_[kCTRL_CHANNEL - 1].send(buff, buff_size);
                    });

  register_func();
  first_send();
}

void HeadUnit::register_func() {
  // register callback function
  hu_.callback_match_ver([this](bool match) {
    LOG_INFO("MD version match %d", match);

    hu_.send_hu_info(/* os */ "Android", /* manufacturer */ "Xiaomi",
                     /* model */ "Redmi K20 Pro",
                     /* release */ "11",
                     /* btaddress */ "02:00:00:00:00:00",
                     /* btname */ "");
  });

  hu_.callback_md_feature_config_request([this]() {
    LOG_INFO("MD feature config request");

    hu_.send_statistic_info(/* cuid */ "F9CD4AD0636D0C6B6C3E0BDBC94FCAB4|0",
                            /* versionname */ "3.0.0",
                            /* versioncode */ 15, /* channel */ "20022100",
                            /* connectcount */ 1, /* connectsuccesscount */ 1,
                            /* connecttime */ 50,
                            /* crashlog */ "");
  });

  hu_.callback_md_authen_result([this](bool authenresult) {
    LOG_INFO("MD authen result %d", authenresult);

    // video
    hu_.send_encoder_init(1920, 1080, 30);
  });

  hu_.callback_md_info(
      [this](const std::string &os, const std::string &board,
             const std::string &bootloader, const std::string &brand,
             const std::string &cpu_abi, const std::string &cpu_abi2,
             const std::string &device, const std::string &display,
             const std::string &fingerprint, const std::string &hardware,
             const std::string &host, const std::string &cid,
             const std::string &manufacturer, const std::string &model,
             const std::string &product, const std::string &serial,
             const std::string &codename, const std::string &incremental,
             const std::string &release, const std::string &sdk, int sdk_int,
             const std::string &token, const std::string &btaddress,
             const std::string &btname) {
        LOG_INFO(
            "MD info is os:%s, board:%s, bootloader:%s, brand:%s, cpu_abi:%s, "
            "cpu_abi2:%s, device:%s, display:%s, fingerprint:%s, hardware:%s, "
            "host:%s, cid:%s, manufacturer:%s, model:%s, product:%s, "
            "serial:%s, codename:%s, incremental:%s, release:%s, sdk:%s, "
            "sdk_int:%d, token:%s, btaddress:%s, btname:%s",
            os.c_str(), board.c_str(), bootloader.c_str(), brand.c_str(),
            cpu_abi.c_str(), cpu_abi2.c_str(), device.c_str(), display.c_str(),
            fingerprint.c_str(), hardware.c_str(), host.c_str(), cid.c_str(),
            manufacturer.c_str(), model.c_str(), product.c_str(),
            serial.c_str(), codename.c_str(), incremental.c_str(),
            release.c_str(), sdk.c_str(), sdk_int, token.c_str(),
            btaddress.c_str(), btname.c_str());

        hu_.send_module_control(CARLIFE_CONNECT_MODULE_ID, CONNECT_STATUS_WIFI);
        // video
        hu_.send_encoder_init(1920, 1080, 30);
      });

  hu_.callback_screen_on([this]() { LOG_INFO("MD call screen on"); });
  hu_.callback_screen_off([this]() { LOG_INFO("MD call screen off"); });
  hu_.callback_screen_userpresent(
      [this]() { LOG_INFO("MD call screen userpresent"); });
  hu_.callback_foreground([this]() { LOG_INFO("MD call foreground"); });
  hu_.callback_background([this]() { LOG_INFO("MD call background"); });

  // video {
  hu_.callback_encoder_init_done([this](int width, int height, int framerate) {
    LOG_INFO("MD video encoder init done width %d, height %d, framerate %d",
             width, height, framerate);

    // hu_.send_module_status({{CARLIFE_CONNECT_MODULE_ID,
    // CONNECT_STATUS_WIFI}}); hu_.send_media_info(/* source */, /* song */, /*
    // artist */, /* album */,
    //                     /* albumart */, /* duration */, /* playlistnum */,
    //                     /* songid */,
    //                     /* mode */);

    hu_.send_encoder_start();
  });

  FILE *ff = fopen("view_out.h264", "wb");
  hu_.callback_video_data(
      [this, ff](uint64_t timestamp, const char *data, size_t data_size) {
        // LOG_INFO("timestamp %ul recv video data size %d", timestamp,
        // data_size);

        fwrite(data, 1, data_size, ff);
      });
  // video }
  // vr {
  hu_.callback_vr_interrupt([]() { LOG_INFO("MD vr interrupt"); });
  // vr }
  // control {
  // control }

  hu_.callback_module_status([](const status_vec_t &status) {
    LOG_INFO("MD moduel status size %d", status.size());
    for (auto const &i : status) {
      LOG_INFO("status %d:%d", i.first, i.second);
    }
  });
}

void HeadUnit::first_send() { hu_.send_version(1, 0); }

void HeadUnit::service_dicovery() {
  // tcp service
  sd_.start(7999, [this](const SocketAddr &target) {
    begin_tcp_service(target);
    sd_.stop();
  });

  // aoa service
  begin_aoa_service();
}

void HeadUnit::BindUdpSocket::start(
    int bind_port, std::function<void(const SocketAddr &)> bind_callback) {

  if (op_.native_handle() != -1) {
    return;
  }
  // bind port
  std::error_code ec;
  char port[32] = {};
  snprintf(port, sizeof(port), "%d", bind_port);
  op_.bind(port, ec);
  if (ec) {
    LOG_ERROR("bind port %s error : (%d)%s", port, ec.value(),
              ec.message().c_str());

    return;
  }

  // async read
  auto read_callback = [bind_callback, this](const std::error_code &re_ec,
                                             size_t recv_size,
                                             const SocketAddr &target) {
    if (re_ec) {
      LOG_ERROR("accept error (%d)%s", re_ec.value(), re_ec.message().c_str());
      return;
    }
    static char BUFF[] = "CarlifeHost";
    if (recv_size < sizeof(BUFF) - 1) {
      return;
    }
    LOG_INFO("upd read client from %s:%d", target.get_ip(), target.get_port());
    bind_callback(target);
  };
  op_.async_read(buff_, sizeof(buff_), read_callback, ec);

  if (ec) {
    LOG_ERROR("async read port %s error : (%d)%s", port, ec.value(),
              ec.message().c_str());

    ec.clear();
  }
  LOG_TRACE("wait the broadcast");
}

void HeadUnit::BindUdpSocket::stop() {
  if (op_.native_handle() == -1) {
    return;
  }
  std::error_code ec;
  op_.close(ec);
  if (ec) {
    LOG_ERROR("close HeadUnit error %d : (%d)%s", ec.value(),
              ec.message().c_str());
  }
}

void HeadUnit::DeviceSocket::start(const SocketAddr &target,
                                   CarlifeHeadUnit &hu, EChannelType channel,
                                   std::function<void()> first_callback,
                                   std::function<void()> service_start) {

  LOG_INFO("async connect %s:%d", target.get_ip(), target.get_port());
  std::error_code ec;
  op_.async_connect(
      target,
      [this, &hu, channel, first_callback,
       service_start](const std::error_code &re_ec, size_t) {
        if (re_ec) {
          LOG_ERROR("channel %d client connect error: (%d)%s", channel,
                    re_ec.value(), re_ec.message().c_str());

          service_start();
          return;
        }
        start_recv(hu, channel, service_start);
        first_callback();
      },
      ec);
  if (ec) {
    LOG_ERROR("client async read channel %d error (%d)%s", channel, ec.value(),
              ec.message().c_str());

    service_start();
    return;
  }
}
void HeadUnit::DeviceSocket::start_recv(CarlifeHeadUnit &hu,
                                        EChannelType channel,
                                        std::function<void()> service_start) {

  auto read_callback = [this, &hu, channel, service_start](
                           const std::error_code &re_ec, size_t data_size) {
    if (re_ec) {
      LOG_ERROR("channel %d client recv error: (%d)%s", channel, re_ec.value(),
                re_ec.message().c_str());

      service_start();
      return;
    }
    if (data_size == 0) {
      LOG_WARN("channel %d client remote close", channel);
      service_start();
      return;
    }
    hu.recv_data(channel, buff_, data_size);
    start_recv(hu, channel, service_start);
  };

  std::error_code ec;
  op_.async_read(buff_, sizeof(buff_), read_callback, ec);

  if (ec) {
    LOG_ERROR("client async read channel %d error (%d)%s", channel, ec.value(),
              ec.message().c_str());
  }
}

void HeadUnit::DeviceSocket::stop() {
  if (op_.native_handle() == -1) {
    return;
  }
  std::error_code ec;
  op_.close(ec);
  if (ec) {
    LOG_ERROR("client close error (%d)%s", ec.value(), ec.message().c_str());
  }
}

void HeadUnit::DeviceSocket::set(const TcpStreamOp &stream) {
  stop();
  op_ = stream;
}

int HeadUnit::DeviceSocket::send(const char *buff, size_t buff_size) {
  std::error_code ec;
  int n = op_.write(buff, buff_size, ec);
  if (ec) {
    LOG_ERROR("send buff error (%d)%s", ec.value(), ec.message().c_str());
    return -1;
  }
  return n;
}
