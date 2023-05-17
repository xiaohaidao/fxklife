
#ifndef CARLIFE_CHANNEL_CMDCHANNEL_H
#define CARLIFE_CHANNEL_CMDCHANNEL_H

#include <string>
#include <tuple>

#include "carlife/channel/ChannelBase.h"

// parameter: [moduleid, supportflag, frequency]
typedef std::vector<std::tuple<int, bool, int>> mobile_info_vec_t;
// parameter: [moduleid, flag, frequency]
typedef std::vector<std::tuple<int, int, int>> vehicle_info_vec_t;
// parameter: [module_id, status_id]
typedef std::vector<std::pair<int, int>> status_vec_t;

/**
```mermaid
sequenceDiagram
  Note left of HU: connected
  HU->>MD: MSG_CMD_HU_PROTOCAL_VERSION
  MD->>HU: MSG_CMD_PROTOCAL_VERSION_MATCH_STATUS

  Note left of HU: protocal match
  HU-->>MD: MSG_CMD_HU_AUTHEN_REQUEST
  MD-->>HU: MSG_CMD_MD_AUTHEN_RESPONSE
  HU-->>MD: MSG_CMD_HU_AUTHEN_RESULT
  HU->>MD: MSG_CMD_STATISTIC_INFO
  MD->>HU: MSG_CMD_MD_AUTHEN_RESULT

  Note left of HU: verification passed
  HU->>MD: MSG_CMD_HU_INFO
  MD->>HU: MSG_CMD_MD_FEATURE_CONFIG_REQUEST
  HU-->>MD: MSG_CMD_MD_FEATURE_CONFIG_RESPONSE
  HU->>MD: MSG_CMD_VIDEO_ENCODER_INIT

  Note left of HU: Support Bt<br>Auto Pair
  HU->>MD: MSG_CMD_HU_BT_OOB_INFO

  Note right of MD: protocal match
  MD->>HU: MSG_CMD_MD_INFO
  MD->>HU: MSG_CMD_MODULE_STATUS

  Note right of MD: music playing
  MD->>HU: MSG_CMD_MEDIA_INFO

  Note right of MD: carlife foreground
  MD->>HU: MSG_CMD_FOREGROUND

  Note right of MD: carlife background
  MD->>HU: MSG_CMD_BACKGROUND

  Note right of MD: get video encoder<br>init message
  MD->>HU: MSG_CMD_VIDEO_ENCODER_INIT_DONE
  HU->>MD: MSG_CMD_VIDEO_ENCODER_START

  Note right of MD: begin to send video <br> data in video channel
  MD->>HU: MSG_VIDEO_DATA
```

```mermaid
sequenceDiagram
  Note left of HU: stop Navi
  HU->>MD: MSG_CMD_MODULE_CONTROL(parameter1)
  MD-->>HU: MSG_CMD_MODULE_STATUS(parameter1)

  Note left of HU: stop VR
  HU->>MD: MSG_CMD_MODULE_CONTROL(parameter2)
  MD-->>HU: MSG_CMD_MODULE_STATUS(parameter2)

  Note left of HU: pause Media
  HU->>MD: MSG_CMD_MODULE_CONTROL(parameter3)
  MD-->>HU: MSG_CMD_MODULE_STATUS(parameter3)

  Note left of HU: resume Media
  HU->>MD: MSG_CMD_MODULE_CONTROL(parameter4)
  MD-->>HU: MSG_CMD_MODULE_STATUS(parameter4)
```

*/
class CmdChannel : public ChannelBase {
public:
  // HU->MD
  DECLARE_MEMBER_FUNCS(version, int, int);

  // MD->HU
  DECLARE_MEMBER_FUNCS(match_ver, bool);

  // HU->MD
  DECLARE_MEMBER_FUNCS(hu_info, const std::string &, const std::string &,
                       const std::string &, const std::string &,
                       const std::string &, const std::string &);

  // MD->HU
  // parameter: os, board, bootloader, brand, cpu_abi,
  // cpu_abi2, device, display, fingerprint, hardware, host, cid,
  // manufacturer, model, product, serial, codename, incremental, release,
  // sdk, sdk_int, token, btaddress, btname
  DECLARE_MEMBER_FUNCS(
      md_info, const std::string &, const std::string &, const std::string &,
      const std::string &, const std::string &, const std::string &,
      const std::string &, const std::string &, const std::string &,
      const std::string &, const std::string &, const std::string &,
      const std::string &, const std::string &, const std::string &,
      const std::string &, const std::string &, const std::string &,
      const std::string &, const std::string &, int, const std::string &,
      const std::string &, const std::string &);

  // HU->MD
  DECLARE_MEMBER_FUNCS(car_velocity, int, uint64_t);

  // HU->MD
  // defualt send frequency 1HZ
  // parameter: antennastate(dBHz), latitude(1/1,000,000 degrees),
  // longitude(1/1,000,000 degrees), height(0.1 meters), speed(0.01 km/h units),
  // heading(0.1 degrees), pdop(units	0.1), satsused, satsvisible, timestamp
  DECLARE_MEMBER_FUNCS(car_gps, uint32_t, int, int, int, uint32_t, uint32_t,
                       uint32_t, uint32_t, uint32_t, uint64_t);

