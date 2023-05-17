
#ifndef AV_ENCODER_H
#define AV_ENCODER_H

#include <cstdint>
#include <map>
#include <vector>

struct AVCodecContext;
struct AVFrame;
struct SwsContext;

class Encoder {
public:
  Encoder();
  ~Encoder();

  /**
   * @param encoder_lib library name, libx264/libx265...
   * @param options option parameter, libx264 has <preset,slow/ultrafast/..>
   * <tune,zerolatency> ..
   */
  int init(int width, int height, int framerate, int gop_size, int64_t bitrate,
           const char *encoder_lib,
           const std::map<const char *, const char *> &options);

  /**
   * @param image input image format is bgra
   */
  void encoder(const std::vector<uint8_t> &image, uint32_t width,
               uint32_t height, std::vector<uint8_t> &output);

private:
  int recv(std::vector<uint8_t> &output);

  int scale(AVFrame *input, AVFrame *output);

  void destroy();

  uint32_t sws_width_;
  uint32_t sws_height_;
  uint32_t in_max_width_;
  uint32_t in_max_height_;
  int64_t pts_;

  AVCodecContext *codec_context_;
  SwsContext *sws_context_;

  AVFrame *in_frame_;
  AVFrame *out_frame_;

}; // class Encoder

#endif // AV_ENCODER_H
