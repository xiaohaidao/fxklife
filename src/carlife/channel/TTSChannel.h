
#ifndef CARLIFE_CHANNEL_TTSCHANNEL_H
#define CARLIFE_CHANNEL_TTSCHANNEL_H

#include "carlife/channel/ChannelBase.h"

class TTSChannel : public ChannelBase {
public:
  // MD->HU
  // paramter: rate hz, channel pcs, format bit
  DECLARE_MEMBER_FUNCS(tts_init, int, int, int);

  // MD->HU
  DECLARE_MEMBER_FUNCS(tts_stop, );

  // MD->HU
  // paramter: timestamp, data, data_size
  DECLARE_MEMBER_FUNCS(tts_data, uint64_t, const char *, size_t);

public:
  void recv_data(const char *data, size_t data_size) override;

private:
  void parse_message(const char *data, size_t data_size);

  const AudioMessageHeader &transfer_from_head(const char *data);
  const char *transfer_head(AudioMessageHeader &header);

  char buff_[TTS_DATA_SIZE];
  size_t buff_size_;

};     // class TTSChannel

#endif // CARLIFE_CHANNEL_TTSCHANNEL_H
