#ifndef CARLIFE_CHANNEL_CHANNELBASEMACRO_H
#define CARLIFE_CHANNEL_CHANNELBASEMACRO_H

////////////////////////////////////////////////////////////////////////////////
// function declare
////////////////////////////////////////////////////////////////////////////////

#define DECLARE_MEMBER_FUNCS(name, ...)                                        \
public:                                                                        \
  typedef void(type_##name)(__VA_ARGS__);                                      \
  type_##name send_##name;                                                     \
  void callback_##name(const std::function<type_##name> &f) {                  \
    m_callback_##name = f;                                                     \
  }                                                                            \
                                                                               \
private:                                                                       \
  std::function<type_##name> m_callback_##name

#define DECLARE_MEMBER_FUNCS_TWENTY_FOUR(                                      \
    name, param_type1, param_type2, param_type3, param_type4, param_type5,     \
    param_type6, param_type7, param_type8, param_type9, param_type10,          \
    param_type11, param_type12, param_type13, param_type14, param_type15,      \
    param_type16, param_type17, param_type18, param_type19, param_type20,      \
    param_type21, param_type22, param_type23, param_type24)                    \
public:                                                                        \
  typedef void(type_##name)(                                                   \
      param_type1, param_type2, param_type3, param_type4, param_type5,         \
      param_type6, param_type7, param_type8, param_type9, param_type10,        \
      param_type11, param_type12, param_type13, param_type14, param_type15,    \
      param_type16, param_type17, param_type18, param_type19, param_type20,    \
      param_type21, param_type22, param_type23, param_type24);                 \
                                                                               \
  type_##name send_##name;                                                     \
  void callback_##name(const std::function<type_##name> &f) {                  \
    m_callback_##name = f;                                                     \
  }                                                                            \
                                                                               \
private:                                                                       \
  std::function<type_##name> m_callback_##name

////////////////////////////////////////////////////////////////////////////////
// function implementation
////////////////////////////////////////////////////////////////////////////////

#define DECLARE_SND_MSG(message_type, servicetype)                             \
  /* set header */                                                             \
  message_type header = {};                                                    \
  header.service_type = servicetype;                                           \
  header.length = data.ByteSizeLong();                                         \
                                                                               \
  /* send */                                                                   \
  LOG_DEBUG("send service type 0x%08X and size %d", header.service_type,       \
            header.length);                                                    \
  send(transfer_head(header), sizeof(message_type), data);

#define DECLARE_SND_NONE_MSG(message_type, servicetype)                        \
  /* set header */                                                             \
  message_type header = {};                                                    \
  header.service_type = servicetype;                                           \
                                                                               \
  /* send */                                                                   \
  LOG_DEBUG("send service type 0x%08X and size %d", header.service_type,       \
            header.length);                                                    \
  send(transfer_head(header), sizeof(message_type));

#define DECLARE_MEMBER_IMPL_TWENTY_FOUR(                                       \
    class_name, name, message_type, service_type, service_struct, param1_type, \
    param1_name, param2_type, param2_name, param3_type, param3_name,           \
    param4_type, param4_name, param5_type, param5_name, param6_type,           \
    param6_name, param7_type, param7_name, param8_type, param8_name,           \
    param9_type, param9_name, param10_type, param10_name, param11_type,        \
    param11_name, param12_type, param12_name, param13_type, param13_name,      \
    param14_type, param14_name, param15_type, param15_name, param16_type,      \
    param16_name, param17_type, param17_name, param18_type, param18_name,      \
    param19_type, param19_name, param20_type, param20_name, param21_type,      \
    param21_name, param22_type, param22_name, param23_type, param23_name,      \
    param24_type, param24_name)                                                \
                                                                               \
  void class_name::send_##name(                                                \
      param1_type param1_name, param2_type param2_name,                        \
      param3_type param3_name, param4_type param4_name,                        \
      param5_type param5_name, param6_type param6_name,                        \
      param7_type param7_name, param8_type param8_name,                        \
      param9_type param9_name, param10_type param10_name,                      \
      param11_type param11_name, param12_type param12_name,                    \
      param13_type param13_name, param14_type param14_name,                    \
      param15_type param15_name, param16_type param16_name,                    \
      param17_type param17_name, param18_type param18_name,                    \
      param19_type param19_name, param20_type param20_name,                    \
      param21_type param21_name, param22_type param22_name,                    \
      param23_type param23_name, param24_type param24_name) {                  \
                                                                               \
    service_struct data;                                                       \
    data.set_##param1_name(param1_name);                                       \
    data.set_##param2_name(param2_name);                                       \
    data.set_##param3_name(param3_name);                                       \
    data.set_##param4_name(param4_name);                                       \
    data.set_##param5_name(param5_name);                                       \
    data.set_##param6_name(param6_name);                                       \
    data.set_##param7_name(param7_name);                                       \
    data.set_##param8_name(param8_name);                                       \
    data.set_##param9_name(param9_name);                                       \
    data.set_##param10_name(param10_name);                                     \
    data.set_##param11_name(param11_name);                                     \
    data.set_##param12_name(param12_name);                                     \
    data.set_##param13_name(param13_name);                                     \
    data.set_##param14_name(param14_name);                                     \
    data.set_##param15_name(param15_name);                                     \
    data.set_##param16_name(param16_name);                                     \
    data.set_##param17_name(param17_name);                                     \
    data.set_##param18_name(param18_name);                                     \
    data.set_##param19_name(param19_name);                                     \
    data.set_##param20_name(param20_name);                                     \
    data.set_##param21_name(param21_name);                                     \
    data.set_##param22_name(param22_name);                                     \
    data.set_##param23_name(param23_name);                                     \
    data.set_##param24_name(param24_name);                                     \
    DECLARE_SND_MSG(message_type, service_type)                                \
  }

