
#include "carlife/channel/CmdChannel.h"

#include "CarlifeAccelerationProto.pb.h"
#include "CarlifeAuthenRequestProto.pb.h"
#include "CarlifeAuthenResponseProto.pb.h"
#include "CarlifeAuthenResultProto.pb.h"
#include "CarlifeBTHfpConnectionProto.pb.h"
#include "CarlifeBTHfpIndicationProto.pb.h"
#include "CarlifeBTHfpRequestProto.pb.h"
#include "CarlifeBTHfpResponseProto.pb.h"
#include "CarlifeBTHfpStatusRequestProto.pb.h"
#include "CarlifeBTHfpStatusResponseProto.pb.h"
#include "CarlifeBTIdentifyResultIndProto.pb.h"
#include "CarlifeBTStartIdentifyReqProto.pb.h"
#include "CarlifeBTStartPairReqProto.pb.h"
#include "CarlifeCarGpsProto.pb.h"
#include "CarlifeCarSpeedProto.pb.h"
#include "CarlifeConnectExceptionProto.pb.h"
#include "CarlifeDeviceInfoProto.pb.h"
#include "CarlifeErrorCodeProto.pb.h"
#include "CarlifeGearInfoProto.pb.h"
#include "CarlifeGyroscopeProto.pb.h"
#include "CarlifeMediaInfoProto.pb.h"
#include "CarlifeMediaProgressBarProto.pb.h"
#include "CarlifeModuleStatusListProto.pb.h"
#include "CarlifeModuleStatusProto.pb.h"
#include "CarlifeNaviAssitantGuideInfoProto.pb.h"
#include "CarlifeNaviNextTurnInfoProto.pb.h"
#include "CarlifeOilProto.pb.h"
#include "CarlifeProtocolVersionMatchStatusProto.pb.h"
#include "CarlifeProtocolVersionProto.pb.h"
#include "CarlifeStatisticsInfoProto.pb.h"
#include "CarlifeSubscribeMobileCarLifeInfoListProto.pb.h"
#include "CarlifeVehicleInfoListProto.pb.h"
#include "CarlifeVehicleInfoProto.pb.h"
#include "CarlifeVideoEncoderInfoProto.pb.h"
#include "CarlifeVideoFrameRateProto.pb.h"
#include "utils/log.h"

using namespace com::baidu::carlife::protobuf;

// Protocal Version Match enum
enum ProtocalVersionMatch {
  VERSION_MATCH = 1,
  VERSION_NOT_MATCH = 2,
};

// MSG_CMD_HU_BT_OOB_INFO;CarlifeBTOOBInfo;HU->MD
// MSG_CMD_MD_BT_OOB_INFO;CarlifeBTOOBInfo;MD->HU
// MSG_CMD_HU_CONNECT_STATISTIC;CarlifeConStatisticProto;HU->MD

// MSG_CMD_SCREEN_ON;;MD->HU
// MSG_CMD_SCREEN_OFF;;MD->HU
// MSG_CMD_SCREEN_USERPRESENT;;MD->HU
// MSG_CMD_BT_HFP_CALL_STATUS_COVER;CarlifeCallStatusCover;MD->HU

#define CARLIFE_DATA_SUBSCRIBE_LIST(func)                                      \
  CarlifeSubscribeMobileCarLifeInfoList data;                                  \
  data.ParseFromArray(message, message_size);                                  \
                                                                               \
  if (func) {                                                                  \
    /* status_vec_t result(data.cnt()); */                                     \
    mobile_info_vec_t result(data.subscribemobilecarlifeinfo_size());          \
    /* assert(result == data.cnt()); */                                        \
    for (size_t i = 0; i < result.size(); ++i) {                               \
      const CarlifeSubscribeMobileCarLifeInfo &status =                        \
          data.subscribemobilecarlifeinfo(i);                                  \
      result[i] = std::make_tuple(status.moduleid(), status.supportflag(),     \
                                  status.frequency());                         \
    }                                                                          \
    func(result);                                                              \
  }

#define CARLIFE_DATA_SUBSCRIBE_LIST_IMPL(name, cmd_type)                       \
  void CmdChannel::send_##name(const mobile_info_vec_t &infos) {               \
    CarlifeSubscribeMobileCarLifeInfoList data;                                \
    data.set_cnt(infos.size());                                                \
    for (auto const &i : infos) {                                              \
      CarlifeSubscribeMobileCarLifeInfo *s =                                   \
          data.add_subscribemobilecarlifeinfo();                               \
      s->set_moduleid(std::get<0>(i));                                         \
      s->set_supportflag(std::get<1>(i));                                      \
      s->set_frequency(std::get<2>(i));                                        \
    }                                                                          \
                                                                               \
    DECLARE_SND_MSG(CmdMessageHeader, cmd_type)                                \
  }

RECV_DATA_IMPL(CmdChannel, CmdMessageHeader)

