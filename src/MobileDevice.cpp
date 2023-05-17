
#include "MobileDevice.h"

#include "proactor/Proactor.h"
#include "utils/log.h"

// the queue is same the EChannelType
const std::array<int, CHANNEL_NUMBER> MobileDevice::MD_PORTS = {
    CMD_SOCKET_PORT_MD,     VIDEO_SOCKET_PORT_MD, MEDIA_SOCKET_PORT_MD,
    TTS_SOCKET_PORT_MD,     VR_SOCKET_PORT_MD,    TOUCH_SOCKET_PORT_MD,
    UNKNOWN_SOCKET_PORT_MD,
};

MobileDevice::MobileDevice(Proactor &p)
    : is_aoa_service_(false), aoa_(p), sd_(p), vs_(p) {

  for (size_t i = 0; i < channels_.size(); ++i) {
    channels_[i] = BindSocket(p);
  }
  // register_func();
}

MobileDevice::~MobileDevice() { stop(); }

void MobileDevice::start() {
  begin_aoa_service();
  begin_tcp_service();
}

void MobileDevice::stop() {
  stop_tcp_service();
  stop_aoa_service();
}

void MobileDevice::begin_aoa_service() {
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
      md_.recv_data((EChannelType)channel, data, data_size);
      size -= data_size;
    } while (ret == 0 && size > 0);
  });
}

void MobileDevice::begin_tcp_service() {
  auto service_start = [this]() {
    stop();
    start();
  };
  auto service_stop = [this]() { stop(); };
  auto discovery_start = [this]() { sd_.start(); };
  auto discovery_stop = [this]() { sd_.stop(); };

  discovery_start();
  for (size_t i = 0; i < channels_.size(); ++i) {
    EChannelType channel_enum = (EChannelType)(i + 1);
    auto &bind = channels_[i];
    auto &client = connects_[i];

    auto bind_callback = [channel_enum, service_start, discovery_stop, &bind,
                          &client, this](const TcpStreamOp &op) {
      client.set(op);
      if (channel_enum == kCMD_CHANNEL) {
        register_tcp_func();
        client.start(md_, channel_enum, service_start);
      } else {
        client.start(md_, channel_enum, []() {});
      }
      discovery_stop();
      bind.stop();
    };

    bind.start(MD_PORTS[i], channel_enum, bind_callback);
  }
}

void MobileDevice::stop_tcp_service() {
  vs_.stop();
  sd_.stop();
  for (size_t i = 0; i < channels_.size(); ++i) {
    channels_[i].stop();
    connects_[i].stop();
  }
}

void MobileDevice::stop_aoa_service() {
  vs_.stop();
  aoa_.stop();
}

void MobileDevice::register_aoa_func() {
  stop_tcp_service();

  // register send function
  md_.register_send(kCMD_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return aoa_.write(buff, buff_size, kCMD_CHANNEL);
                    });
  md_.register_send(kVIDEO_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return aoa_.write(buff, buff_size, kVIDEO_CHANNEL);
                    });
  md_.register_send(kMEDIA_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return aoa_.write(buff, buff_size, kMEDIA_CHANNEL);
                    });
  md_.register_send(kTTS_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return aoa_.write(buff, buff_size, kTTS_CHANNEL);
                    });
  md_.register_send(kVR_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return aoa_.write(buff, buff_size, kVR_CHANNEL);
                    });
  md_.register_send(kCTRL_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return aoa_.write(buff, buff_size, kCTRL_CHANNEL);
                    });

  register_func();
}

void MobileDevice::register_tcp_func() {
  stop_aoa_service();

  // register send function
  md_.register_send(kCMD_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return connects_[kCMD_CHANNEL - 1].send(buff, buff_size);
                    });
  md_.register_send(
      kVIDEO_CHANNEL, [this](const char *buff, size_t buff_size) -> int {
        return connects_[kVIDEO_CHANNEL - 1].send(buff, buff_size);
      });
  md_.register_send(
      kMEDIA_CHANNEL, [this](const char *buff, size_t buff_size) -> int {
        return connects_[kMEDIA_CHANNEL - 1].send(buff, buff_size);
      });
  md_.register_send(kTTS_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return connects_[kTTS_CHANNEL - 1].send(buff, buff_size);
                    });
  md_.register_send(kVR_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return connects_[kVR_CHANNEL - 1].send(buff, buff_size);
                    });
  md_.register_send(kCTRL_CHANNEL,
                    [this](const char *buff, size_t buff_size) -> int {
                      return connects_[kCTRL_CHANNEL - 1].send(buff, buff_size);
                    });

  register_func();
}

