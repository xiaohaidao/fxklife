
#ifndef AV_DECODER_H
#define AV_DECODER_H

#include <cstdint>
#include <map>
#include <vector>

struct AVCodecContext;
struct AVCodecParserContext;
struct AVFrame;
struct AVPacket;

class Decoder {
public:
  Decoder();
  ~Decoder();

  /**
   * @param encoder_lib library name, h264...
   *
   */
  int init(const char *decoder_lib);

  void decoder(const char *data, uint32_t data_size,
               std::vector<std::vector<uint8_t>> &output, uint32_t &width,
               uint32_t &height);

private:
  int recv(std::vector<std::vector<uint8_t>> &output, uint32_t &width,
           uint32_t &height);

  void destroy();

  AVCodecContext *codec_context_;
  AVCodecParserContext *parser_;

  AVPacket *pkg_;
  AVFrame *frame_;

}; // class Decoder

#endif // AV_DECODER_H