void CmdChannel::parse_message(const char *data, size_t data_size) {
  PARSE_MESSAGE_HEAD_IMPL(CmdMessageHeader)

  switch (header.service_type) {
  case MSG_CMD_HU_PROTOCOL_VERSION: { // HU->MD
    DECLARE_CALLBACK_IMPL_TWO(version, CarlifeProtocolVersion, majorversion,
                              minorversion)
    break;
  }
  case MSG_CMD_PROTOCOL_VERSION_MATCH_STATUS: { // MD->HU
    CarlifeProtocolVersionMatchStatus data;
    data.ParseFromArray(message, message_size);

    if (m_callback_match_ver) {
      m_callback_match_ver(data.matchstatus() == VERSION_MATCH);
    }
    break;
  }
  case MSG_CMD_HU_INFO: { // HU->MD
    DECLARE_CALLBACK_IMPL_SIX(hu_info, CarlifeDeviceInfo, os, manufacturer,
                              model, release, btaddress, btname);
    break;
  }
  case MSG_CMD_MD_INFO: { // MD->HU
    DECLARE_CALLBACK_IMPL_TWENTY_FOUR(
        md_info, CarlifeDeviceInfo, os, board, bootloader, brand, cpu_abi,
        cpu_abi2, device, display, fingerprint, hardware, host, cid,
        manufacturer, model, product, serial, codename, incremental, release,
        sdk, sdk_int, token, btaddress, btname);

    break;
  }
  case MSG_CMD_CAR_VELOCITY: { // HU->MD
    DECLARE_CALLBACK_IMPL_TWO(car_velocity, CarlifeCarSpeed, speed, timestamp)
    break;
  }
  case MSG_CMD_CAR_GPS: { // HU->MD
    DECLARE_CALLBACK_IMPL_TEN(car_gps, CarlifeCarGps, antennastate, latitude,
                              longitude, height, speed, heading, pdop, satsused,
                              satsvisible, timestamp)

    break;
  }
  case MSG_CMD_CAR_GYROSCOPE: { // HU->MD
    enum GyroType {
      SINGLE_AXIS_GYRO = 1,
      THREE_AXIS_GYRO = 2,
    };
    DECLARE_CALLBACK_IMPL_FIVE(car_gyroscope, CarlifeGyroscope, gyrotype, gyrox,
                               gyroy, gyroz, timestamp)
    break;
  }
  case MSG_CMD_CAR_ACCELERATION: { // HU->MD
    DECLARE_CALLBACK_IMPL_FOUR(car_acceleration, CarlifeAcceleration, accx,
                               accy, accz, timestamp)
    break;
  }
  case MSG_CMD_CAR_OIL: { // HU->MD
    DECLARE_CALLBACK_IMPL_THREE(car_oil, CarlifeOil, level, range,
                                lowfulewarning)
    break;
  }
  case MSG_CMD_SCREEN_ON: { // MD->HU
    LOG_WARN("Not support now");
    break;
  }
  case MSG_CMD_SCREEN_OFF: { // MD->HU
    LOG_WARN("Not support now");
    break;
  }
  case MSG_CMD_SCREEN_USERPRESENT: { // MD->HU
    LOG_WARN("Not support now");
    break;
  }
  case MSG_CMD_FOREGROUND: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(foreground)
    break;
  }
  case MSG_CMD_BACKGROUND: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(background)
    break;
  }
  case MSG_CMD_LAUNCH_MODE_NORMAL: { // HU->MD
    DECLARE_CALLBACK_IMPL_NONE(launch_home)
    break;
  }
  case MSG_CMD_LAUNCH_MODE_PHONE: { // HU->MD
    DECLARE_CALLBACK_IMPL_NONE(launch_phone)
    break;
  }
  case MSG_CMD_LAUNCH_MODE_MAP: { // HU->MD
    DECLARE_CALLBACK_IMPL_NONE(launch_map)
    break;
  }
  case MSG_CMD_LAUNCH_MODE_MUSIC: { // HU->MD
    DECLARE_CALLBACK_IMPL_NONE(launch_music)
    break;
  }
  case MSG_CMD_GO_TO_DESKTOP: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(launch_exit)
    break;
  }
  case MSG_CMD_GO_TO_FOREGROUND: { // HU->MD
    DECLARE_CALLBACK_IMPL_NONE(go_to_foreground)
    break;
  }
  case MSG_CMD_GEAR_INFO: { // HU->MD
    DECLARE_CALLBACK_IMPL_ONE(gear_info, CarlifeGearInfo, gear)
    break;
  }
  case MSG_CMD_NAVI_NEXT_TURN_INFO: { // MD->HU
    DECLARE_CALLBACK_IMPL_SIX(navi_next_turn_info, CarlifeNaviNextTurnInfo,
                              action, nextturn, roadname, totaldistance,
                              remaindistance, time /*sturnicondata*/)
    break;
  }
  case MSG_CMD_CAR_DATA_SUBSCRIBE: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(car_data_subscribe)
    break;
  }
  case MSG_CMD_CAR_DATA_SUBSCRIBE_DONE: { // HU->MD
    CarlifeVehicleInfoList data;
    data.ParseFromArray(message, message_size);

    if (m_callback_car_data_subscribe_done) {
      // vehicle_info_t result(data.cnt());
      vehicle_info_vec_t result(data.vehicleinfo_size());
      // assert(result == data.cnt());
      for (size_t i = 0; i < result.size(); ++i) {
        const CarlifeVehicleInfo &info = data.vehicleinfo(i);
        result[i] =
            std::make_tuple(info.moduleid(), info.flag(), info.frequency());
      }
      m_callback_car_data_subscribe_done(result);
    }
    break;
  }
  case MSG_CMD_CAR_DATA_SUBSCRIBE_START: { // MD->HU
    DECLARE_CALLBACK_IMPL_THREE(car_data_subscribe_start, CarlifeVehicleInfo,
                                moduleid, flag, frequency)
    break;
  }
  case MSG_CMD_CAR_DATA_SUBSCRIBE_STOP: { // MD->HU
    DECLARE_CALLBACK_IMPL_THREE(car_data_subscribe_stop, CarlifeVehicleInfo,
                                moduleid, flag, frequency)
    break;
  }
  case MSG_CMD_MEDIA_INFO: { // MD->HU
    DECLARE_CALLBACK_IMPL_NINE(media_info, CarlifeMediaInfo, source, song,
                               artist, album, albumart, duration, playlistnum,
                               songid, mode)
    break;
  }
  case MSG_CMD_MEDIA_PROGRESS_BAR: { // MD->HU
    DECLARE_CALLBACK_IMPL_ONE(media_progress_bar, CarlifeMediaProgressBar,
                              progressbar)
    break;
  }
  case MSG_CMD_CONNECT_EXCEPTION: { // MD->HU
    enum ExceptionType {
      TYPE_ENCORDER_ERROR = 1,
      TYPE_PERMISSION_DENIED = 2,
      TYPE_SCREENSHARE_REQUEST = 3,
    };
    DECLARE_CALLBACK_IMPL_ONE(connect_exception, CarlifeConnectException,
                              exceptiontype)
    break;
  }
  case MSG_CMD_REQUEST_GO_TO_FOREGROUND: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(request_go_to_foreground)
    break;
  }
  case MSG_CMD_UI_ACTION_SOUND: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(ui_action_sound)
    break;
  }
  // bluetooth phone {
  case MSG_CMD_BT_HFP_REQUEST: { // MD->HU
    DECLARE_CALLBACK_IMPL_THREE(bt_hfp_request, CarlifeBTHfpRequest, command,
                                phonenum, dtmfcode)
    break;
  }
  case MSG_CMD_BT_HFP_INDICATION: { // HU->MD
    DECLARE_CALLBACK_IMPL_FOUR(bt_hfp_indication, CarlifeBTHfpIndication, state,
                               phonenum, phonename, address)
    break;
  }
  case MSG_CMD_BT_HFP_CONNECTION: { // HU->MD
    DECLARE_CALLBACK_IMPL_THREE(bt_hfp_connection, CarlifeBTHfpConnection,
                                state, address, name)
    break;
  }
  // bluetooth phone }
  case MSG_CMD_CARLIFE_DATA_SUBSCRIBE: { // HU->MD
    enum ModuleID {
      CARLIFE_DATA_TURNBYTURN = 0,
      CARLFIE_DATA_ASSISTANTGUIDE = 1,
    };
    CARLIFE_DATA_SUBSCRIBE_LIST(m_callback_carlife_data_subscribe)
    break;
  }
  case MSG_CMD_CARLIFE_DATA_SUBSCRIBE_DONE: { // MD->HU
    CARLIFE_DATA_SUBSCRIBE_LIST(m_callback_carlife_data_subscribe_done)
    break;
  }
  case MSG_CMD_CARLIFE_DATA_SUBSCRIBE_START: { // HU->MD
    CARLIFE_DATA_SUBSCRIBE_LIST(m_callback_carlife_data_subscribe_start)
    break;
  }
  case MSG_CMD_CARLIFE_DATA_SUBSCRIBE_STOP: { // HU->MD
    CARLIFE_DATA_SUBSCRIBE_LIST(m_callback_carlife_data_subscribe_stop)
    break;
  }
  case MSG_CMD_NAVI_ASSITANTGUIDE_INFO: { // MD->HU
    DECLARE_CALLBACK_IMPL_SIX(navi_assitantguide_info,
                              CarlifeNaviAssitantGuideInfo, action,
                              assistanttype, trafficsigntype, totaldistance,
                              remaindistance, cameraspeed)

    break;
  }
  case MSG_CMD_GO_TO_FOREGROUND_RESPONSE: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(go_to_foreground_response)
    break;
  }
  case MSG_CMD_START_BT_AUTOPAIR_REQUEST: { // MD->HU
    DECLARE_CALLBACK_IMPL_TWO(start_bt_autopair_request, CarlifeBTStartPairReq,
                              ostype, address)
    break;
  }
  case MSG_CMD_BT_HFP_RESPONSE: { // HU->MD
    enum RequestType {
      REQUEST_STARTCALL = 1,
      REQUEST_TERMINATECALL = 2,
      REQUEST_ANSWERCALL = 3,
      REQUEST_REJECTCALL = 4,
      REQUEST_DTMFCODE = 5,
      REQUEST_MUTEMIC = 6
    };
    DECLARE_CALLBACK_IMPL_THREE(bt_hfp_response, CarlifeBTHfpResponse, status,
                                cmd, dtmfcode)
    break;
  }
  case MSG_CMD_BT_HFP_STATUS_REQUEST: { // MD->HU
    enum StatusType { STATUS_MIC = 1 };
    DECLARE_CALLBACK_IMPL_ONE(bt_hfp_status_request, CarlifeBTHfpStatusRequest,
                              type)
    break;
  }
  case MSG_CMD_BT_HFP_STATUS_RESPONSE: { // HU->MD
    DECLARE_CALLBACK_IMPL_TWO(bt_hfp_status_response,
                              CarlifeBTHfpStatusResponse, status, type)
    break;
  }
  case MSG_CMD_MD_FEATURE_CONFIG_REQUEST: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(md_feature_config_request)
    break;
  }
  case MSG_CMD_HU_FEATURE_CONFIG_RESPONSE: { // HU->MD
    DECLARE_CALLBACK_IMPL_NONE(hu_feature_config_response)
    break;
  }
  case MSG_CMD_BT_START_IDENTIFY_REQ: { // HU->MD
    DECLARE_CALLBACK_IMPL_ONE(bt_start_identify_req, CarlifeBTStartIdentifyReq,
                              address)
    break;
  }
  case MSG_CMD_BT_IDENTIFY_RESULT_IND: { // MD->HU
    DECLARE_CALLBACK_IMPL_TWO(bt_identify_result_ind,
                              CarlifeBTIdentifyResultInd, status, address)
    break;
  }
  case MSG_CMD_ERROR_CODE: { // HU->MD
    DECLARE_CALLBACK_IMPL_ONE(error_code, CarlifeErrorCode, errorcode)
    break;
  }
  case MSG_CMD_VIDEO_ENCODER_JPEG: { // HU->MD
    DECLARE_CALLBACK_IMPL_NONE(video_encoder_jpeg)
    break;
  }
  case MSG_CMD_VIDEO_ENCODER_JPEG_ACK: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(video_encoder_jpeg_ack)
    break;
  }
  case MSG_CMD_BT_HFP_CALL_STATUS_COVER: { // MD->HU
    // CarlifeCallStatusCover data;
    LOG_WARN("MSG_CMD_BT_HFP_CALL_STATUS_COVER not support now");
    break;
  }
  case MSG_CMD_MD_EXIT: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(md_exit)
    break;
  }
  /** not support
  case MSG_CMD_HU_BT_OOB_INFO: { // HU->MD
    CarlifeBTOOBInfo data;
    break;
  }
  case MSG_CMD_MD_BT_OOB_INFO: { // MD->HU
    CarlifeBTOOBInfo data;
    break;
  }
  case MSG_CMD_HU_CONNECT_STATISTIC: { // HU->MD
    CarlifeConStatisticProto data;
    break;
  }
  */

  // verification cmd {
  case MSG_CMD_HU_AUTHEN_REQUEST: { // HU->MD
    DECLARE_CALLBACK_IMPL_ONE(hu_authen_request, CarlifeAuthenRequest,
                              randomvalue)
    break;
  }
  case MSG_CMD_MD_AUTHEN_RESPONSE: { // MD->HU
    DECLARE_CALLBACK_IMPL_ONE(md_authen_response, CarlifeAuthenResponse,
                              encryptvalue)
    break;
  }
  case MSG_CMD_HU_AUTHEN_RESULT: { // HU->MD
    DECLARE_CALLBACK_IMPL_ONE(hu_authen_result, CarlifeAuthenResult,
                              authenresult)
    break;
  }
  case MSG_CMD_MD_AUTHEN_RESULT: { // MD->HU
    DECLARE_CALLBACK_IMPL_ONE(md_authen_result, CarlifeAuthenResult,
                              authenresult)
    break;
  }
  case MSG_CMD_STATISTIC_INFO: { // HU->MD
    DECLARE_CALLBACK_IMPL_EIGHT(statistic_info, CarlifeStatisticsInfo, cuid,
                                versionname, versioncode, channel, connectcount,
                                connectsuccesscount, connecttime, crashlog)
    break;
  }
  // verification cmd }

  // Video control cmd {
  case MSG_CMD_VIDEO_ENCODER_INIT: { // HU->MD
    DECLARE_CALLBACK_IMPL_THREE(encoder_init, CarlifeVideoEncoderInfo, width,
                                height, framerate)
    break;
  }
  case MSG_CMD_VIDEO_ENCODER_INIT_DONE: { // MD->HU
    DECLARE_CALLBACK_IMPL_THREE(encoder_init_done, CarlifeVideoEncoderInfo,
                                width, height, framerate)
    break;
  }
  case MSG_CMD_VIDEO_ENCODER_START: { // HU->MD
    DECLARE_CALLBACK_IMPL_NONE(encoder_start)
    break;
  }
  case MSG_CMD_VIDEO_ENCODER_PAUSE: { // HU->MD
    DECLARE_CALLBACK_IMPL_NONE(encoder_pause)
    break;
  }
  case MSG_CMD_VIDEO_ENCODER_FRAME_RATE_CHANGE: { // HU->MD
    DECLARE_CALLBACK_IMPL_ONE(encoder_frame_rate_change, CarlifeVideoFrameRate,
                              framerate)
    break;
  }
  case MSG_CMD_VIDEO_ENCODER_FRAME_RATE_CHANGE_DONE: { // MD->HU
    DECLARE_CALLBACK_IMPL_ONE(encoder_frame_rate_change_done,
                              CarlifeVideoFrameRate, framerate)
    break;
  }
  case MSG_CMD_VIDEO_ENCODER_RESET: { // HU->MD
    DECLARE_CALLBACK_IMPL_NONE(encoder_reset)
    break;
  }
  // Video control cmd }

  // VR control cmd {
  case MSG_CMD_MIC_RECORD_WAKEUP_START: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(mic_record_wakeup_start)
    break;
  }
  case MSG_CMD_MIC_RECORD_END: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(mic_record_end)
    break;
  }
  case MSG_CMD_MIC_RECORD_RECOG_START: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(mic_record_recog_start)
    break;
  }
  case MSG_CMD_MODULE_STATUS: { // MD->HU
    CarlifeModuleStatusList data;
    data.ParseFromArray(message, message_size);

    if (m_callback_mic_status) {
      // status_vec_t result(data.cnt());
      status_vec_t result(data.modulestatus_size());
      // assert(result == data.cnt());
      for (size_t i = 0; i < result.size(); ++i) {
        const CarlifeModuleStatus &status = data.modulestatus(i);
        result[i] = std::make_pair(status.moduleid(), status.statusid());
      }
      m_callback_mic_status(result);
    }
    break;
  }
  case MSG_CMD_MODULE_CONTROL: { // HU->MD
    DECLARE_CALLBACK_IMPL_TWO(mic_control, CarlifeModuleStatus, moduleid,
                              statusid)
    break;
  }
    // VR control cmd }

  default:
    LOG_ERROR("undefine service type!");
    break;
  }
}

