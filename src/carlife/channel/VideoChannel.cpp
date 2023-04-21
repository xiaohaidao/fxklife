
#include "carlife/channel/VideoChannel.h"

#include <chrono>

#include "CarLifeVideoEncoderInfoProto.pb.h"
#include "utils/log.h"

using namespace com::baidu::carlife::protobuf;

RECV_DATA_IMPL(VideoChannel, VideoMessageHeader)

void VideoChannel::parse_message(const char *data, size_t data_size) {
  PARSE_MESSAGE_HEAD_IMPL(VideoMessageHeader)

  switch (header.service_type) {
  case MSG_VIDEO_DATA: { // MD->HU
    if (m_callback_video_data) {
      m_callback_video_data(header.timestamp, message, message_size);
    }

    break;
  }
  case MSG_VIDEO_HEARTBEAT: { // MD->HU
    DECLARE_CALLBACK_IMPL_NONE(heartbeat)
    break;
  }

  default:
    LOG_ERROR("undefine service type!");
    break;
  }
}

// MD->HU
void VideoChannel::send_video_data(uint64_t timestamp, const char *video,
                                   size_t video_size) {
  /* set header */
  VideoMessageHeader header = {};
  header.service_type = MSG_VIDEO_DATA;
  header.length = video_size;
  header.timestamp = timestamp;

  /* send */
  send(transfer_head(header), sizeof(VideoMessageHeader));
  send(video, video_size);
}

// MD->HU
DECLARE_MEMBER_IMPL_NONE(VideoChannel, heartbeat, VideoMessageHeader,
                         MSG_VIDEO_HEARTBEAT)

const VideoMessageHeader &VideoChannel::transfer_from_head(const char *data) {
  return *(VideoMessageHeader *)data;
}

const char *VideoChannel::transfer_head(VideoMessageHeader &header) {
  header.length = sockets::hostToNet(header.length);
  header.service_type =
      (EPackageHeadType)sockets::hostToNet((uint32_t)header.service_type);
  return (const char *)&header;
}
