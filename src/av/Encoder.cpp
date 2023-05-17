
#include "av/Encoder.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"
#include "libswscale/swscale.h"
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

Encoder::Encoder()
    : sws_width_(0), sws_height_(0), pts_(0), in_max_width_(0),
      in_max_height_(0), codec_context_(nullptr), sws_context_(nullptr),
      in_frame_(nullptr), out_frame_(nullptr) {}

Encoder::~Encoder() { destroy(); }

int Encoder::init(int width, int height, int framerate, int gop_size,
                  int64_t bitrate, const char *encoder_lib,
                  const std::map<const char *, const char *> &option) {

  destroy();

  const AVCodec *codec = avcodec_find_encoder_by_name(encoder_lib);
  if (!codec) {
    LOG_ERROR("avcodec_find_encoder_by_name %s error", encoder_lib);
    return -1;
  }

  codec_context_ = avcodec_alloc_context3(codec);
  if (!codec_context_) {
    LOG_ERROR("avcodec_alloc_context3 error");
    return -1;
  }

  if (width % 2 == 1) {
    width -= 1;
  }
  if (height % 2 == 1) {
    height -= 1;
  }

  codec_context_->width = width;
  codec_context_->height = height;
  codec_context_->time_base = {1, framerate};
  codec_context_->framerate = {framerate, 1};
  codec_context_->gop_size = gop_size;
  codec_context_->max_b_frames = 0;
  codec_context_->pix_fmt = AV_PIX_FMT_YUV420P;

  // rc control mode: abr
  codec_context_->bit_rate = bitrate;

  // cbr mode config
  // codec_context_->rc_min_rate = bitrate;
  // codec_context_->rc_max_rate = bitrate;
  // codec_context_->rc_buffer_size = (int)bitrate;

  // codec_context_->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

  // if (codec->id == AV_CODEC_ID_H264) {
  //   av_opt_set(codec_context_->priv_data, "preset", "slow",
  //              0); // ultrafast
  // }

  // av_opt_set(codec_context_->priv_data, "tune", "zerolatency", 0);
  // av_opt_set_int(codec_context_->priv_data, "forced-idr", 1, 0);
  // av_opt_set(codec_context_->priv_data, "avcintra-class", "-1", 0);
  for (auto &op : option) {
    av_opt_set(codec_context_->priv_data, op.first, op.second, 0);
  }

  int ret = 0;
  if ((ret = avcodec_open2(codec_context_, codec, NULL)) != 0) {
    LOG_ERROR("avcodec_open2 error %d:%s", ret, av_err2str(ret));
    return ret;
  }

  if (!in_frame_) {
    in_frame_ = av_frame_alloc();
    out_frame_ = av_frame_alloc();
  }

  // initialize in_frame_
  in_frame_->width = codec_context_->width;
  in_frame_->height = codec_context_->height;
  in_frame_->format = AV_PIX_FMT_BGRA;
  in_max_width_ = width;
  in_max_height_ = height;

  if ((ret = av_frame_get_buffer(in_frame_, 0)) != 0) {
    LOG_ERROR("av_frame_get_buffer error %d %s", ret, av_err2str(ret));
    return ret;
  }

  // initialize out_frame_
  out_frame_->width = codec_context_->width;
  out_frame_->height = codec_context_->height;
  out_frame_->format = codec_context_->pix_fmt; // YUV420
  if ((ret = av_frame_get_buffer(out_frame_, 0) != 0)) {
    LOG_ERROR("av_frame_get_buffer error %d %s", ret, av_err2str(ret));
    return ret;
  }
  return 0;
}