// HU->MD
DECLARE_MEMBER_IMPL_TWO(CmdChannel, version, CmdMessageHeader,
                        MSG_CMD_HU_PROTOCOL_VERSION, CarlifeProtocolVersion,
                        int, majorversion, int, minorversion)

// MD->HU
void CmdChannel::send_match_ver(bool match) {
  // set data
  CarlifeProtocolVersionMatchStatus data;
  data.set_matchstatus(match ? VERSION_MATCH : VERSION_NOT_MATCH);

  DECLARE_SND_MSG(CmdMessageHeader, MSG_CMD_PROTOCOL_VERSION_MATCH_STATUS);
}

// HU->MD
DECLARE_MEMBER_IMPL_SIX(CmdChannel, hu_info, CmdMessageHeader, MSG_CMD_HU_INFO,
                        CarlifeDeviceInfo, const std::string &, os,
                        const std::string &, manufacturer, const std::string &,
                        model, const std::string &, release,
                        const std::string &, btaddress, const std::string &,
                        btname)

// MD->HU
DECLARE_MEMBER_IMPL_TWENTY_FOUR(
    CmdChannel, md_info, CmdMessageHeader, MSG_CMD_MD_INFO, CarlifeDeviceInfo,
    const std::string &, os, const std::string &, board, const std::string &,
    bootloader, const std::string &, brand, const std::string &, cpu_abi,
    const std::string &, cpu_abi2, const std::string &, device,
    const std::string &, display, const std::string &, fingerprint,
    const std::string &, hardware, const std::string &, host,
    const std::string &, cid, const std::string &, manufacturer,
    const std::string &, model, const std::string &, product,
    const std::string &, serial, const std::string &, codename,
    const std::string &, incremental, const std::string &, release,
    const std::string &, sdk, int, sdk_int, const std::string &, token,
    const std::string &, btaddress, const std::string &, btname)