#define DECLARE_MEMBER_IMPL_TEN(                                               \
    class_name, name, message_type, service_type, service_struct, param1_type, \
    param1_name, param2_type, param2_name, param3_type, param3_name,           \
    param4_type, param4_name, param5_type, param5_name, param6_type,           \
    param6_name, param7_type, param7_name, param8_type, param8_name,           \
    param9_type, param9_name, param10_type, param10_name)                      \
                                                                               \
  void class_name::send_##name(                                                \
      param1_type param1_name, param2_type param2_name,                        \
      param3_type param3_name, param4_type param4_name,                        \
      param5_type param5_name, param6_type param6_name,                        \
      param7_type param7_name, param8_type param8_name,                        \
      param9_type param9_name, param10_type param10_name) {                    \
                                                                               \
    service_struct data;                                                       \
    data.set_##param1_name(param1_name);                                       \
    data.set_##param2_name(param2_name);                                       \
    data.set_##param3_name(param3_name);                                       \
    data.set_##param4_name(param4_name);                                       \
    data.set_##param5_name(param5_name);                                       \
    data.set_##param6_name(param6_name);                                       \
    data.set_##param7_name(param7_name);                                       \
    data.set_##param8_name(param8_name);                                       \
    data.set_##param9_name(param9_name);                                       \
    data.set_##param10_name(param10_name);                                     \
    DECLARE_SND_MSG(message_type, service_type)                                \
  }

#define DECLARE_MEMBER_IMPL_NINE(                                              \
    class_name, name, message_type, service_type, service_struct, param1_type, \
    param1_name, param2_type, param2_name, param3_type, param3_name,           \
    param4_type, param4_name, param5_type, param5_name, param6_type,           \
    param6_name, param7_type, param7_name, param8_type, param8_name,           \
    param9_type, param9_name)                                                  \
                                                                               \
  void class_name::send_##name(                                                \
      param1_type param1_name, param2_type param2_name,                        \
      param3_type param3_name, param4_type param4_name,                        \
      param5_type param5_name, param6_type param6_name,                        \
      param7_type param7_name, param8_type param8_name,                        \
      param9_type param9_name) {                                               \
                                                                               \
    service_struct data;                                                       \
    data.set_##param1_name(param1_name);                                       \
    data.set_##param2_name(param2_name);                                       \
    data.set_##param3_name(param3_name);                                       \
    data.set_##param4_name(param4_name);                                       \
    data.set_##param5_name(param5_name);                                       \
    data.set_##param6_name(param6_name);                                       \
    data.set_##param7_name(param7_name);                                       \
    data.set_##param8_name(param8_name);                                       \
    data.set_##param9_name(param9_name);                                       \
    DECLARE_SND_MSG(message_type, service_type)                                \
  }

#define DECLARE_MEMBER_IMPL_EIGHT(                                             \
    class_name, name, message_type, service_type, service_struct, param1_type, \
    param1_name, param2_type, param2_name, param3_type, param3_name,           \
    param4_type, param4_name, param5_type, param5_name, param6_type,           \
    param6_name, param7_type, param7_name, param8_type, param8_name)           \
                                                                               \
  void class_name::send_##name(                                                \
      param1_type param1_name, param2_type param2_name,                        \
      param3_type param3_name, param4_type param4_name,                        \
      param5_type param5_name, param6_type param6_name,                        \
      param7_type param7_name, param8_type param8_name) {                      \
                                                                               \
    service_struct data;                                                       \
    data.set_##param1_name(param1_name);                                       \
    data.set_##param2_name(param2_name);                                       \
    data.set_##param3_name(param3_name);                                       \
    data.set_##param4_name(param4_name);                                       \
    data.set_##param5_name(param5_name);                                       \
    data.set_##param6_name(param6_name);                                       \
    data.set_##param7_name(param7_name);                                       \
    data.set_##param8_name(param8_name);                                       \
    DECLARE_SND_MSG(message_type, service_type)                                \
  }

