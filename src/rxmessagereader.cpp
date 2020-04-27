#include "rxmessagereader.h"

static constexpr uint8_t b1[] = {9};
static constexpr uint8_t b2[] = {0x0c};

static constexpr const uint8_t *const d[] = {b1, b2};

int32_t RxMessageReader::getActualMessageSize(pb_istream_t *isteram) {
  uint64_t res;
  return pb_decode_varint(isteram, &res) ? (res & 0xffffffff) : -1;
}

void RxMessageReader::readTo(uint8_t *&dest, size_t &count) {
  auto to_read = std::min<size_t>(remaning, count);

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
      // сидим в этом цыкле пока не придут какие-нибудь данные
      while (!argument->_this->tryRefill()) {
        argument->wait_fun();
      }
    }
  }

  return true;
}
