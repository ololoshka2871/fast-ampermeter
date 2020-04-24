#include "txmessagewriter.h"

bool TxMessageWriter::writeTxBuf(pb_byte_t *&buf, size_t &count) {
  auto writen = tx_buf->write(buf, count);
  count -= writen;
  buf += writen;
  return count != 0;
}

bool TxMessageWriter::TxCallback(pb_ostream_t *stream, const pb_byte_t *buf,
                                 size_t count) {
  const auto argument = static_cast<Argument *>(stream->state);

  pb_byte_t *b = const_cast<pb_byte_t *>(buf);

  while (count) {
    if (argument->_this->tx_buf) {
      if (argument->_this->writeTxBuf(b, count)) {
        // full
        argument->_this->send();
      } else {
        continue;
      }
    }
    // сидим в этом цыкле пока не будут доступен буфер на передачу
    while (true) {
      auto res = CDC_ACM::allocateTxBuffer();
      if (res.isOk()) {
        argument->_this->tx_buf = res.unwrap();
        break;
      }
      argument->wait_fun();
    }
  }

  return true;
}

void TxMessageWriter::send() {
  tx_buf->send();
  tx_buf = nullptr;
}