/**

v4
to: Data: 0004000000018001	MSG_CMD_HU_PROTOCOL_VERSION
from: Data: 0002000000010002	MSG_CMD_PROTOCOL_VERSION_MATCH_STATUS
from: Data: 000000000001001b	MSG_CMD_FOREGROUND
from: Data: 0000000000010018	MSG_CMD_SCREEN_ON
from: Data: 01b3000000010004	MSG_CMD_MD_INFO
to: Data: 0023000000018027	MSG_CMD_STATISTIC_INFO
Data: 000000000001000200020002	MSG_VIDEO_HEARTBEAT
to: Data: 019c000000018003	MSG_CMD_HU_INFO
from: Data: 000200000001004b	MSG_CMD_MD_AUTHEN_RESULT
from: Data: 0000000000010051	MSG_CMD_MD_FEATURE_CONFIG_REQUEST
to: Data: 000000000001804c
to: Data: 0008000000018007	MSG_CMD_VIDEO_ENCODER_INIT
to: Data: 0098000000018052	MSG_CMD_HU_FEATURE_CONFIG_RESPONSE
from: Data: 0000000000010031	MSG_CMD_CAR_DATA_SUBSCRIBE
to: Data: 0002000000018032
from: Data: 0008000000010008	MSG_CMD_VIDEO_ENCODER_INIT_DONE
from: Data: 000000000001001b	MSG_CMD_FOREGROUND
from: Data: 0008000000010026	MSG_CMD_MODULE_STATUS
from: Data: 002c000000010026	MSG_CMD_MODULE_STATUS
to: Data: 0000000000018009	MSG_CMD_VIDEO_ENCODER_START
Data: 000000000001800900020002	MSG_VIDEO_HEARTBEAT
Data: 0000144762fcfe8a00020001	MSG_VIDEO_DATA

*/
void MobileDevice::register_func() {
  // register video data send
  vs_.register_send([this](const std::vector<unsigned char> &data) {
    // md_.send_heartbeat();

    uint64_t timestamp =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    md_.send_video_data(timestamp, (const char *)data.data(), data.size());
    LOG_TRACE("send video data timestamp %d and size %d", timestamp,
              data.size());
  });

  // register callback function
  md_.callback_version([this](int major, int minor) {
    LOG_INFO("HU version is %d:%d", major, minor);
    md_.send_match_ver(true);

    md_.send_foreground();
    // md_.send_background();
    md_.send_screen_on();
    // md_.send_vr_interrupt();

    md_.send_md_info(
        /*os */ "Android", /* board */ "raphael",
        /* bootloader */ "unknown",
        /* brand */ "Xiaomi", /* cpu_abi */ "arm64-v8a", /* cpu_abi2 */ "",
        /* device */ "raphael",
        /* display */ "RKQ1.200826.002 test-keys", /* fingerprint */
        "Xiaomi/raphael/raphael:11/RKQ1.200826.002/V12.5.6.0.RFKCNXM:user/"
        "release-keys",
        /* hardware */ "qcom",
        /* host */ "c5-xm-ota-bd022.bj", /* cid */ "RKQ1.200826.002",
        /* manufacturer */ "Xiaomi",
        /* model */ "Redmi K20 Pro", /* product */ "raphael",
        /* serial */ "unknown", /* codename */ "REL",
        /* incremental */ "V12.5.6.0.RFKCNXM", /* release */ "11",
        /* sdk */ "30", /* sdk_int */ 30, /* token */ "",
        /* btaddress */ "02:00:00:00:00:00",
        /* btname */ "8.0.9");
  });

  md_.callback_hu_info(
      [this](const std::string &os, const std::string &manufacturer,
             const std::string &model, const std::string &release,
             const std::string &btaddress, const std::string &btname) {
        LOG_INFO("HU info is os:%s, manufacturer:%s, model:%s, release:%s, "
                 "btaddress:%s, btname:%s",
                 os.c_str(), manufacturer.c_str(), model.c_str(),
                 release.c_str(), btaddress.c_str(), btname.c_str());

        md_.send_md_authen_result(true);
        md_.send_md_feature_config_request();
      });

  md_.callback_statistic_info([this](const std::string &cuid,
                                     const std::string &versionname,
                                     int versioncode,
                                     const std::string &channel,
                                     int connectcount, int connectsuccesscount,
                                     int connecttime,
                                     const std::string &crashlog) {
    LOG_INFO(
        "HU statistic info is cuid:%s, versionname:%s, versioncode:%d, "
        "channel:%s, "
        "connectcount:%d, connectsuccesscount:%d, connecttime:%d, crashlog:%s",
        cuid.c_str(), versionname.c_str(), versioncode, channel.c_str(),
        connectcount, connectsuccesscount, connecttime, crashlog.c_str());

    md_.send_heartbeat();

    // md_.send_media_info(/* source */, /* song */, /* artist */, /* album */,
    //                     /* albumart */, /* duration */, /* playlistnum */,
    //                     /* songid */,
    //                     /* mode */);
  });

  md_.callback_hu_feature_config_response([this]() {
    LOG_INFO("HU call feature config response");
    // car data
    md_.send_car_data_subscribe();
  });

  // car data {
  md_.callback_car_data_subscribe_done([](const vehicle_info_vec_t &datas) {
    LOG_INFO("HU call data subscribe done data size %d", datas.size());
    for (auto const &i : datas) {
      LOG_INFO("subscribe data [moduleid:%d, flag:%d, frequency:%d]",
               std::get<0>(i), std::get<1>(i), std::get<2>(i));
    }
  });
  // car data }

  // video {
  md_.callback_encoder_init([this](int width, int height, int framerate) {
    LOG_INFO("video encoder init width %d, height %d, framerate %d", width,
             height, framerate);

    // TODO
    vs_.init_video(width, height, 30, 4000000);
    md_.send_encoder_init_done(width, height, 60);
    md_.send_foreground();
    md_.send_module_status(
        {{CARLIFE_PHONE_MODULE_ID, PHONE_STATUS_IDLE},
         {CARLIFE_NAVI_MODULE_ID, NAVI_STATUS_IDLE},
         {CARLIFE_MUSIC_MODULE_ID, MUSIC_STATUS_IDLE},
         {CARLIFE_VR_MODULE_ID, VR_STATUS_RECORD_IDLE},
         {CARLIFE_MIC_MODULE_ID, MIC_STATUS_USE_VEHICLE_MIC}});
  });
  md_.callback_encoder_frame_rate_change([this](int framerate) {
    LOG_INFO("video encoder frame rate change framerate %d", framerate);

    // TODO
    vs_.init_video(0, 0, framerate, 4000000);
    md_.send_encoder_frame_rate_change_done(framerate);
  });
  md_.callback_encoder_start([this]() {
    LOG_INFO("video encoder start");
    // TODO
    vs_.start(40);
  });
  md_.callback_encoder_pause([this]() {
    LOG_INFO("video encoder pause");
    // TODO
    vs_.stop();
  });
  // video }
  // vr {
  md_.callback_mic_data(
      [this](uint64_t timestamp, const char *tts, size_t tts_size) {
        LOG_INFO("vr mic data received");
        // TODO
      });
  // vr }
  // control {
  md_.callback_action([this](int action, int x, int y) {
    LOG_INFO("action received action %d x %d y %d", action, x, y);
    // TODO
  });
  md_.callback_action_down([this](int x, int y) {
    LOG_INFO("action down received x %d y %d", x, y);
    // TODO
  });
  md_.callback_action_up([this](int x, int y) {
    LOG_INFO("action up received x %d y %d", x, y);
    // TODO
  });
  md_.callback_action_move([this](int x, int y) {
    LOG_INFO("action move received x %d y %d", x, y);
    // TODO
  });
  md_.callback_single_click([this](int x, int y) {
    LOG_INFO("single click received x %d y %d", x, y);
    // TODO
  });
  md_.callback_double_click([this](int x, int y) {
    LOG_INFO("double click received x %d y %d", x, y);
    // TODO
  });
  md_.callback_long_press([this](int x, int y) {
    LOG_INFO("long press received x %d y %d", x, y);
    // TODO
  });
  md_.callback_car_hard_key_code([this](int keycode) {
    LOG_INFO("key code received %d", keycode);
    // TODO
  });
  // control }

  md_.callback_module_control([this](int moduleid, int statusid) {
    LOG_INFO("module control moduble id %d, status id %d", moduleid, statusid);
    // TODO
    // md_.send_module_status({{moduleid, statusid}});
  });

  md_.callback_error_code([this](const std::string &errorcode) {
    LOG_INFO("error code %s", errorcode.c_str());
    // TODO
  });
}

