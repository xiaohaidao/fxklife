
#include "carlife/channel/MediaChannel.h"

#include <chrono>

#include "CarlifeMusicInitProto.pb.h"
#include "utils/log.h"

using namespace com::baidu::carlife::protobuf;

RECV_DATA_IMPL(MediaChannel, AudioMessageHeader)

void MediaChannel::parse_message(const char *data, size_t data_size) {
  PARSE_MESSAGE_HEAD_IMPL(AudioMessageHeader)

  switch (header.service_type) {
  case MSG_MEDIA_INIT: { // MD->HU
    DECLARE_CALLBACK_IMPL_THREE(media_init, CarlifeMusicInit, samplerate,
                                channelconfig, sampleformat)
    break;
  }
  case MSG_MEDIA_PAUSE: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(media_pause)
    break;
  }
  case MSG_MEDIA_RESUME_PLAY: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(media_play)
    break;
  }
  case MSG_MEDIA_DATA: { // MD->HU
    if (m_callback_media_data) {
      m_callback_media_data(header.timestamp, message, message_size);
    }
    break;
  }
  default:
    LOG_ERROR("undefine service type!");
    break;
  }
}

// MD->HU
DECLARE_MEMBER_IMPL_THREE(MediaChannel, media_init, AudioMessageHeader,
                          MSG_MEDIA_INIT, CarlifeMusicInit, int, samplerate,
                          int, channelconfig, int, sampleformat)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(MediaChannel, media_pause, AudioMessageHeader,
                         MSG_MEDIA_PAUSE)

// MD->HU
DECLARE_MEMBER_IMPL_NONE(MediaChannel, media_play, AudioMessageHeader,
                         MSG_MEDIA_RESUME_PLAY)

// MD->HU
void MediaChannel::send_media_data(uint64_t timestamp, const char *media,
                                   size_t media_size) {
  /* set header */
  AudioMessageHeader header = {};
  header.service_type = MSG_MEDIA_DATA;
  header.length = media_size;
  header.timestamp = timestamp;

  /* send */
  send(transfer_head(header), sizeof(AudioMessageHeader));
  send(media, media_size);
}

const AudioMessageHeader &MediaChannel::transfer_from_head(const char *data) {
  return *(AudioMessageHeader *)data;
}

const char *MediaChannel::transfer_head(AudioMessageHeader &header) {
  header.length = sockets::hostToNet(header.length);
  header.service_type =
      (EPackageHeadType)sockets::hostToNet((uint32_t)header.service_type);
  return (const char *)&header;
}
