#ifndef TXMESSAGEWRITER_H
#define TXMESSAGEWRITER_H

#include <functional>

#include "cdc_acm.h"

#include "pb_encode.h"

struct TxMessageWriter {
  using callback_type = std::function<void()>;

  TxMessageWriter() : tx_buf(nullptr) {}

  template <typename T, typename F, typename M>
  void write(const T &msg, const F *fields, const M &magick,
             const callback_type &wait_fun) {
    Argument arg{this, wait_fun};

    pb_ostream_t output_stream{TxCallback, &arg, SIZE_MAX, 0};

    if (!encode_magick(output_stream, magick)) {
      return;
    }
    pb_encode_delimited(&output_stream, fields, &msg);

    send();
  }

private:
  struct Argument {
    TxMessageWriter *_this;
    const callback_type &wait_fun;
  };

  CDC_ACM::TxBuffer *tx_buf;

  template <typename M>
  auto encode_magick(pb_ostream_t &stream, const M &magick) {
    return pb_encode_varint(&stream, magick);
  }

  bool writeTxBuf(pb_byte_t *&buf, size_t &count);

  static bool TxCallback(pb_ostream_t *stream, const pb_byte_t *buf,
                         size_t count);

  void send();
};

#endif // TXMESSAGEWRITER_H