// HU->MD
DECLARE_MEMBER_IMPL_TWO(CmdChannel, car_velocity, CmdMessageHeader,
                        MSG_CMD_CAR_VELOCITY, CarlifeCarSpeed, int, speed,
                        uint64_t, timestamp)

// HU->MD
DECLARE_MEMBER_IMPL_TEN(CmdChannel, car_gps, CmdMessageHeader, MSG_CMD_CAR_GPS,
                        CarlifeCarGps, uint32_t, antennastate, int, latitude,
                        int, longitude, int, height, uint32_t, speed, uint32_t,
                        heading, uint32_t, pdop, uint32_t, satsused, uint32_t,
                        satsvisible, uint64_t, timestamp)

// HU->MD
DECLARE_MEMBER_IMPL_FIVE(CmdChannel, car_gyroscope, CmdMessageHeader,
                         MSG_CMD_CAR_GYROSCOPE, CarlifeGyroscope, int, gyrotype,
                         double, gyrox, double, gyroy, double, gyroz, uint64_t,
                         timestamp)

// HU->MD
DECLARE_MEMBER_IMPL_FOUR(CmdChannel, car_acceleration, CmdMessageHeader,
                         MSG_CMD_CAR_ACCELERATION, CarlifeAcceleration, double,
                         accx, double, accy, double, accz, uint64_t, timestamp)

