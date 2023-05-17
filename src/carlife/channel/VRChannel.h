
#ifndef CARLIFE_CHANNEL_VRCHANNEL_H
#define CARLIFE_CHANNEL_VRCHANNEL_H

#include "carlife/channel/ChannelBase.h"

// parameter: [module_id, status_id]
typedef std::vector<std::pair<int, int>> status_vec_t;

/**
  Timing diagram
  MD                               HU
       -- status -->

       -- record_wakerup_start -->
       <-- vr_data (1k/package) --
       <-- ... --
       -- mic_record_end -->

       -- status -->
---

```mermaid
sequenceDiagram
  note left of MD: wakeup
  MD->>HU: MSG_CMD_MIC_RECORD_WAKEUP_START
  HU-->>MD: MSG_VR_MIC_DATA(MSG_VR_DATA)
  MD->>HU: MSG_CMD_MIC_RECORD_END

  note left of HU: mic
  MD->>HU: MSG_CMD_PROTOCOL_VERSION_MATCH_STATUS
  HU-->>MD: MSG_CMD_MODULE_CONTROL(param1:	CARLIFE_MIC_MODULE_ID, param2:
MIC_STATUS_USE_MOBILE_MIC)

  note left of MD: send VR TTS data to HU
  MD->>HU: MSG_VR_INIT(MSG_VR_AUDIO_INIT)
  MD->>HU: MSG_VR_DATA(MSG_VR_AUDIO_DATA)
  MD->>HU: MSG_VR_STOP(MSG_VR_AUDIO_STOP)
```
*/
class VRChannel : public ChannelBase {
public: //! vr
  // MD->HU
  // paramter: rate hz, channel pcs, format bit
  DECLARE_MEMBER_FUNCS(vr_init, int, int, int);

  // MD->HU
  DECLARE_MEMBER_FUNCS(vr_stop, );

  // MD->HU
  // parameter: [module_id, status_id]
  DECLARE_MEMBER_FUNCS(vr_status, const status_vec_t &);

  // MD->HU
  DECLARE_MEMBER_FUNCS(vr_interrupt, );

  // HU->MD
  // paramter: timestamp, data, data_size
  DECLARE_MEMBER_FUNCS(mic_data, uint64_t, const char *, size_t);

  // HU->MD
  // paramter: timestamp, data, data_size
  DECLARE_MEMBER_FUNCS(vr_data, uint64_t, const char *, size_t);

public:
  void recv_data(const char *data, size_t data_size) override;

private:
  void parse_message(const char *data, size_t data_size);

  const VoiceMessageHeader &transfer_from_head(const char *data);
  const char *transfer_head(VoiceMessageHeader &header);

  char buff_[VR_DATA_SIZE];
  size_t buff_size_;

}; // class VRChannel

#endif // CARLIFE_CHANNEL_VRCHANNEL_H