  // HU->MD
  // defualt send frequence 10HZ
  // parameter: gyrotype(GryoType),gyrox(degree/s units), gyroy(degree/s
  // units),gyroz(degree/s units)
  DECLARE_MEMBER_FUNCS(car_gyroscope, int, double, double, double, uint64_t);

  // HU->MD
  // defualt send frequence 10HZ
  // parameter: accx(meter/s units), accy(meter/s units), accz(meter/s units),
  DECLARE_MEMBER_FUNCS(car_acceleration, double, double, double, uint64_t);

  // HU->MD
  DECLARE_MEMBER_FUNCS(car_oil, int, int, bool);

  // MD->HU
  DECLARE_MEMBER_FUNCS(screen_on, );

  // MD->HU
  DECLARE_MEMBER_FUNCS(screen_off, );

  // MD->HU
  DECLARE_MEMBER_FUNCS(screen_userpresent, );

  // MD->HU
  DECLARE_MEMBER_FUNCS(foreground, );

  // MD->HU
  DECLARE_MEMBER_FUNCS(background, );

  // HU->MD
  DECLARE_MEMBER_FUNCS(launch_home, );

  // HU->MD
  DECLARE_MEMBER_FUNCS(launch_phone, );

  // HU->MD
  DECLARE_MEMBER_FUNCS(launch_map, );

  // HU->MD
  DECLARE_MEMBER_FUNCS(launch_music, );

  // MD->HU
  DECLARE_MEMBER_FUNCS(launch_exit, );

  // HU->MD
  DECLARE_MEMBER_FUNCS(go_to_foreground, );

  enum GearState {
    GEAR_NEUTRAL = 1,
    GEAR_PARK = 2,
    GEAR_DRIVE = 3,
    GEAR_LOW = 4,
    GEAR_REVERSE = 5,
  };
  // HU->MD
  // parameter: GearState
  DECLARE_MEMBER_FUNCS(gear_info, int);

  // MD->HU
  // parameter: ActionType, TurnKind
  DECLARE_MEMBER_FUNCS(navi_next_turn_info, int, int, const std::string &, int,
                       int, int);

  // MD->HU
  DECLARE_MEMBER_FUNCS(car_data_subscribe, );

  enum ModuleID {
    CAR_DATA_GPS = 0,
    CAR_DATA_VELOCITY = 1,
    CAR_DATA_GYROSCOPE = 2,
    CAR_DATA_ACCELERATION = 3,
    CAR_DATA_GEAR = 4,
    CAR_DATA_OIL = 5,
  };
  // HU->MD
  // parameter: [moduleid, flag, frequency]
  DECLARE_MEMBER_FUNCS(car_data_subscribe_done, const vehicle_info_vec_t &);

  // MD->HU
  // parameter: moduleid, flag, frequency
  DECLARE_MEMBER_FUNCS(car_data_subscribe_start, int, int, int);

  // MD->HU
  // parameter: moduleid, flag, frequency
  DECLARE_MEMBER_FUNCS(car_data_subscribe_stop, int, int, int);

  enum CarlifeMediaPlayMode {
    PLAYMODE_REPEATE_ONE = 0,
    PLAYMODE_SHUFFLE = 1,
    PLAYMODE_REPEATE_ALL = 2,
  };
  // MD->HU
  DECLARE_MEMBER_FUNCS(media_info, const std::string &, const std::string &,
                       const std::string &, const std::string &,
                       const std::string &, int, int, const std::string &, int);

  // MD->HU
  // parameter: units ms
  DECLARE_MEMBER_FUNCS(media_progress_bar, int);

  // MD->HU
  DECLARE_MEMBER_FUNCS(connect_exception, int);

  // MD->HU
  DECLARE_MEMBER_FUNCS(request_go_to_foreground, );

  // MD->HU
  DECLARE_MEMBER_FUNCS(ui_action_sound, );

  // HU->MD
  DECLARE_MEMBER_FUNCS(carlife_data_subscribe, const mobile_info_vec_t &);

  // MD->HU
  DECLARE_MEMBER_FUNCS(carlife_data_subscribe_done, const mobile_info_vec_t &);

  // HU->MD
  DECLARE_MEMBER_FUNCS(carlife_data_subscribe_start, const mobile_info_vec_t &);

  // HU->MD
  DECLARE_MEMBER_FUNCS(carlife_data_subscribe_stop, const mobile_info_vec_t &);

  // MD->HU
  DECLARE_MEMBER_FUNCS(navi_assitantguide_info, int, int, int, int, int, int);

  // MD->HU
  DECLARE_MEMBER_FUNCS(go_to_foreground_response, );

  // MD->HU
  DECLARE_MEMBER_FUNCS(start_bt_autopair_request, int, const std::string &);

