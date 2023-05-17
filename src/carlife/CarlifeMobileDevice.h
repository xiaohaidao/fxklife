
#ifndef CARLIFE_CARLIFEMOBILEDEVICE_H
#define CARLIFE_CARLIFEMOBILEDEVICE_H

#include "carlife/channel/CmdChannel.h"
#include "carlife/channel/CtrlChannel.h"
#include "carlife/channel/MediaChannel.h"
#include "carlife/channel/TTSChannel.h"
#include "carlife/channel/VRChannel.h"
#include "carlife/channel/VideoChannel.h"

class CarlifeMobileDevice {
public:
  CarlifeMobileDevice();
  ~CarlifeMobileDevice();

public: // cmd // MD->HU
  // MD->HU
  IMPL_MEMBER_FUNC_ONE(match_ver, cmd_, bool, match)

  // MD->HU
  IMPL_MEMBER_FUNC_TWENTY_FOUR(
      md_info, cmd_, const std::string &, os, const std::string &, board,
      const std::string &, bootloader, const std::string &, brand,
      const std::string &, cpu_abi, const std::string &, cpu_abi2,
      const std::string &, device, const std::string &, display,
      const std::string &, fingerprint, const std::string &, hardware,
      const std::string &, host, const std::string &, cid, const std::string &,
      manufacturer, const std::string &, model, const std::string &, product,
      const std::string &, serial, const std::string &, codename,
      const std::string &, incremental, const std::string &, release,
      const std::string &, sdk, int, sdk_int, const std::string &, token,
      const std::string &, btaddress, const std::string &, btname)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(screen_on, cmd_)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(screen_off, cmd_)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(screen_userpresent, cmd_)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(foreground, cmd_)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(background, cmd_)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(launch_exit, cmd_)

  // MD->HU
  // parameter: ActionType, TurnKind
  IMPL_MEMBER_FUNC_SIX(navi_next_turn_info, cmd_, int, action, int, nextturn,
                       const std::string &, roadname, int, totaldistance, int,
                       remaindistance, int, time)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(car_data_subscribe, cmd_)

  // MD->HU
  IMPL_MEMBER_FUNC_THREE(car_data_subscribe_start, cmd_, int, moduleid, int,
                         flag, int, frequency)

  // MD->HU
  IMPL_MEMBER_FUNC_THREE(car_data_subscribe_stop, cmd_, int, moduleid, int,
                         flag, int, frequency)

  // MD->HU
  IMPL_MEMBER_FUNC_NINE(media_info, cmd_, const std::string &, source,
                        const std::string &, song, const std::string &, artist,
                        const std::string &, album, const std::string &,
                        albumart, int, duration, int, playlistnum,
                        const std::string &, songid, int, mode)

  // MD->HU
  // parameter: units ms
  IMPL_MEMBER_FUNC_ONE(media_progress_bar, cmd_, int, progressbar)

  // MD->HU
  IMPL_MEMBER_FUNC_ONE(connect_exception, cmd_, int, exceptiontype)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(request_go_to_foreground, cmd_)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(ui_action_sound, cmd_)

  // MD->HU
  IMPL_MEMBER_FUNC_ONE(carlife_data_subscribe_done, cmd_,
                       const mobile_info_vec_t &, infos)

  // MD->HU
  IMPL_MEMBER_FUNC_SIX(navi_assitantguide_info, cmd_, int, action, int,
                       assistanttype, int, trafficsigntype, int, totaldistance,
                       int, remaindistance, int, cameraspeed)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(go_to_foreground_response, cmd_)

  // MD->HU
  IMPL_MEMBER_FUNC_TWO(start_bt_autopair_request, cmd_, int, ostype,
                       const std::string &, address)

  // MD->HU
  IMPL_MEMBER_FUNC_ONE(bt_hfp_status_request, cmd_, int, type)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(md_feature_config_request, cmd_)

  // MD->HU
  IMPL_MEMBER_FUNC_TWO(bt_identify_result_ind, cmd_, int, status,
                       const std::string &, address)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(video_encoder_jpeg_ack, cmd_)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(md_exit, cmd_)

public: // cmd // HU->MD
  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_TWO(version, cmd_, int, majorversion, int,
                                minorversion)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_SIX(hu_info, cmd_, const std::string &, os,
                                const std::string &, manufacturer,
                                const std::string &, model, const std::string &,
                                release, const std::string &, btaddress,
                                const std::string &, btname)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_TWO(car_velocity, cmd_, int, speed, uint64_t,
                                timestamp)

