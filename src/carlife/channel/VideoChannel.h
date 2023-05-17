
#ifndef CARLIFE_CHANNEL_VIDEOCHANNEL_H
#define CARLIFE_CHANNEL_VIDEOCHANNEL_H

#include "carlife/channel/ChannelBase.h"

/**
```mermaid
sequenceDiagram
  HU->>MD: MSG_CMD_VIDEO_ENCODER_INIT
  MD->>HU: MSG_CMD_VIDEO_ENCODER_INIT_DONE
  HU->>MD: MSG_CMD_VIDEO_ENCODER_START

  note left of HU: change frame rate
  HU->>MD: MSG_CMD_VIDEO_ENCODER_FRAME_RATE_CHANGE
  MD->>HU: MSG_CMD_VIDEO_ENCODER_FRAME_RATE_CHANGE_DONE

  note left of HU: pause video
  HU->>MD: MSG_CMD_VIDEO_ENCODER_PAUSE
  HU->>MD: MSG_CMD_VIDEO_ENCODER_START
```
*/
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

}; // class VideoChannel

#endif // CARLIFE_CHANNEL_VIDEOCHANNEL_H
