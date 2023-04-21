
#include "carlife/channel/TTSChannel.h"

#include <chrono>

#include "CarlifeTTSInitProto.pb.h"
#include "utils/log.h"

using namespace com::baidu::carlife::protobuf;

RECV_DATA_IMPL(TTSChannel, AudioMessageHeader)

void TTSChannel::parse_message(const char *data, size_t data_size) {
  PARSE_MESSAGE_HEAD_IMPL(AudioMessageHeader)

  switch (header.service_type) {
  case MSG_TTS_INIT: { // MD->HU
    DECLARE_CALLBACK_IMPL_THREE(tts_init, CarlifeTTSInit, samplerate,
                                channelconfig, sampleformat)
    break;
  }
  case MSG_TTS_STOP: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(tts_stop)
    break;
  }
  case MSG_TTS_DATA: { // MD->HU
    if (m_callback_tts_data) {
      m_callback_tts_data(header.timestamp, message, message_size);
    }
    break;
  }
  default:
    LOG_ERROR("undefine service type!");
    break;
  }
}

// MD->HU
DECLARE_MEMBER_IMPL_THREE(TTSChannel, tts_init, AudioMessageHeader,
                          MSG_TTS_INIT, CarlifeTTSInit, int, samplerate, int,
                          channelconfig, int, sampleformat)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(TTSChannel, tts_stop, AudioMessageHeader, MSG_TTS_STOP)

// MD->HU
void TTSChannel::send_tts_data(uint64_t timestamp, const char *tts,
                               size_t tts_size) {
  /* set header */
  AudioMessageHeader header = {};
  header.service_type = MSG_TTS_DATA;
  header.length = tts_size;
  header.timestamp = timestamp;

  /* send */
  send(transfer_head(header), sizeof(AudioMessageHeader));
  send(tts, tts_size);
}

const AudioMessageHeader &TTSChannel::transfer_from_head(const char *data) {
  return *(AudioMessageHeader *)data;
}

const char *TTSChannel::transfer_head(AudioMessageHeader &header) {
  header.length = sockets::hostToNet(header.length);
  header.service_type =
      (EPackageHeadType)sockets::hostToNet((uint32_t)header.service_type);
  return (const char *)&header;
}
