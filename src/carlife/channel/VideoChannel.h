
#ifndef CARLIFE_CHANNEL_VIDEOCHANNEL_H
#define CARLIFE_CHANNEL_VIDEOCHANNEL_H

#include "carlife/channel/ChannelBase.h"

class VideoChannel : public ChannelBase {
public:
  // MD->HU
  // paramter: timestamp, data, data_size
  DECLARE_MEMBER_FUNCS(video_data, uint64_t, const char *, size_t);

  // MD->HU
  DECLARE_MEMBER_FUNCS(heartbeat, );

public:
  void recv_data(const char *data, size_t data_size) override;

private:
  void parse_message(const char *data, size_t data_size);

  const VideoMessageHeader &transfer_from_head(const char *data);
  const char *transfer_head(VideoMessageHeader &header);

  char buff_[VIDEO_DATA_SIZE];
  size_t buff_size_;

};     // class VideoChannel

#endif // CARLIFE_CHANNEL_VIDEOCHANNEL_H