  // HU->MD
  // defualt send frequency 1HZ
  // parameter: antennastate(dBHz), latitude(1/1,000,000 degrees),
  // longitude(1/1,000,000 degrees), height(0.1 meters), speed(0.01 km/h units),
  // heading(0.1 degrees), pdop(units	0.1), satsused, satsvisible, timestamp
  IMPL_CALLBACK_MEMBER_FUNC_TEN(car_gps, cmd_, uint32_t, antennastate, int,
                                latitude, int, longitude, int, height, uint32_t,
                                speed, uint32_t, heading, uint32_t, pdop,
                                uint32_t, satsused, uint32_t, satsvisible,
                                uint64_t, timestamp)

  // HU->MD
  // defualt send frequence 10HZ
  // parameter: gyrotype(GryoType),gyrox(degree/s units), gyroy(degree/s
  // units),gyroz(degree/s units)
  IMPL_CALLBACK_MEMBER_FUNC_FIVE(car_gyroscope, cmd_, int, gyrotype, double,
                                 gyrox, double, gyroy, double, gyroz, uint64_t,
                                 timestamp)

  // HU->MD
  // defualt send frequence 10HZ
  // parameter: accx(meter/s units), accy(meter/s units), accz(meter/s units),
  IMPL_CALLBACK_MEMBER_FUNC_FOUR(car_acceleration, cmd_, double, accx, double,
                                 accy, double, accz, uint64_t, timestamp)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_THREE(car_oil, cmd_, int, level, int, range, bool,
                                  lowfulewarning)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_NONE(launch_home, cmd_)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_NONE(launch_phone, cmd_)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_NONE(launch_map, cmd_)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_NONE(launch_music, cmd_)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_NONE(go_to_foreground, cmd_)

  // HU->MD
  // parameter: GearState
  IMPL_CALLBACK_MEMBER_FUNC_ONE(gear_info, cmd_, int, gear)

  // HU->MD
  // parameter: [moduleid, flag, frequency]
  IMPL_CALLBACK_MEMBER_FUNC_ONE(car_data_subscribe_done, cmd_,
                                const vehicle_info_vec_t &, infos)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_ONE(carlife_data_subscribe, cmd_,
                                const mobile_info_vec_t &, infos)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_ONE(carlife_data_subscribe_start, cmd_,
                                const mobile_info_vec_t &, infos)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_ONE(carlife_data_subscribe_stop, cmd_,
                                const mobile_info_vec_t &, infos)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_THREE(bt_hfp_response, cmd_, int, status, int, cmd,
                                  int, dtmfcode)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_TWO(bt_hfp_status_response, cmd_, int, status, int,
                                type)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_NONE(hu_feature_config_response, cmd_)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_ONE(bt_start_identify_req, cmd_,
                                const std::string &, address)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_ONE(error_code, cmd_, const std::string &,
                                errorcode)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_NONE(video_encoder_jpeg, cmd_)

public: //! media/navi/vr control cmd // MD->HU
  // MD->HU
  // Deprecation, replace by vr_status
  // parameter: [module_id, status_id]
  IMPL_MEMBER_FUNC_ONE(module_status, cmd_, const status_vec_t &, status);

public: //! media/navi/vr control cmd // HU->MD
  // HU->MD
  // paramter: module id, status id
  IMPL_CALLBACK_MEMBER_FUNC_TWO(module_control, cmd_, int, moduleid, int,
                                statusid);

public: //! verification cmd // MD->HU
  // MD->HU
  IMPL_MEMBER_FUNC_ONE(md_authen_response, cmd_, const std::string &,
                       encryptvalue);

  // MD->HU
  IMPL_MEMBER_FUNC_ONE(md_authen_result, cmd_, bool, authenresult);

public: //! verification cmd // HU->MD
  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_ONE(hu_authen_request, cmd_, const std::string &,
                                randomvalue);

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_ONE(hu_authen_result, cmd_, bool, authenresult);

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_EIGHT(statistic_info, cmd_, const std::string &,
                                  cuid, const std::string &, versionname, int,
                                  versioncode, const std::string &, channel,
                                  int, connectcount, int, connectsuccesscount,
                                  int, connecttime, const std::string &,
                                  crashlog);

public: //! bluetooth cmd // MD->HU
  // MD->HU
  IMPL_MEMBER_FUNC_THREE(bt_hfp_request, cmd_, int, command,
                         const std::string &, phonenum, int, dtmfcode);

public: //! bluetooth cmd // HU->MD
  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_FOUR(bt_hfp_indication, cmd_, int, state,
                                 const std::string &, phonenum,
                                 const std::string &, phonename,
                                 const std::string &, address);

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_THREE(bt_hfp_connection, cmd_, int, state,
                                  const std::string &, address,
                                  const std::string &, name);

public: //! Vidoe cmd // MD->HU
  // MD->HU
  // paramter: width pixel, height pixel, rate frame/second
  IMPL_MEMBER_FUNC_THREE(encoder_init_done, cmd_, int, width, int, height, int,
                         framerate);

