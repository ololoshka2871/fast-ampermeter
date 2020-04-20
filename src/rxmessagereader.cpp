#include "rxmessagereader.h"

int32_t RxMessageReader::getActualMessageSize(pb_istream_t *isteram) {
  uint64_t res;
  return pb_decode_varint(isteram, &res) ? (res & 0xffffffff) : -1;
}

void RxMessageReader::readTo(uint8_t *&dest, size_t &count) {
  auto to_read = std::max<size_t>(remaning, count);

  std::memcpy(dest, rp, to_read);

  rp += to_read;
  remaning -= to_read;
  dest += to_read;
  count -= to_read;
}

bool RxMessageReader::tryRefill() {
  RxDataBuf = std::move(CDC_ACM::tryReadData());
  if (RxDataBuf.size()) {
    rp = RxDataBuf.pData();
    remaning = RxDataBuf.size();
    return true;
  }
  return false;
}

bool RxMessageReader::RxCallback(pb_istream_t *stream, uint8_t *buf,
                                 size_t count) {
  auto argument = static_cast<Argument *>(stream->state);

  while (count) {
    if (argument->_this->remaning) {
      argument->_this->readTo(buf, count);
    } else {
      while (!argument->_this->tryRefill()) {
        argument->wait_fun();
      }
    }
  }

  return true;
}
