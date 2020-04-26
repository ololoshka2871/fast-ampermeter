#ifndef RXMESSAGEREADER_H
#define RXMESSAGEREADER_H

#include <cstring>
#include <functional>
#include <limits>

#include "cdc_acm.h"

#include "pb_decode.h"

struct RxMessageReader {
  using callback_type = std::function<void()>;

  RxMessageReader() : remaning(0) {}

  template <typename T, typename F, typename M>
  void read(T &msg, const F *fields, const M &magick,
            const callback_type &wait_fun) {
    Argument arg{this, wait_fun};

    pb_istream_t input_stream{RxCallback, &arg,
                              std::numeric_limits<size_t>::max()};

    // бесконечно пытаемся прочитать валидное сообщение
    while (true) {
      if (checkMagick(&input_stream, magick)) {
        auto size = getActualMessageSize(&input_stream);
        if (size > 10 && size < 100) {
          input_stream.bytes_left = size;
          if (pb_decode(&input_stream, fields, &msg)) {
            break;
          }
        }
      }
      input_stream.bytes_left = std::numeric_limits<size_t>::max();
    }
  }

private:
  struct Argument {
    RxMessageReader *_this;
    const callback_type &wait_fun;
  };

  CDC_ACM::RxData RxDataBuf;
  uint8_t *rp;
  uint32_t remaning;

  template <typename M>
  static bool checkMagick(pb_istream_t *isteram, const M &magick) {
    uint64_t v;
    return pb_decode_varint(isteram, &v) && (v == magick);
  }

  static int32_t getActualMessageSize(pb_istream_t *isteram);

  void readTo(uint8_t *&dest, size_t &count);
  bool tryRefill();
  static bool RxCallback(pb_istream_t *stream, uint8_t *buf, size_t count);
};

#endif // RXMESSAGEREADER_H