#define DECLARE_MEMBER_IMPL_SIX(                                               \
    class_name, name, message_type, service_type, service_struct, param1_type, \
    param1_name, param2_type, param2_name, param3_type, param3_name,           \
    param4_type, param4_name, param5_type, param5_name, param6_type,           \
    param6_name)                                                               \
                                                                               \
  void class_name::send_##name(                                                \
      param1_type param1_name, param2_type param2_name,                        \
      param3_type param3_name, param4_type param4_name,                        \
      param5_type param5_name, param6_type param6_name) {                      \
                                                                               \
    service_struct data;                                                       \
    data.set_##param1_name(param1_name);                                       \
    data.set_##param2_name(param2_name);                                       \
    data.set_##param3_name(param3_name);                                       \
    data.set_##param4_name(param4_name);                                       \
    data.set_##param5_name(param5_name);                                       \
    data.set_##param6_name(param6_name);                                       \
    DECLARE_SND_MSG(message_type, service_type)                                \
  }

#define DECLARE_MEMBER_IMPL_FIVE(                                              \
    class_name, name, message_type, service_type, service_struct, param1_type, \
    param1_name, param2_type, param2_name, param3_type, param3_name,           \
    param4_type, param4_name, param5_type, param5_name)                        \
                                                                               \
  void class_name::send_##name(                                                \
      param1_type param1_name, param2_type param2_name,                        \
      param3_type param3_name, param4_type param4_name,                        \
      param5_type param5_name) {                                               \
                                                                               \
    service_struct data;                                                       \
    data.set_##param1_name(param1_name);                                       \
    data.set_##param2_name(param2_name);                                       \
    data.set_##param3_name(param3_name);                                       \
    data.set_##param4_name(param4_name);                                       \
    data.set_##param5_name(param5_name);                                       \
    DECLARE_SND_MSG(message_type, service_type)                                \
  }

#define DECLARE_MEMBER_IMPL_FOUR(class_name, name, message_type, service_type, \
                                 service_struct, param1_type, param1_name,     \
                                 param2_type, param2_name, param3_type,        \
                                 param3_name, param4_type, param4_name)        \
                                                                               \
  void class_name::send_##name(                                                \
      param1_type param1_name, param2_type param2_name,                        \
      param3_type param3_name, param4_type param4_name) {                      \
                                                                               \
    service_struct data;                                                       \
    data.set_##param1_name(param1_name);                                       \
    data.set_##param2_name(param2_name);                                       \
    data.set_##param3_name(param3_name);                                       \
    data.set_##param4_name(param4_name);                                       \
    DECLARE_SND_MSG(message_type, service_type)                                \
  }

#define DECLARE_MEMBER_IMPL_THREE(                                             \
    class_name, name, message_type, service_type, service_struct, param1_type, \
    param1_name, param2_type, param2_name, param3_type, param3_name)           \
                                                                               \
  void class_name::send_##name(param1_type param1_name,                        \
                               param2_type param2_name,                        \
                               param3_type param3_name) {                      \
    service_struct data;                                                       \
    data.set_##param1_name(param1_name);                                       \
    data.set_##param2_name(param2_name);                                       \
    data.set_##param3_name(param3_name);                                       \
    DECLARE_SND_MSG(message_type, service_type)                                \
  }

#define DECLARE_MEMBER_IMPL_TWO(class_name, name, message_type, service_type,  \
                                service_struct, param1_type, param1_name,      \
                                param2_type, param2_name)                      \
                                                                               \
  void class_name::send_##name(param1_type param1_name,                        \
                               param2_type param2_name) {                      \
    service_struct data;                                                       \
    data.set_##param1_name(param1_name);                                       \
    data.set_##param2_name(param2_name);                                       \
    DECLARE_SND_MSG(message_type, service_type)                                \
  }

#define DECLARE_MEMBER_IMPL_ONE(class_name, name, message_type, service_type,  \
                                service_struct, param1_type, param1_name)      \
                                                                               \
  void class_name::send_##name(param1_type param1_name) {                      \
    service_struct data;                                                       \
    data.set_##param1_name(param1_name);                                       \
    DECLARE_SND_MSG(message_type, service_type)                                \
  }

