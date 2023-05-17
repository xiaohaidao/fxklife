
#include "av/Decoder.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
}

#include "utils/log.h"

#ifdef av_err2str
#undef av_err2str
av_always_inline char *av_err2str(int errnum) {
  // static char str[AV_ERROR_MAX_STRING_SIZE];
  // thread_local may be better than static in multi-thread circumstance
  thread_local char str[AV_ERROR_MAX_STRING_SIZE];
  memset(str, 0, sizeof(str));
  return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}
#endif

Decoder::Decoder()
    : codec_context_(nullptr), parser_(nullptr), pkg_(nullptr),
      frame_(nullptr) {}

Decoder::~Decoder() { destroy(); }

int Decoder::init(const char *decoder_lib) {
  destroy();

  const AVCodec *codec = avcodec_find_decoder_by_name(decoder_lib);
  if (!codec) {
    LOG_ERROR("avcodec_find_encoder_by_name %s error", decoder_lib);
    return -1;
  }

  parser_ = av_parser_init(codec->id);
  if (!parser_) {
    LOG_ERROR("av_parser_init %d error", codec->id);
    return -1;
  }

  codec_context_ = avcodec_alloc_context3(codec);
  if (!codec_context_) {
    LOG_ERROR("avcodec_alloc_context3 error");
    return -1;
  }

  int ret = 0;
  if ((ret = avcodec_open2(codec_context_, codec, NULL)) != 0) {
    LOG_ERROR("avcodec_open2 error %d:%s", ret, av_err2str(ret));
    return ret;
  }

  if (!pkg_) {
    pkg_ = av_packet_alloc();
    frame_ = av_frame_alloc();
  }

  return 0;
}

void Decoder::decoder(const char *data, uint32_t data_size,
                      std::vector<std::vector<uint8_t>> &output,
                      uint32_t &width, uint32_t &height) {

  output.clear();
  if (!codec_context_) {
    LOG_ERROR("uninitialized codec context");
    return;
  }
  if (!parser_) {
    LOG_ERROR("uninitialized parser context");
    return;
  }
  if (data_size == 0 || data == nullptr) {
    int ret = avcodec_send_packet(codec_context_, NULL); // success ? 0 : !0;
    if (ret < 0) {
      LOG_ERROR("avcodec_send_packet error %d %s", ret, av_err2str(ret));
      return;
    }
    recv(output, width, height);
    return;
  }

  const uint8_t *en_data = (const uint8_t *)data;
  int en_data_size = data_size;
  while (en_data_size > 0) {
    int ret = av_parser_parse2(parser_, codec_context_, &pkg_->data,
                               &pkg_->size, en_data, en_data_size,
                               AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
    if (ret < 0) {
      LOG_ERROR("parser error %d:%s", ret, av_err2str(ret));
      return;
    }
    en_data += ret;
    en_data_size -= ret;

    if (pkg_->size) {
      // send and encoder
      int ret = 0;
      if ((ret = avcodec_send_packet(codec_context_, pkg_) < 0)) {
        LOG_ERROR("avcodec_send_packet error %d %s", ret, av_err2str(ret));
        return;
      }
      recv(output, width, height);
    }
  }
}

int Decoder::recv(std::vector<std::vector<uint8_t>> &output, uint32_t &width,
                  uint32_t &height) {

  AVFrame &av_frame = *frame_;
  int ret = 0;
  while ((ret = avcodec_receive_frame(codec_context_, &av_frame)) >= 0) {
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
      LOG_ERROR("avcodec_receive_packet error %d %s", ret, av_err2str(ret));
      ret = 0;
      break;
    } else if (ret < 0) {
      LOG_ERROR("avcodec_receive_packet error %d %s", ret, av_err2str(ret));
      break;
    }
    width = av_frame.width;
    height = av_frame.height;
    std::vector<uint8_t> o;
    o.resize(av_image_get_buffer_size((AVPixelFormat)av_frame.format,
                                      av_frame.width, av_frame.height, 32));

    if (av_image_copy_to_buffer(
            o.data(), o.size(), (const uint8_t *const *)av_frame.data,
            (const int *)av_frame.linesize, (AVPixelFormat)av_frame.format,
            av_frame.width, av_frame.height, 1) < 0) {
      LOG_ERROR("image copy error");
    }

    output.emplace_back(std::move(o));
  }

  return ret;
}

void Decoder::destroy() {
  if (codec_context_) {
    avcodec_close(codec_context_);
    avcodec_free_context(&codec_context_);
  }
  if (parser_) {
    av_parser_close(parser_);
    parser_ = nullptr;
  }
  if (pkg_) {
    av_packet_free(&pkg_);
  }
  if (frame_) {
    av_frame_free(&frame_);
  }
}