void MobileDevice::BindSocket::start(
    int bind_port, EChannelType channel,
    std::function<void(const TcpStreamOp &)> bind_callback) {

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

  // accept
  auto accept_callback = [bind_callback, channel,
                          this](const std::error_code &re_ec,
                                const std::pair<TcpStreamOp, SocketAddr> &ac) {
    if (re_ec) {
      LOG_ERROR("channel %d accept %s:%d error (%d)%s", channel,
                ac.second.get_ip(), ac.second.get_port(), re_ec.value(),
                re_ec.message().c_str());
      return;
    }
    LOG_INFO("channel %d accept client from %s:%d", channel, ac.second.get_ip(),
             ac.second.get_port());

    bind_callback(ac.first);
  };
  op_.async_accept(accept_callback, ec);

  if (ec) {
    LOG_ERROR("async accept port %s error : (%d)%s", port, ec.value(),
              ec.message().c_str());

    ec.clear();
  }
}

void MobileDevice::BindSocket::stop() {
  if (op_.native_handle() == -1) {
    return;
  }
  std::error_code ec;
  op_.close(ec);
  if (ec) {
    LOG_ERROR("close MobileDevice error %d : (%d)%s", ec.value(),
              ec.message().c_str());
  }
}

void MobileDevice::DeviceSocket::start(CarlifeMobileDevice &mobile_device,
                                       EChannelType channel,
                                       std::function<void()> service_start) {

  std::error_code ec;
  op_.async_read(
      buff_, sizeof(buff_),
      [&mobile_device, channel, service_start,
       this](const std::error_code &re_ec, size_t data_size) {
        if (re_ec) {
          LOG_ERROR("channel %d client recv error: (%d)%s", channel,
                    re_ec.value(), re_ec.message().c_str());

          service_start();
          return;
        }
        if (data_size == 0) {
          LOG_WARN("channel %d client remote close", channel);
          // TODO: check client is exit
          service_start();
          return;
        }
        mobile_device.recv_data(channel, buff_, data_size);
        start(mobile_device, channel, service_start);
      },
      ec);

  if (ec) {
    LOG_ERROR("client async read channel %d error (%d)%s", channel, ec.value(),
              ec.message().c_str());
  }
}

void MobileDevice::DeviceSocket::stop() {
  if (op_.native_handle() == -1) {
    return;
  }
  std::error_code ec;
  op_.close(ec);
  if (ec) {
    LOG_ERROR("client close error (%d)%s", ec.value(), ec.message().c_str());
  }
}

void MobileDevice::DeviceSocket::set(const TcpStreamOp &stream) {
  if (stream.native_handle() == -1 || stream.native_handle() == 0) {
    LOG_ERROR("Invaild socket");
    return;
  }
  stop();
  op_ = stream;
}

int MobileDevice::DeviceSocket::send(const char *buff, size_t buff_size) {
  std::error_code ec;
  int n = op_.write(buff, buff_size, ec);
  if (ec) {
    LOG_ERROR("send buff error (%d)%s", ec.value(), ec.message().c_str());
    return -1;
  }
  return n;
}