#define DECLARE_MEMBER_IMPL_NONE(class_name, name, message_type, service_type) \
  void class_name::send_##name() {                                             \
    DECLARE_SND_NONE_MSG(message_type, service_type)                           \
  }

////////////////////////////////////////////////////////////////////////////////
// callback function implementation
////////////////////////////////////////////////////////////////////////////////

#define DECLARE_CALLBACK_IMPL_ONE(name, service_struct, param1_name)           \
  service_struct data;                                                         \
  data.ParseFromArray(message, message_size);                                  \
                                                                               \
  if (m_callback_##name) {                                                     \
    m_callback_##name(data.param1_name());                                     \
  }

#define DECLARE_CALLBACK_IMPL_TWO(name, service_struct, param1_name,           \
                                  param2_name)                                 \
  service_struct data;                                                         \
  data.ParseFromArray(message, message_size);                                  \
                                                                               \
  if (m_callback_##name) {                                                     \
    m_callback_##name(data.param1_name(), data.param2_name());                 \
  }

#define DECLARE_CALLBACK_IMPL_THREE(name, service_struct, param1_name,         \
                                    param2_name, param3_name)                  \
  service_struct data;                                                         \
  data.ParseFromArray(message, message_size);                                  \
                                                                               \
  if (m_callback_##name) {                                                     \
    m_callback_##name(data.param1_name(), data.param2_name(),                  \
                      data.param3_name());                                     \
  }

#define DECLARE_CALLBACK_IMPL_FOUR(name, service_struct, param1_name,          \
                                   param2_name, param3_name, param4_name)      \
  service_struct data;                                                         \
  data.ParseFromArray(message, message_size);                                  \
                                                                               \
  if (m_callback_##name) {                                                     \
    m_callback_##name(data.param1_name(), data.param2_name(),                  \
                      data.param3_name(), data.param4_name());                 \
  }

#define DECLARE_CALLBACK_IMPL_FIVE(name, service_struct, param1_name,          \
                                   param2_name, param3_name, param4_name,      \
                                   param5_name)                                \
  service_struct data;                                                         \
  data.ParseFromArray(message, message_size);                                  \
                                                                               \
  if (m_callback_##name) {                                                     \
    m_callback_##name(data.param1_name(), data.param2_name(),                  \
                      data.param3_name(), data.param4_name(),                  \
                      data.param5_name());                                     \
  }

#define DECLARE_CALLBACK_IMPL_SIX(name, service_struct, param1_name,           \
                                  param2_name, param3_name, param4_name,       \
                                  param5_name, param6_name)                    \
  service_struct data;                                                         \
  data.ParseFromArray(message, message_size);                                  \
                                                                               \
  if (m_callback_##name) {                                                     \
    m_callback_##name(data.param1_name(), data.param2_name(),                  \
                      data.param3_name(), data.param4_name(),                  \
                      data.param5_name(), data.param6_name());                 \
  }

#define DECLARE_CALLBACK_IMPL_EIGHT(                                           \
    name, service_struct, param1_name, param2_name, param3_name, param4_name,  \
    param5_name, param6_name, param7_name, param8_name)                        \
                                                                               \
  service_struct data;                                                         \
  data.ParseFromArray(message, message_size);                                  \
                                                                               \
  if (m_callback_##name) {                                                     \
    m_callback_##name(data.param1_name(), data.param2_name(),                  \
                      data.param3_name(), data.param4_name(),                  \
                      data.param5_name(), data.param6_name(),                  \
                      data.param7_name(), data.param8_name());                 \
  }

#define DECLARE_CALLBACK_IMPL_NINE(                                            \
    name, service_struct, param1_name, param2_name, param3_name, param4_name,  \
    param5_name, param6_name, param7_name, param8_name, param9_name)           \
                                                                               \
  service_struct data;                                                         \
  data.ParseFromArray(message, message_size);                                  \
                                                                               \
  if (m_callback_##name) {                                                     \
    m_callback_##name(                                                         \
        data.param1_name(), data.param2_name(), data.param3_name(),            \
        data.param4_name(), data.param5_name(), data.param6_name(),            \
        data.param7_name(), data.param8_name(), data.param9_name());           \
  }