// HU->MD
DECLARE_MEMBER_IMPL_THREE(CmdChannel, car_oil, CmdMessageHeader,
                          MSG_CMD_CAR_OIL, CarlifeOil, int, level, int, range,
                          bool, lowfulewarning)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(CmdChannel, foreground, CmdMessageHeader,
                         MSG_CMD_FOREGROUND)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(CmdChannel, background, CmdMessageHeader,
                         MSG_CMD_BACKGROUND)

// HU->MD
DECLARE_MEMBER_IMPL_NONE(CmdChannel, launch_home, CmdMessageHeader,
                         MSG_CMD_LAUNCH_MODE_NORMAL)

// HU->MD
DECLARE_MEMBER_IMPL_NONE(CmdChannel, launch_phone, CmdMessageHeader,
                         MSG_CMD_LAUNCH_MODE_PHONE)

// HU->MD
DECLARE_MEMBER_IMPL_NONE(CmdChannel, launch_map, CmdMessageHeader,
                         MSG_CMD_LAUNCH_MODE_MAP)

// HU->MD
DECLARE_MEMBER_IMPL_NONE(CmdChannel, launch_music, CmdMessageHeader,
                         MSG_CMD_LAUNCH_MODE_MUSIC)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(CmdChannel, launch_exit, CmdMessageHeader,
                         MSG_CMD_GO_TO_DESKTOP)

