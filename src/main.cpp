
#include <stdio.h>

#include <thread>

#include "HeadUnit.h"
#include "MobileDevice.h"
#include "TransferPort.h"
#include "VideoService.h"
#include "av/CaptureAudioWasapi.h"
#include "av/Decoder.h"
#include "carlife/AOA.h"
#include "lock_free/thread_pool.h"
#include "proactor/Proactor.h"
#include "utils/log.h"

#include <google/protobuf/stubs/common.h>

void md_service(int args, char **argv) {
  std::error_code ec;
  Proactor p(ec);
  if (ec) {
    LOG_ERROR("Proactor start error");
    ec.clear();
  }

  MobileDevice md(p);
  HeadUnit hu(p);
  VideoService v(p);

  if (args > 1 && memcmp(argv[1], "hu", 2) == 0) {
    hu.start();
  } else if (args > 1 && memcmp(argv[1], "md", 2) == 0) {
    md.start();
  } else {
    FILE *ff = fopen("view_capture.h264", "wb");
    uint64_t count = 0;
    v.init_video(v.screen_width(), v.screen_height(), 25, 4000000);
    v.register_send(
        [&ff, &count, &v, &p](const std::vector<unsigned char> &data) {
          ++count;
          fwrite(data.data(), 1, data.size(), ff);

          if (count % 20 == 0) {
            LOG_TRACE("data write end %d", count);
          }
          if (count == 100) {
            v.init_video(800, 600, 25, 4000000);
          }
          if (count >= 200) {
            fclose(ff);
            v.stop();
            p.shutdown();
          }
        });
    v.start(40);
  }

  // TransferPort tp(p);
  // tp.start("192.168.50.51", "127.0.0.1", false); // CarlifeHost

  thread::thread_pool pool;
  size_t size = pool.thread_size();
  for (size_t i = 0; i < size; ++i) {
    pool.submit([&p]() { LOG_INFO("exit %d", p.run()); });
  }
  LOG_INFO("exit %d", p.run());
}

void decoder() {
  Decoder dec;
  if (dec.init("h264") < 0) {
    LOG_ERROR("decoder error");
    return;
  }

  FILE *ff = fopen("view_capture.h264", "rb");
  if (!ff) {
    LOG_ERROR("fopen error");
    return;
  }
  char buff[4096];
  int ret = 0;
  int i = 0;
  auto save_pic = [&dec, &buff, &ret, &i]() {
    std::vector<std::vector<uint8_t>> raw;
    uint32_t width = 0, height = 0;
    dec.decoder(buff, ret, raw, width, height);
    if (raw.size() > 0) {
      LOG_TRACE("ret %d get picture size %d width %d height %d", ret,
                raw.size(), width, height);
      for (auto &i_raw : raw) {
        LOG_TRACE("get index %d picture size %d", i, i_raw.size());
        char name[1024] = {};
        sprintf(name, "capture%03d.yuv", i);
        FILE *out = fopen(name, "wb");
        if (!out) {
          LOG_ERROR("open %s error", name);
          continue;
        }
        fwrite(i_raw.data(), 1, i_raw.size(), out);
        fclose(out);
        ++i;
      }
    }
  };
  while ((ret = fread(buff, 1, sizeof(buff), ff)) > 0) {
    save_pic();
    if (ret != sizeof(buff)) {
      LOG_TRACE("ret %d", ret);
    }
  }
  save_pic();

  fclose(ff);
  LOG_TRACE("exit %d", ret);
}

int main(int args, char **argv) {
  /*
  CaptureAudioWasapi was;
  was.start();
  // unsigned char *buff = nullptr;
  // size_t buff_size = 0;
  int ret = 0;
  FILE *ff = fopen("was.pcm", "w");
  if (!ff) {
    printf("error open file\n");
    return -1;
  }
  int i = 0;
  std::vector<unsigned char> audio;
  while ((ret = was.capture(audio)) >= 0) {
    printf("write pcm %d\n", audio.size());
    fwrite(audio.data(), 1, audio.size(), ff);
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    if (++i > 500) {
      break;
    }
  }
  fclose(ff);
  if (ret < 0) {
    printf("capture error\n");
    return -1;
  }
  return 0;
  */
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  // setLogLevel(kInfo);
  // AOA a;
  // auto list = a.list_aoa_device();
  // LOG_TRACE("list size %d", list.size());
  // if (list.size() > 0) {
  //   a.connect(list[0]);
  //   a.write("..........................", 27);
  //   char buff[1024] = {};
  //   a.read(buff, sizeof(buff));
  //   LOG_TRACE("read buff %s", buff);
  // }

  md_service(args, argv);
  // decoder();

  // Optional:  Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();
  return 0;
}