#define DECLARE_CALLBACK_IMPL_TEN(name, service_struct, param1_name,           \
                                  param2_name, param3_name, param4_name,       \
                                  param5_name, param6_name, param7_name,       \
                                  param8_name, param9_name, param10_name)      \
                                                                               \
  service_struct data;                                                         \
  data.ParseFromArray(message, message_size);                                  \
                                                                               \
  if (m_callback_##name) {                                                     \
    m_callback_##name(data.param1_name(), data.param2_name(),                  \
                      data.param3_name(), data.param4_name(),                  \
                      data.param5_name(), data.param6_name(),                  \
                      data.param7_name(), data.param8_name(),                  \
                      data.param9_name(), data.param10_name());                \
  }

#define DECLARE_CALLBACK_IMPL_TWENTY_FOUR(                                     \
    name, service_struct, param1_name, param2_name, param3_name, param4_name,  \
    param5_name, param6_name, param7_name, param8_name, param9_name,           \
    param10_name, param11_name, param12_name, param13_name, param14_name,      \
    param15_name, param16_name, param17_name, param18_name, param19_name,      \
    param20_name, param21_name, param22_name, param23_name, param24_name)      \
                                                                               \
  service_struct data;                                                         \
  data.ParseFromArray(message, message_size);                                  \
                                                                               \
  if (m_callback_##name) {                                                     \
    m_callback_##name(                                                         \
        data.param1_name(), data.param2_name(), data.param3_name(),            \
        data.param4_name(), data.param5_name(), data.param6_name(),            \
        data.param7_name(), data.param8_name(), data.param9_name(),            \
        data.param10_name(), data.param11_name(), data.param12_name(),         \
        data.param13_name(), data.param14_name(), data.param15_name(),         \
        data.param16_name(), data.param17_name(), data.param18_name(),         \
        data.param19_name(), data.param20_name(), data.param21_name(),         \
        data.param22_name(), data.param23_name(), data.param24_name());        \
  }

#define DECLARE_CALLBACK_IMPL_NONE(name)                                       \
  if (m_callback_##name) {                                                     \
    m_callback_##name();                                                       \
  }

////////////////////////////////////////////////////////////////////////////////
// function declare and implementation
////////////////////////////////////////////////////////////////////////////////

#define IMPL_MEMBER_FUNC_NONE(name, inner)                                     \
  void send_##name() { inner.send_##name(); }

#define IMPL_MEMBER_FUNC_ONE(name, inner, param_type1, param_name1)            \
  void send_##name(param_type1 param_name1) { inner.send_##name(param_name1); }

#define IMPL_MEMBER_FUNC_TWO(name, inner, param_type1, param_name1,            \
                             param_type2, param_name2)                         \
  void send_##name(param_type1 param_name1, param_type2 param_name2) {         \
    inner.send_##name(param_name1, param_name2);                               \
  }

#define IMPL_MEMBER_FUNC_THREE(name, inner, param_type1, param_name1,          \
                               param_type2, param_name2, param_type3,          \
                               param_name3)                                    \
  void send_##name(param_type1 param_name1, param_type2 param_name2,           \
                   param_type3 param_name3) {                                  \
                                                                               \
    inner.send_##name(param_name1, param_name2, param_name3);                  \
  }

#define IMPL_MEMBER_FUNC_FOUR(name, inner, param_type1, param_name1,           \
                              param_type2, param_name2, param_type3,           \
                              param_name3, param_type4, param_name4)           \
  void send_##name(param_type1 param_name1, param_type2 param_name2,           \
                   param_type3 param_name3, param_type4 param_name4) {         \
                                                                               \
    inner.send_##name(param_name1, param_name2, param_name3, param_name4);     \
  }

#define IMPL_MEMBER_FUNC_FIVE(name, inner, param_type1, param_name1,           \
                              param_type2, param_name2, param_type3,           \
                              param_name3, param_type4, param_name4,           \
                              param_type5, param_name5)                        \
  void send_##name(param_type1 param_name1, param_type2 param_name2,           \
                   param_type3 param_name3, param_type4 param_name4,           \
                   param_type5 param_name5) {                                  \
                                                                               \
    inner.send_##name(param_name1, param_name2, param_name3, param_name4,      \
                      param_name5);                                            \
  }

#define IMPL_MEMBER_FUNC_SIX(                                                  \
    name, inner, param_type1, param_name1, param_type2, param_name2,           \
    param_type3, param_name3, param_type4, param_name4, param_type5,           \
    param_name5, param_type6, param_name6)                                     \
  void send_##name(param_type1 param_name1, param_type2 param_name2,           \
                   param_type3 param_name3, param_type4 param_name4,           \
                   param_type5 param_name5, param_type6 param_name6) {         \
                                                                               \
    inner.send_##name(param_name1, param_name2, param_name3, param_name4,      \
                      param_name5, param_name6);                               \
  }