// HU->MD
DECLARE_MEMBER_IMPL_NONE(CmdChannel, go_to_foreground, CmdMessageHeader,
                         MSG_CMD_GO_TO_FOREGROUND)

// HU->MD // MSG_CMD_CAR_DATA_GEAR
DECLARE_MEMBER_IMPL_ONE(CmdChannel, gear_info, CmdMessageHeader,
                        MSG_CMD_GEAR_INFO, CarlifeGearInfo, int, gear)

// MD->HU
DECLARE_MEMBER_IMPL_SIX(CmdChannel, navi_next_turn_info, CmdMessageHeader,
                        MSG_CMD_NAVI_NEXT_TURN_INFO, CarlifeNaviNextTurnInfo,
                        int, action, int, nextturn, const std::string &,
                        roadname, int, totaldistance, int, remaindistance, int,
                        time)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(CmdChannel, car_data_subscribe, CmdMessageHeader,
                         MSG_CMD_CAR_DATA_SUBSCRIBE)

// HU->MD
void CmdChannel::send_car_data_subscribe_done(
    const vehicle_info_vec_t &vehicle_infos) {

  CarlifeVehicleInfoList data;
  data.set_cnt(vehicle_infos.size());
  for (auto const &i : vehicle_infos) {
    CarlifeVehicleInfo *s = data.add_vehicleinfo();
    s->set_moduleid(std::get<0>(i));
    s->set_flag(std::get<1>(i));
    s->set_frequency(std::get<2>(i));
  }

  DECLARE_SND_MSG(CmdMessageHeader, MSG_CMD_CAR_DATA_SUBSCRIBE_DONE)
}

// MD->HU
DECLARE_MEMBER_IMPL_THREE(CmdChannel, car_data_subscribe_start,
                          CmdMessageHeader, MSG_CMD_CAR_DATA_SUBSCRIBE_START,
                          CarlifeVehicleInfo, int, moduleid, int, flag, int,
                          frequency)

// MD->HU
DECLARE_MEMBER_IMPL_THREE(CmdChannel, car_data_subscribe_stop, CmdMessageHeader,
                          MSG_CMD_CAR_DATA_SUBSCRIBE_STOP, CarlifeVehicleInfo,
                          int, moduleid, int, flag, int, frequency)

// MD->HU
DECLARE_MEMBER_IMPL_NINE(CmdChannel, media_info, CmdMessageHeader,
                         MSG_CMD_MEDIA_INFO, CarlifeMediaInfo,
                         const std::string &, source, const std::string &, song,
                         const std::string &, artist, const std::string &,
                         album, const std::string &, albumart, int, duration,
                         int, playlistnum, const std::string &, songid, int,
                         mode)

// MD->HU
DECLARE_MEMBER_IMPL_ONE(CmdChannel, media_progress_bar, CmdMessageHeader,
                        MSG_CMD_MEDIA_PROGRESS_BAR, CarlifeMediaProgressBar,
                        int, progressbar)

// MD->HU
DECLARE_MEMBER_IMPL_ONE(CmdChannel, connect_exception, CmdMessageHeader,
                        MSG_CMD_CONNECT_EXCEPTION, CarlifeConnectException, int,
                        exceptiontype)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(CmdChannel, request_go_to_foreground, CmdMessageHeader,
                         MSG_CMD_REQUEST_GO_TO_FOREGROUND)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(CmdChannel, ui_action_sound, CmdMessageHeader,
                         MSG_CMD_UI_ACTION_SOUND)

// HU->MD
CARLIFE_DATA_SUBSCRIBE_LIST_IMPL(carlife_data_subscribe,
                                 MSG_CMD_CARLIFE_DATA_SUBSCRIBE)

// MD->HU
CARLIFE_DATA_SUBSCRIBE_LIST_IMPL(carlife_data_subscribe_done,
                                 MSG_CMD_CARLIFE_DATA_SUBSCRIBE_DONE)

