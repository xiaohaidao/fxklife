
#ifndef CARLIFE_CHANNEL_CTRLCHANNEL_H
#define CARLIFE_CHANNEL_CTRLCHANNEL_H

#include "carlife/channel/ChannelBase.h"

class CtrlChannel : public ChannelBase {
public:
  // HU->MD
  // parameter: action, x, y
  DECLARE_MEMBER_FUNCS(action, int, int, int);

  // HU->MD
  // parameter: x, y
  DECLARE_MEMBER_FUNCS(action_down, int, int);

  // HU->MD
  // parameter: x, y
  DECLARE_MEMBER_FUNCS(action_up, int, int);

  // HU->MD
  // parameter: x, y
  DECLARE_MEMBER_FUNCS(action_move, int, int);

  // HU->MD
  // parameter: x, y
  DECLARE_MEMBER_FUNCS(single_click, int, int);

  // HU->MD
  // parameter: x, y
  DECLARE_MEMBER_FUNCS(double_click, int, int);

  // HU->MD
  // parameter: x, y
  DECLARE_MEMBER_FUNCS(long_press, int, int);

  // HU->MD
  // parameter: keycode
  DECLARE_MEMBER_FUNCS(car_hard_key_code, int);

public:
  void recv_data(const char *data, size_t data_size) override;

private:
  void parse_message(const char *data, size_t data_size);

  const CtrlMessageHeader &transfer_from_head(const char *data);
  const char *transfer_head(CtrlMessageHeader &header);

  char buff_[CTRL_DATA_SIZE];
  size_t buff_size_;

};     // class CtrlChannel

#endif // CARLIFE_CHANNEL_CTRLCHANNEL_H