#define IMPL_MEMBER_FUNC_EIGHT(                                                \
    name, inner, param_type1, param_name1, param_type2, param_name2,           \
    param_type3, param_name3, param_type4, param_name4, param_type5,           \
    param_name5, param_type6, param_name6, param_type7, param_name7,           \
    param_type8, param_name8)                                                  \
  void send_##name(param_type1 param_name1, param_type2 param_name2,           \
                   param_type3 param_name3, param_type4 param_name4,           \
                   param_type5 param_name5, param_type6 param_name6,           \
                   param_type7 param_name7, param_type8 param_name8) {         \
                                                                               \
    inner.send_##name(param_name1, param_name2, param_name3, param_name4,      \
                      param_name5, param_name6, param_name7, param_name8);     \
  }

#define IMPL_MEMBER_FUNC_NINE(                                                 \
    name, inner, param_type1, param_name1, param_type2, param_name2,           \
    param_type3, param_name3, param_type4, param_name4, param_type5,           \
    param_name5, param_type6, param_name6, param_type7, param_name7,           \
    param_type8, param_name8, param_type9, param_name9)                        \
  void send_##name(param_type1 param_name1, param_type2 param_name2,           \
                   param_type3 param_name3, param_type4 param_name4,           \
                   param_type5 param_name5, param_type6 param_name6,           \
                   param_type7 param_name7, param_type8 param_name8,           \
                   param_type9 param_name9) {                                  \
                                                                               \
    inner.send_##name(param_name1, param_name2, param_name3, param_name4,      \
                      param_name5, param_name6, param_name7, param_name8,      \
                      param_name9);                                            \
  }

#define IMPL_MEMBER_FUNC_TEN(                                                  \
    name, inner, param_type1, param_name1, param_type2, param_name2,           \
    param_type3, param_name3, param_type4, param_name4, param_type5,           \
    param_name5, param_type6, param_name6, param_type7, param_name7,           \
    param_type8, param_name8, param_type9, param_name9, param_type10,          \
    param_name10)                                                              \
  void send_##name(param_type1 param_name1, param_type2 param_name2,           \
                   param_type3 param_name3, param_type4 param_name4,           \
                   param_type5 param_name5, param_type6 param_name6,           \
                   param_type7 param_name7, param_type8 param_name8,           \
                   param_type9 param_name9, param_type10 param_name10) {       \
                                                                               \
    inner.send_##name(param_name1, param_name2, param_name3, param_name4,      \
                      param_name5, param_name6, param_name7, param_name8,      \
                      param_name9, param_name10);                              \
  }

#define IMPL_MEMBER_FUNC_TWENTY_FOUR(                                          \
    name, inner, param_type1, param_name1, param_type2, param_name2,           \
    param_type3, param_name3, param_type4, param_name4, param_type5,           \
    param_name5, param_type6, param_name6, param_type7, param_name7,           \
    param_type8, param_name8, param_type9, param_name9, param_type10,          \
    param_name10, param_type11, param_name11, param_type12, param_name12,      \
    param_type13, param_name13, param_type14, param_name14, param_type15,      \
    param_name15, param_type16, param_name16, param_type17, param_name17,      \
    param_type18, param_name18, param_type19, param_name19, param_type20,      \
    param_name20, param_type21, param_name21, param_type22, param_name22,      \
    param_type23, param_name23, param_type24, param_name24)                    \
  void send_##name(param_type1 param_name1, param_type2 param_name2,           \
                   param_type3 param_name3, param_type4 param_name4,           \
                   param_type5 param_name5, param_type6 param_name6,           \
                   param_type7 param_name7, param_type8 param_name8,           \
                   param_type9 param_name9, param_type10 param_name10,         \
                   param_type11 param_name11, param_type12 param_name12,       \
                   param_type13 param_name13, param_type14 param_name14,       \
                   param_type15 param_name15, param_type16 param_name16,       \
                   param_type17 param_name17, param_type18 param_name18,       \
                   param_type19 param_name19, param_type20 param_name20,       \
                   param_type21 param_name21, param_type22 param_name22,       \
                   param_type23 param_name23, param_type24 param_name24) {     \
                                                                               \
    inner.send_##name(param_name1, param_name2, param_name3, param_name4,      \
                      param_name5, param_name6, param_name7, param_name8,      \
                      param_name9, param_name10, param_name11, param_name12,   \
                      param_name13, param_name14, param_name15, param_name16,  \
                      param_name17, param_name18, param_name19, param_name20,  \
                      param_name21, param_name22, param_name23, param_name24); \
  }

////////////////////////////////////////////////////////////////////////////////
// function declare and callback implementation
////////////////////////////////////////////////////////////////////////////////