// HU->MD
CARLIFE_DATA_SUBSCRIBE_LIST_IMPL(carlife_data_subscribe_start,
                                 MSG_CMD_CARLIFE_DATA_SUBSCRIBE_START)

// HU->MD
CARLIFE_DATA_SUBSCRIBE_LIST_IMPL(carlife_data_subscribe_stop,
                                 MSG_CMD_CARLIFE_DATA_SUBSCRIBE_STOP)

// MD->HU
DECLARE_MEMBER_IMPL_SIX(CmdChannel, navi_assitantguide_info, CmdMessageHeader,
                        MSG_CMD_NAVI_ASSITANTGUIDE_INFO,
                        CarlifeNaviAssitantGuideInfo, int, action, int,
                        assistanttype, int, trafficsigntype, int, totaldistance,
                        int, remaindistance, int, cameraspeed)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(CmdChannel, go_to_foreground_response,
                         CmdMessageHeader, MSG_CMD_GO_TO_FOREGROUND_RESPONSE)

// MD->HU
DECLARE_MEMBER_IMPL_TWO(CmdChannel, start_bt_autopair_request, CmdMessageHeader,
                        MSG_CMD_START_BT_AUTOPAIR_REQUEST,
                        CarlifeBTStartPairReq, int, ostype, const std::string &,
                        address)

// HU->MD
DECLARE_MEMBER_IMPL_THREE(CmdChannel, bt_hfp_response, CmdMessageHeader,
                          MSG_CMD_BT_HFP_RESPONSE, CarlifeBTHfpResponse, int,
                          status, int, cmd, int, dtmfcode)

// MD->HU
DECLARE_MEMBER_IMPL_ONE(CmdChannel, bt_hfp_status_request, CmdMessageHeader,
                        MSG_CMD_BT_HFP_STATUS_REQUEST,
                        CarlifeBTHfpStatusRequest, int, type)

// HU->MD
DECLARE_MEMBER_IMPL_TWO(CmdChannel, bt_hfp_status_response, CmdMessageHeader,
                        MSG_CMD_BT_HFP_STATUS_RESPONSE,
                        CarlifeBTHfpStatusResponse, int, status, int, type)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(CmdChannel, md_feature_config_request,
                         CmdMessageHeader, MSG_CMD_MD_FEATURE_CONFIG_REQUEST)

// HU->MD
DECLARE_MEMBER_IMPL_NONE(CmdChannel, hu_feature_config_response,
                         CmdMessageHeader, MSG_CMD_HU_FEATURE_CONFIG_RESPONSE)

// HU->MD
DECLARE_MEMBER_IMPL_ONE(CmdChannel, bt_start_identify_req, CmdMessageHeader,
                        MSG_CMD_BT_START_IDENTIFY_REQ,
                        CarlifeBTStartIdentifyReq, const std::string &, address)

// MD->HU
DECLARE_MEMBER_IMPL_TWO(CmdChannel, bt_identify_result_ind, CmdMessageHeader,
                        MSG_CMD_BT_IDENTIFY_RESULT_IND,
                        CarlifeBTIdentifyResultInd, int, status,
                        const std::string &, address)

// HU->MD
DECLARE_MEMBER_IMPL_ONE(CmdChannel, error_code, CmdMessageHeader,
                        MSG_CMD_ERROR_CODE, CarlifeErrorCode,
                        const std::string &, errorcode)

// HU->MD
DECLARE_MEMBER_IMPL_NONE(CmdChannel, video_encoder_jpeg, CmdMessageHeader,
                         MSG_CMD_VIDEO_ENCODER_JPEG)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(CmdChannel, video_encoder_jpeg_ack, CmdMessageHeader,
                         MSG_CMD_VIDEO_ENCODER_JPEG_ACK)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(CmdChannel, md_exit, CmdMessageHeader, MSG_CMD_MD_EXIT)

////////////////////////////////////////////////////////////////////////////////
// verification cmd {
////////////////////////////////////////////////////////////////////////////////

// HU->MD
DECLARE_MEMBER_IMPL_ONE(CmdChannel, hu_authen_request, CmdMessageHeader,
                        MSG_CMD_HU_AUTHEN_REQUEST, CarlifeAuthenRequest,
                        const std::string &, randomvalue)

// MD->HU
DECLARE_MEMBER_IMPL_ONE(CmdChannel, md_authen_response, CmdMessageHeader,
                        MSG_CMD_MD_AUTHEN_RESPONSE, CarlifeAuthenResponse,
                        const std::string &, encryptvalue)

// HU->MD
DECLARE_MEMBER_IMPL_ONE(CmdChannel, hu_authen_result, CmdMessageHeader,
                        MSG_CMD_HU_AUTHEN_RESULT, CarlifeAuthenResult, bool,
                        authenresult)

// MD->HU
DECLARE_MEMBER_IMPL_ONE(CmdChannel, md_authen_result, CmdMessageHeader,
                        MSG_CMD_MD_AUTHEN_RESULT, CarlifeAuthenResult, bool,
                        authenresult)