  // MD->HU
  // paramter: rate frame/second
  IMPL_MEMBER_FUNC_ONE(encoder_frame_rate_change_done, cmd_, int, framerate);

public: //! Vidoe cmd // HU->MD
  // HU->MD
  // paramter: width pixel, height pixel, rate frame/second
  IMPL_CALLBACK_MEMBER_FUNC_THREE(encoder_init, cmd_, int, width, int, height,
                                  int, framerate);

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_NONE(encoder_start, cmd_);

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_NONE(encoder_pause, cmd_);

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_NONE(encoder_reset, cmd_);

  // HU->MD
  // paramter: rate frame/second
  IMPL_CALLBACK_MEMBER_FUNC_ONE(encoder_frame_rate_change, cmd_, int,
                                framerate);

public: //! VR cmd // MD->HU
  // MD->HU
  IMPL_MEMBER_FUNC_NONE(mic_record_wakeup_start, cmd_);

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(mic_record_end, cmd_);

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(mic_record_recog_start, cmd_);

public: // control
  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_THREE(action, ctrl_, int, action, int, x, int, y)

  // HU->MD
  // parameter: x, y
  IMPL_CALLBACK_MEMBER_FUNC_TWO(action_down, ctrl_, int, x, int, y)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_TWO(action_up, ctrl_, int, x, int, y)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_TWO(action_move, ctrl_, int, x, int, y)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_TWO(single_click, ctrl_, int, x, int, y)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_TWO(double_click, ctrl_, int, x, int, y)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_TWO(long_press, ctrl_, int, x, int, y)

  // HU->MD
  IMPL_CALLBACK_MEMBER_FUNC_ONE(car_hard_key_code, ctrl_, int, keycode)

public: // media
  // MD->HU
  // paramter: rate hz, channel pcs, format bit
  IMPL_MEMBER_FUNC_THREE(media_init, media_, int, samplerate, int,
                         channelconfig, int, sampleformat)
  // MD->HU
  IMPL_MEMBER_FUNC_NONE(media_pause, media_)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(media_play, media_)

  // MD->HU
  // paramter: timestamp, data, data_size
  IMPL_MEMBER_FUNC_THREE(media_data, media_, uint64_t, timestamp, const char *,
                         media, size_t, media_size)

public: // tts
  // MD->HU
  // paramter: rate hz, channel pcs, format bit
  IMPL_MEMBER_FUNC_THREE(tts_init, tts_, int, samplerate, int, channelconfig,
                         int, sampleformat)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(tts_stop, tts_)

  // MD->HU
  // paramter: timestamp, data, data_size
  IMPL_MEMBER_FUNC_THREE(tts_data, tts_, uint64_t, timestamp, const char *, tts,
                         size_t, tts_size)

public: // video
  // MD->HU
  // paramter: timestamp, data, data_size
  IMPL_MEMBER_FUNC_THREE(video_data, video_, uint64_t, timestamp, const char *,
                         video, size_t, video_size)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(heartbeat, video_)

public: // vr // MD->HU
  // MD->HU
  // paramter: rate hz, channel pcs, format bit
  IMPL_MEMBER_FUNC_THREE(vr_init, vr_, int, samplerate, int, channelconfig, int,
                         sampleformat)

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(vr_stop, vr_)

  // MD->HU
  // parameter: [module_id, status_id]
  IMPL_MEMBER_FUNC_ONE(vr_status, vr_, const status_vec_t &, status);

  // MD->HU
  IMPL_MEMBER_FUNC_NONE(vr_interrupt, vr_);

public: // vr // HU->MD
  // HU->MD
  // paramter: timestamp, data, data_size
  IMPL_CALLBACK_MEMBER_FUNC_THREE(vr_data, vr_, uint64_t, timestamp,
                                  const char *, tts, size_t, tts_size);

  // HU->MD
  // paramter: timestamp, data, data_size
  IMPL_CALLBACK_MEMBER_FUNC_THREE(mic_data, vr_, uint64_t, timestamp,
                                  const char *, tts, size_t, tts_size);

public:
  void register_send(EChannelType channel,
                     const std::function<int(const char *, size_t)> &f);

  void recv_data(EChannelType channel, const char *data, size_t data_size);

private:
  CmdChannel cmd_;
  CtrlChannel ctrl_;
  MediaChannel media_;
  TTSChannel tts_;
  VideoChannel video_;
  VRChannel vr_;

}; // class CarlifeMobileDevice

#endif // CARLIFE_CARLIFEMOBILEDEVICE_H