#define IMPL_CALLBACK_MEMBER_FUNC_NONE(name, inner)                            \
  void callback_##name(const std::function<void()> &f) {                       \
    inner.callback_##name(f);                                                  \
  }

#define IMPL_CALLBACK_MEMBER_FUNC_ONE(name, inner, param_type1, param_name1)   \
  void callback_##name(const std::function<void(param_type1)> &f) {            \
    inner.callback_##name(f);                                                  \
  }

#define IMPL_CALLBACK_MEMBER_FUNC_TWO(name, inner, param_type1, param_name1,   \
                                      param_type2, param_name2)                \
                                                                               \
  void callback_##name(                                                        \
      const std::function<void(param_type1, param_type2)> &f) {                \
    inner.callback_##name(f);                                                  \
  }

#define IMPL_CALLBACK_MEMBER_FUNC_THREE(name, inner, param_type1, param_name1, \
                                        param_type2, param_name2, param_type3, \
                                        param_name3)                           \
                                                                               \
  void callback_##name(                                                        \
      const std::function<void(param_type1, param_type2, param_type3)> &f) {   \
                                                                               \
    inner.callback_##name(f);                                                  \
  }

#define IMPL_CALLBACK_MEMBER_FUNC_FOUR(name, inner, param_type1, param_name1,  \
                                       param_type2, param_name2, param_type3,  \
                                       param_name3, param_type4, param_name4)  \
                                                                               \
  void callback_##name(                                                        \
      const std::function<void(param_type1, param_type2, param_type3,          \
                               param_type4)> &f) {                             \
                                                                               \
    inner.callback_##name(f);                                                  \
  }

#define IMPL_CALLBACK_MEMBER_FUNC_FIVE(name, inner, param_type1, param_name1,  \
                                       param_type2, param_name2, param_type3,  \
                                       param_name3, param_type4, param_name4,  \
                                       param_type5, param_name5)               \
                                                                               \
  void callback_##name(                                                        \
      const std::function<void(param_type1, param_type2, param_type3,          \
                               param_type4, param_type5)> &f) {                \
                                                                               \
    inner.callback_##name(f);                                                  \
  }

#define IMPL_CALLBACK_MEMBER_FUNC_SIX(                                         \
    name, inner, param_type1, param_name1, param_type2, param_name2,           \
    param_type3, param_name3, param_type4, param_name4, param_type5,           \
    param_name5, param_type6, param_name6)                                     \
                                                                               \
  void callback_##name(                                                        \
      const std::function<void(param_type1, param_type2, param_type3,          \
                               param_type4, param_type5, param_type6)> &f) {   \
                                                                               \
    inner.callback_##name(f);                                                  \
  }

#define IMPL_CALLBACK_MEMBER_FUNC_EIGHT(                                       \
    name, inner, param_type1, param_name1, param_type2, param_name2,           \
    param_type3, param_name3, param_type4, param_name4, param_type5,           \
    param_name5, param_type6, param_name6, param_type7, param_name7,           \
    param_type8, param_name8)                                                  \
                                                                               \
  void callback_##name(                                                        \
      const std::function<void(param_type1, param_type2, param_type3,          \
                               param_type4, param_type5, param_type6,          \
                               param_type7, param_type8)> &f) {                \
                                                                               \
    inner.callback_##name(f);                                                  \
  }

#define IMPL_CALLBACK_MEMBER_FUNC_NINE(                                        \
    name, inner, param_type1, param_name1, param_type2, param_name2,           \
    param_type3, param_name3, param_type4, param_name4, param_type5,           \
    param_name5, param_type6, param_name6, param_type7, param_name7,           \
    param_type8, param_name8, param_type9, param_name9)                        \
                                                                               \
  void callback_##name(                                                        \
      const std::function<void(param_type1, param_type2, param_type3,          \
                               param_type4, param_type5, param_type6,          \
                               param_type7, param_type8, param_type9)> &f) {   \
                                                                               \
    inner.callback_##name(f);                                                  \
  }

#define IMPL_CALLBACK_MEMBER_FUNC_TEN(                                         \
    name, inner, param_type1, param_name1, param_type2, param_name2,           \
    param_type3, param_name3, param_type4, param_name4, param_type5,           \
    param_name5, param_type6, param_name6, param_type7, param_name7,           \
    param_type8, param_name8, param_type9, param_name9, param_type10,          \
    param_name10)                                                              \
                                                                               \
  void callback_##name(const std::function<void(                               \
                           param_type1, param_type2, param_type3, param_type4, \
                           param_type5, param_type6, param_type7, param_type8, \
                           param_type9, param_type10)> &f) {                   \
                                                                               \
    inner.callback_##name(f);                                                  \
  }

