
#include "carlife/channel/CtrlChannel.h"

#include "CarlifeCarHardKeyCodeProto.pb.h"
#include "CarlifeTouchActionProto.pb.h"
#include "CarlifeTouchSinglePointProto.pb.h"
#include "utils/log.h"

using namespace com::baidu::carlife::protobuf;

RECV_DATA_IMPL(CtrlChannel, CtrlMessageHeader)

void CtrlChannel::parse_message(const char *data, size_t data_size) {
  PARSE_MESSAGE_HEAD_IMPL(CtrlMessageHeader)

  switch (header.service_type) {
  case MSG_TOUCH_ACTION: { // HU->MD
    DECLARE_CALLBACK_IMPL_THREE(action, CarlifeTouchAction, action, x, y)
    break;
  }
  case MSG_TOUCH_ACTION_DOWN: { // HU->MD
    DECLARE_CALLBACK_IMPL_TWO(action_down, CarlifeTouchSinglePoint, x, y)
    break;
  }
  case MSG_TOUCH_ACTION_UP: { // HU->MD
    DECLARE_CALLBACK_IMPL_TWO(action_up, CarlifeTouchSinglePoint, x, y)
    break;
  }
  case MSG_TOUCH_ACTION_MOVE: { // HU->MD
    DECLARE_CALLBACK_IMPL_TWO(action_move, CarlifeTouchSinglePoint, x, y)
    break;
  }
  case MSG_TOUCH_SINGLE_CLICK: { // HU->MD
    DECLARE_CALLBACK_IMPL_TWO(single_click, CarlifeTouchSinglePoint, x, y)
    break;
  }
  case MSG_TOUCH_DOUBLE_CLICK: { // HU->MD
    DECLARE_CALLBACK_IMPL_TWO(double_click, CarlifeTouchSinglePoint, x, y)
    break;
  }
  case MSG_TOUCH_LONG_PRESS: { // HU->MD
    DECLARE_CALLBACK_IMPL_TWO(long_press, CarlifeTouchSinglePoint, x, y)
    break;
  }
  case MSG_TOUCH_CAR_HARD_KEY_CODE: { // HU->MD
    DECLARE_CALLBACK_IMPL_ONE(car_hard_key_code, CarlifeCarHardKeyCode, keycode)
    break;
  }
  default:
    LOG_ERROR("undefine service type!");
    break;
  }
}

// HU->MD
DECLARE_MEMBER_IMPL_THREE(CtrlChannel, action, CtrlMessageHeader,
                          MSG_TOUCH_ACTION, CarlifeTouchAction, int, action,
                          int, x, int, y)

// HU->MD
DECLARE_MEMBER_IMPL_TWO(CtrlChannel, action_down, CtrlMessageHeader,
                        MSG_TOUCH_ACTION_DOWN, CarlifeTouchSinglePoint, int, x,
                        int, y)

// HU->MD
DECLARE_MEMBER_IMPL_TWO(CtrlChannel, action_up, CtrlMessageHeader,
                        MSG_TOUCH_ACTION_UP, CarlifeTouchSinglePoint, int, x,
                        int, y)

// HU->MD
DECLARE_MEMBER_IMPL_TWO(CtrlChannel, action_move, CtrlMessageHeader,
                        MSG_TOUCH_ACTION_MOVE, CarlifeTouchSinglePoint, int, x,
                        int, y)

// HU->MD
DECLARE_MEMBER_IMPL_TWO(CtrlChannel, single_click, CtrlMessageHeader,
                        MSG_TOUCH_SINGLE_CLICK, CarlifeTouchSinglePoint, int, x,
                        int, y)

// HU->MD
DECLARE_MEMBER_IMPL_TWO(CtrlChannel, double_click, CtrlMessageHeader,
                        MSG_TOUCH_DOUBLE_CLICK, CarlifeTouchSinglePoint, int, x,
                        int, y)

// HU->MD
DECLARE_MEMBER_IMPL_TWO(CtrlChannel, long_press, CtrlMessageHeader,
                        MSG_TOUCH_LONG_PRESS, CarlifeTouchSinglePoint, int, x,
                        int, y)

// HU->MD
DECLARE_MEMBER_IMPL_ONE(CtrlChannel, car_hard_key_code, CtrlMessageHeader,
                        MSG_TOUCH_CAR_HARD_KEY_CODE, CarlifeCarHardKeyCode, int,
                        keycode)

const CtrlMessageHeader &CtrlChannel::transfer_from_head(const char *data) {
  return *(CtrlMessageHeader *)data;
}

const char *CtrlChannel::transfer_head(CtrlMessageHeader &header) {
  header.length = sockets::host_to_net(header.length);
  header.service_type =
      (EPackageHeadType)sockets::host_to_net((uint32_t)header.service_type);
  return (const char *)&header;
}