  // HU->MD
  DECLARE_MEMBER_FUNCS(bt_hfp_response, int, int, int);

  // MD->HU
  DECLARE_MEMBER_FUNCS(bt_hfp_status_request, int);

  // HU->MD
  DECLARE_MEMBER_FUNCS(bt_hfp_status_response, int, int);

  // MD->HU
  DECLARE_MEMBER_FUNCS(md_feature_config_request, );

  // HU->MD
  DECLARE_MEMBER_FUNCS(hu_feature_config_response, );

  // HU->MD
  DECLARE_MEMBER_FUNCS(bt_start_identify_req, const std::string &);

  // MD->HU
  DECLARE_MEMBER_FUNCS(bt_identify_result_ind, int, const std::string &);

  // HU->MD
  DECLARE_MEMBER_FUNCS(error_code, const std::string &);

  // HU->MD
  DECLARE_MEMBER_FUNCS(video_encoder_jpeg, );

  // MD->HU
  DECLARE_MEMBER_FUNCS(video_encoder_jpeg_ack, );

  // MD->HU
  DECLARE_MEMBER_FUNCS(md_exit, );

public: //! media/navi/vr cmd
  // MD->HU
  // Deprecation, replace by vr_status
  // parameter: [module_id, status_id]
  DECLARE_MEMBER_FUNCS(module_status, const status_vec_t &);

  // HU->MD
  // paramter: module id, status id
  DECLARE_MEMBER_FUNCS(module_control, int, int);

public: //! bluetooth cmd
  enum RequestType {
    REQUEST_STARTCALL = 1,
    REQUEST_TERMINATECALL = 2,
    REQUEST_ANSWERCALL = 3,
    REQUEST_REJECTCALL = 4,
    REQUEST_DTMFCODE = 5,
    REQUEST_MUTEMIC = 6
  };
  // MD->HU
  DECLARE_MEMBER_FUNCS(bt_hfp_request, int, const std::string &, int);

  enum IndicationType {
    INDICATION_NEWCALL = 1,
    INDICATION_OUTCALL = 2,
    INDICATION_CALLACTIVE = 3,
    INDICATION_NOCALLACTIVE = 4,
    INDICATION_MULTICALLACTIVE = 5,
    INDICATION_MULTICALLINACTIVE = 6
  };
  // HU->MD
  DECLARE_MEMBER_FUNCS(bt_hfp_indication, int, const std::string &,
                       const std::string &, const std::string &);

  enum ConnectionState {
    HFP_DISCONNECTED = 0,
    HFP_CONNECTING = 1,
    HFP_CONNECTED = 2
  };
  // HU->MD
  DECLARE_MEMBER_FUNCS(bt_hfp_connection, int, const std::string &,
                       const std::string &);

public: //! verification cmd
  // HU->MD
  DECLARE_MEMBER_FUNCS(hu_authen_request, const std::string &);

  // MD->HU
  DECLARE_MEMBER_FUNCS(md_authen_response, const std::string &);

  // HU->MD
  DECLARE_MEMBER_FUNCS(hu_authen_result, bool);

  // MD->HU
  DECLARE_MEMBER_FUNCS(md_authen_result, bool);

  // HU->MD
  DECLARE_MEMBER_FUNCS(statistic_info, const std::string &, const std::string &,
                       int, const std::string &, int, int, int,
                       const std::string &);

public: //! Vidoe cmd
  // HU->MD
  // paramter: width pixel, height pixel, rate frame/second
  DECLARE_MEMBER_FUNCS(encoder_init, int, int, int);
  // MD->HU
  // paramter: width pixel, height pixel, rate frame/second
  DECLARE_MEMBER_FUNCS(encoder_init_done, int, int, int);
  // HU->MD
  DECLARE_MEMBER_FUNCS(encoder_start, );
  // HU->MD
  DECLARE_MEMBER_FUNCS(encoder_pause, );
  // HU->MD
  DECLARE_MEMBER_FUNCS(encoder_reset, );
  // HU->MD
  // paramter: rate frame/second
  DECLARE_MEMBER_FUNCS(encoder_frame_rate_change, int);
  // MD->HU
  // paramter: rate frame/second
  DECLARE_MEMBER_FUNCS(encoder_frame_rate_change_done, int);

public: //! VR cmd
  // MD->HU
  DECLARE_MEMBER_FUNCS(mic_record_wakeup_start, );

  // MD->HU
  DECLARE_MEMBER_FUNCS(mic_record_end, );

  // MD->HU
  DECLARE_MEMBER_FUNCS(mic_record_recog_start, );

public:
  void recv_data(const char *data, size_t data_size) override;

private:
  void parse_message(const char *data, size_t data_size);

  const CmdMessageHeader &transfer_from_head(const char *data);
  const char *transfer_head(CmdMessageHeader &header);

  char buff_[CMD_DATA_SIZE];
  size_t buff_size_;

}; // class CmdChannel

#endif // CARLIFE_CHANNEL_CMDCHANNEL_H
