
#ifndef CARLIFE_CHANNEL_MEDIACHANNEL_H
#define CARLIFE_CHANNEL_MEDIACHANNEL_H

#include "carlife/channel/ChannelBase.h"

/**
```mermaid
sequenceDiagram
  note left of MD: start to play music
  MD->>HU: MSG_MEDIA_INIT
  MD->>HU: MSG_MEDIA_DATA

  note left of MD: pause music
  MD->>HU: MSG_MEDIA_PAUSE

  note left of MD: resume music
  MD->>HU: MSG_MEDIA_RESUME_PLAY
  MD->>HU: MSG_MEDIA_DATA
```
*/
class MediaChannel : public ChannelBase {
public:
  // MD->HU
  // paramter: rate hz, channel pcs, format bit
  DECLARE_MEMBER_FUNCS(media_init, int, int, int);

  // MD->HU
  DECLARE_MEMBER_FUNCS(media_pause, );

  // MD->HU
  DECLARE_MEMBER_FUNCS(media_play, );

  // MD->HU
  // paramter: timestamp, data, data_size
  DECLARE_MEMBER_FUNCS(media_data, uint64_t, const char *, size_t);

public:
  void recv_data(const char *data, size_t data_size) override;

private:
  void parse_message(const char *data, size_t data_size);

  const AudioMessageHeader &transfer_from_head(const char *data);
  const char *transfer_head(AudioMessageHeader &header);

  char buff_[MEDIA_DATA_SIZE];
  size_t buff_size_;

}; // class MediaChannel

#endif // CARLIFE_CHANNEL_MEDIACHANNEL_H
