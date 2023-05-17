
#include "carlife/channel/VRChannel.h"

#include <chrono>

#include "CarlifeModuleStatusListProto.pb.h"
#include "CarlifeTTSInitProto.pb.h"
#include "utils/log.h"

using namespace com::baidu::carlife::protobuf;

RECV_DATA_IMPL(VRChannel, VoiceMessageHeader)

void VRChannel::parse_message(const char *data, size_t data_size) {
  PARSE_MESSAGE_HEAD_IMPL(VoiceMessageHeader)

  switch (header.service_type) {
  //// vr
  case MSG_VR_MIC_DATA: { // HU->MD
    if (m_callback_mic_data) {
      m_callback_mic_data(header.timestamp, message, message_size);
    }
    break;
  }
  case MSG_VR_INIT: { // MD->HU
    DECLARE_CALLBACK_IMPL_THREE(vr_init, CarlifeTTSInit, samplerate,
                                channelconfig, sampleformat)
    break;
  }
  case MSG_VR_DATA: { // MD->HU
    if (m_callback_vr_data) {
      m_callback_vr_data(header.timestamp, message, message_size);
    }
    break;
  }
  case MSG_VR_STOP: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(vr_stop)
    break;
  }
  case MSG_VR_STATUS: { // MD->HU
    CarlifeModuleStatusList data;
    data.ParseFromArray(message, message_size);

    if (m_callback_vr_status) {
      // status_vec_t result(data.cnt());
      status_vec_t result(data.modulestatus_size());
      // assert(result == data.cnt());
      for (size_t i = 0; i < result.size(); ++i) {
        const CarlifeModuleStatus &status = data.modulestatus(i);
        result[i] = std::make_pair(status.moduleid(), status.statusid());
      }
      m_callback_vr_status(result);
    }
    break;
  }
  case MSG_VR_INTERRUPT: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(vr_interrupt)
    break;
  }
  default:
    LOG_ERROR("undefine service type!");
    break;
  }
}

// MD->HU
DECLARE_MEMBER_IMPL_THREE(VRChannel, vr_init, VoiceMessageHeader, MSG_VR_INIT,
                          CarlifeTTSInit, int, samplerate, int, channelconfig,
                          int, sampleformat)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(VRChannel, vr_stop, VoiceMessageHeader, MSG_VR_STOP)

// MD->HU
void VRChannel::send_vr_status(const status_vec_t &status) {
  CarlifeModuleStatusList data;
  data.set_cnt(status.size());
  for (auto const &i : status) {
    CarlifeModuleStatus *s = data.add_modulestatus();
    s->set_moduleid(i.first);
    s->set_statusid(i.second);
  }

  DECLARE_SND_MSG(VoiceMessageHeader, MSG_VR_STATUS)
}

// MD->HU
DECLARE_MEMBER_IMPL_NONE(VRChannel, vr_interrupt, VoiceMessageHeader,
                         MSG_VR_INTERRUPT)

// HU->MD
void VRChannel::send_mic_data(uint64_t timestamp, const char *tts,
                              size_t tts_size) {
  /* set header */
  VoiceMessageHeader header = {};
  header.service_type = MSG_VR_MIC_DATA;
  header.length = tts_size;

  /* send */
  send(transfer_head(header), sizeof(VoiceMessageHeader));
  send(tts, tts_size);
}

// MD->HU
void VRChannel::send_vr_data(uint64_t timestamp, const char *tts,
                             size_t tts_size) {
  /* set header */
  VoiceMessageHeader header = {};
  header.service_type = MSG_VR_DATA;
  header.length = tts_size;
  header.timestamp = timestamp;

  /* send */
  send(transfer_head(header), sizeof(VoiceMessageHeader));
  send(tts, tts_size);
}

const VoiceMessageHeader &VRChannel::transfer_from_head(const char *data) {
  return *(VoiceMessageHeader *)data;
}

const char *VRChannel::transfer_head(VoiceMessageHeader &header) {
  header.length = sockets::host_to_net(header.length);
  header.service_type =
      (EPackageHeadType)sockets::host_to_net((uint32_t)header.service_type);
  return (const char *)&header;
}