// HU->MD
DECLARE_MEMBER_IMPL_EIGHT(CmdChannel, statistic_info, CmdMessageHeader,
                          MSG_CMD_STATISTIC_INFO, CarlifeStatisticsInfo,
                          const std::string &, cuid, const std::string &,
                          versionname, int, versioncode, const std::string &,
                          channel, int, connectcount, int, connectsuccesscount,
                          int, connecttime, const std::string &, crashlog)

////////////////////////////////////////////////////////////////////////////////
// verification cmd }
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// bluetooth cmd {
////////////////////////////////////////////////////////////////////////////////

// MD->HU
DECLARE_MEMBER_IMPL_THREE(CmdChannel, bt_hfp_request, CmdMessageHeader,
                          MSG_CMD_BT_HFP_REQUEST, CarlifeBTHfpRequest, int,
                          command, const std::string &, phonenum, int, dtmfcode)

// HU->MD
DECLARE_MEMBER_IMPL_FOUR(CmdChannel, bt_hfp_indication, CmdMessageHeader,
                         MSG_CMD_BT_HFP_INDICATION, CarlifeBTHfpIndication, int,
                         state, const std::string &, phonenum,
                         const std::string &, phonename, const std::string &,
                         address)

// HU->MD
DECLARE_MEMBER_IMPL_THREE(CmdChannel, bt_hfp_connection, CmdMessageHeader,
                          MSG_CMD_BT_HFP_INDICATION, CarlifeBTHfpConnection,
                          int, state, const std::string &, address,
                          const std::string &, name)

////////////////////////////////////////////////////////////////////////////////
// bluetooth cmd }
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Video {
////////////////////////////////////////////////////////////////////////////////

// HU->MD
DECLARE_MEMBER_IMPL_THREE(CmdChannel, encoder_init, CmdMessageHeader,
                          MSG_CMD_VIDEO_ENCODER_INIT, CarlifeVideoEncoderInfo,
                          int, width, int, height, int, framerate)

// MD->HU
DECLARE_MEMBER_IMPL_THREE(CmdChannel, encoder_init_done, CmdMessageHeader,
                          MSG_CMD_VIDEO_ENCODER_INIT_DONE,
                          CarlifeVideoEncoderInfo, int, width, int, height, int,
                          framerate)

// HU->MD
DECLARE_MEMBER_IMPL_NONE(CmdChannel, encoder_start, CmdMessageHeader,
                         MSG_CMD_VIDEO_ENCODER_START)

// HU->MD
DECLARE_MEMBER_IMPL_NONE(CmdChannel, encoder_pause, CmdMessageHeader,
                         MSG_CMD_VIDEO_ENCODER_PAUSE)

// HU->MD
DECLARE_MEMBER_IMPL_NONE(CmdChannel, encoder_reset, CmdMessageHeader,
                         MSG_CMD_VIDEO_ENCODER_RESET)

// HU->MD
DECLARE_MEMBER_IMPL_ONE(CmdChannel, encoder_frame_rate_change, CmdMessageHeader,
                        MSG_CMD_VIDEO_ENCODER_FRAME_RATE_CHANGE,
                        CarlifeVideoFrameRate, int, framerate)

// MD->HU
DECLARE_MEMBER_IMPL_ONE(CmdChannel, encoder_frame_rate_change_done,
                        CmdMessageHeader,
                        MSG_CMD_VIDEO_ENCODER_FRAME_RATE_CHANGE_DONE,
                        CarlifeVideoFrameRate, int, framerate)

////////////////////////////////////////////////////////////////////////////////
// Video }
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// VR {
////////////////////////////////////////////////////////////////////////////////

// MD->HU
DECLARE_MEMBER_IMPL_NONE(CmdChannel, mic_record_wakeup_start, CmdMessageHeader,
                         MSG_CMD_MIC_RECORD_WAKEUP_START)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(CmdChannel, mic_record_end, CmdMessageHeader,
                         MSG_CMD_MIC_RECORD_END)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(CmdChannel, mic_record_recog_start, CmdMessageHeader,
                         MSG_CMD_MIC_RECORD_RECOG_START)

// MD->HU
void CmdChannel::send_mic_status(const status_vec_t &status) {
  CarlifeModuleStatusList data;
  data.set_cnt(status.size());
  for (auto const &i : status) {
    CarlifeModuleStatus *s = data.add_modulestatus();
    s->set_moduleid(i.first);
    s->set_statusid(i.second);
  }

  DECLARE_SND_MSG(CmdMessageHeader, MSG_CMD_MODULE_STATUS)
}

// HU->MD
DECLARE_MEMBER_IMPL_TWO(CmdChannel, mic_control, CmdMessageHeader,
                        MSG_CMD_MODULE_CONTROL, CarlifeModuleStatus, int,
                        moduleid, int, statusid)

////////////////////////////////////////////////////////////////////////////////
// VR }
////////////////////////////////////////////////////////////////////////////////

const CmdMessageHeader &CmdChannel::transfer_from_head(const char *data) {
  return *(CmdMessageHeader *)data;
}

const char *CmdChannel::transfer_head(CmdMessageHeader &header) {
  header.length = sockets::hostToNet(header.length);
  header.service_type =
      (EPackageHeadType)sockets::hostToNet((uint32_t)header.service_type);
  return (const char *)&header;
}