void Encoder::encoder(const std::vector<uint8_t> &image, uint32_t width,
                      uint32_t height, std::vector<uint8_t> &output) {

  output.clear();
  if (!codec_context_) {
    LOG_ERROR("uninitialized codec context");
    return;
  }
  if (image.size() == 0) {
    int ret = avcodec_send_frame(codec_context_, NULL); // success ? 0 : !0;
    if (ret < 0) {
      LOG_ERROR("avcodec_send_frame error %d %s", ret, av_err2str(ret));
      return;
    }
    recv(output);
    return;
  }
  if (av_image_get_buffer_size(AV_PIX_FMT_BGRA, width, height, 32) !=
      image.size()) {

    LOG_ERROR("image data format is different from pix_fmt of codec context, "
              "width %d hegith %d size %d",
              width, height, image.size());
    return;
  }

  // get out_frame
  AVFrame &out_frame = *out_frame_;
  AVFrame &in_frame = *in_frame_;

  in_frame.width = width;
  in_frame.height = height;

  int ret = 0;
  if (in_max_width_ * in_max_height_ < width * height) {
    // resize the in_frame
    av_frame_unref(&in_frame);
    in_frame.width = width;
    in_frame.height = height;
    in_frame.format = AV_PIX_FMT_BGRA;
    if ((ret = av_frame_get_buffer(&in_frame, 0)) != 0) {
      LOG_ERROR("av_frame_get_buffer error %d %s", ret, av_err2str(ret));
      return;
    }
    in_max_width_ = width;
    in_max_height_ = height;
  }

  memcpy(in_frame.data[0], image.data(), image.size());

  if ((ret = av_frame_make_writable(&out_frame)) < 0) {
    LOG_ERROR("av_frame_make_writable error %d %s", ret, av_err2str(ret));
    return;
  }

  // create scaling
  if ((ret = scale(&in_frame, &out_frame) < 0)) {
    return;
  }

  out_frame.pts = pts_++;
  // out_frame.pict_type = AV_PICTURE_TYPE_NONE; // AV_PICTURE_TYPE_I

  // send and encoder
  if ((ret = avcodec_send_frame(codec_context_, &out_frame) < 0)) {
    LOG_ERROR("avcodec_send_frame error %d %s", ret, av_err2str(ret));
    return;
  }
  recv(output);
}

int Encoder::recv(std::vector<uint8_t> &output) {
  AVPacket av_packet = {};
  int ret = 0;
  while ((ret = avcodec_receive_packet(codec_context_, &av_packet)) >= 0) {
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
      LOG_ERROR("avcodec_receive_packet error %d %s", ret, av_err2str(ret));
      ret = 0;
      break;
    } else if (ret < 0) {
      LOG_ERROR("avcodec_receive_packet error %d %s", ret, av_err2str(ret));
      break;
    }
    output.insert(output.cend(), av_packet.data,
                  av_packet.data + av_packet.size);
  }

  av_packet_unref(&av_packet);
  return ret;
}

int Encoder::scale(AVFrame *input, AVFrame *output) {
  if (!(input->width != output->width || input->height != output->height ||
        input->format != output->format)) {

    // memcpy(output->data[0], input->buf[0]->data, input->buf[0]->size);
    av_frame_copy(output, input);
    return 0;
  }
  if (sws_context_ &&
      (input->height != sws_height_ || input->width != sws_width_)) {
    sws_freeContext(sws_context_);
    sws_context_ = nullptr;
  }
  sws_height_ = input->height;
  sws_width_ = input->width;

  if (!sws_context_) {
    sws_context_ = sws_getContext(
        input->width, input->height, (AVPixelFormat)(input->format),
        output->width, output->height, (AVPixelFormat)(output->format),
        SWS_BICUBIC, NULL, NULL, NULL);
  }
  int ret = sws_scale(sws_context_, input->data, input->linesize, 0,
                      input->height, output->data, output->linesize);
  if (ret < 0) {
    LOG_ERROR("sws_scale error %d %s", ret, av_err2str(ret));
  }
  return ret;
}

void Encoder::destroy() {
  if (codec_context_) {
    avcodec_close(codec_context_);
    avcodec_free_context(&codec_context_);
  }
  if (sws_context_) {
    sws_freeContext(sws_context_);
    sws_context_ = nullptr;
  }
  if (in_frame_) {
    av_frame_free(&in_frame_);
    av_frame_free(&out_frame_);
  }
  pts_ = 0;
}