#define IMPL_CALLBACK_MEMBER_FUNC_TWENTY_FOUR(                                 \
    name, inner, param_type1, param_name1, param_type2, param_name2,           \
    param_type3, param_name3, param_type4, param_name4, param_type5,           \
    param_name5, param_type6, param_name6, param_type7, param_name7,           \
    param_type8, param_name8, param_type9, param_name9, param_type10,          \
    param_name10, param_type11, param_name11, param_type12, param_name12,      \
    param_type13, param_name13, param_type14, param_name14, param_type15,      \
    param_name15, param_type16, param_name16, param_type17, param_name17,      \
    param_type18, param_name18, param_type19, param_name19, param_type20,      \
    param_name20, param_type21, param_name21, param_type22, param_name22,      \
    param_type23, param_name23, param_type24, param_name24)                    \
                                                                               \
  void callback_##name(                                                        \
      const std::function<void(                                                \
          param_type1, param_type2, param_type3, param_type4, param_type5,     \
          param_type6, param_type7, param_type8, param_type9, param_type10,    \
          param_type11, param_type12, param_type13, param_type14,              \
          param_type15, param_type16, param_type17, param_type18,              \
          param_type19, param_type20, param_type21, param_type22,              \
          param_type23, param_type24)> &f) {                                   \
                                                                               \
    inner.callback_##name(f);                                                  \
  }

////////////////////////////////////////////////////////////////////////////////
// function recv_data implementation
////////////////////////////////////////////////////////////////////////////////

#define RECV_DATA_IMPL(class_name, header_type)                                \
  void class_name::recv_data(const char *data, size_t data_size) {             \
    if (data_size == 0 || data == nullptr) {                                   \
      buff_size_ = 0;                                                          \
      return;                                                                  \
    }                                                                          \
                                                                               \
    memcpy(buff_ + buff_size_, data, data_size);                               \
    buff_size_ += data_size;                                                   \
    const header_type &header = transfer_from_head(buff_);                     \
    if (buff_size_ == data_size) { /* first enter*/                            \
      header_type *h = (header_type *)&header;                                 \
      h->service_type = (decltype(h->service_type))sockets::net_to_host(       \
          (uint32_t)(h->service_type));                                        \
      h->length = sockets::net_to_host(h->length);                             \
    }                                                                          \
    if (buff_size_ < sizeof(header_type) ||                                    \
        header.length > buff_size_ - sizeof(header_type)) {                    \
      return;                                                                  \
    }                                                                          \
    size_t remain = 0, data_offset = 0;                                        \
    if (header.length < buff_size_ - sizeof(header_type)) {                    \
      remain = buff_size_ - sizeof(header_type) - header.length;               \
      buff_size_ -= remain;                                                    \
      data_offset = data_size - remain;                                        \
    }                                                                          \
    /*LOG_DEBUG("parse recv size %d, header length %d header size %d",         \
              buff_size_, header.length, sizeof(header_type));*/               \
                                                                               \
    parse_message(buff_, buff_size_);                                          \
    buff_size_ = 0;                                                            \
    if (remain) {                                                              \
      recv_data(data + data_offset, remain);                                   \
    }                                                                          \
  }
////////////////////////////////////////////////////////////////////////////////
// function parse_message head implementation
////////////////////////////////////////////////////////////////////////////////

#define PARSE_MESSAGE_HEAD_IMPL(message_header)                                \
  const message_header &header = transfer_from_head(data);                     \
  const char *message = data + sizeof(message_header);                         \
  const size_t message_size = data_size - sizeof(message_header);              \
                                                                               \
  if (header.length != message_size) {                                         \
    LOG_ERROR("message size %d is error, header length is %d!", message_size,  \
              header.length);                                                  \
    return;                                                                    \
  }                                                                            \
  switch (header.service_type) {                                               \
  case MSG_VIDEO_DATA:                                                         \
  case MSG_VR_DATA:                                                            \
  case MSG_VR_MIC_DATA:                                                        \
  case MSG_TTS_DATA:                                                           \
  case MSG_MEDIA_DATA:                                                         \
    LOG_TRACE("data size %d, header data length %d, service type 0x%08X",      \
              data_size, header.length, header.service_type);                  \
    break;                                                                     \
  default:                                                                     \
    LOG_INFO("data size %d, header data length %d, service type 0x%08X",       \
             data_size, header.length, header.service_type);                   \
    break;                                                                     \
  }

#endif // CARLIFE_CHANNEL_CHANNELBASEMACRO_H
