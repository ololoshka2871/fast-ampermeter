#ifndef CDC_ACM_H
#define CDC_ACM_H

#include "result.h"

#include "usbd_cdc.h"

struct CDC_ACM {
public:
  struct RxData {
    RxData(const uint32_t buf_size = 0) : buf_size(buf_size) {}

    RxData(const RxData &) = delete;
    RxData(RxData &&) = default;

    ~RxData();

    RxData &operator=(RxData &&rr);

    uint8_t *pData() const;
    uint32_t size() const { return buf_size; }

  private:
    uint32_t buf_size;

    static void release_buf();
  };

  struct TxBuffer {
    TxBuffer() : writen(0) {}

    bool write(uint8_t byte);
    size_t write(uint8_t *buf, size_t count);
    bool isFull() const;

    void send();

  private:
    static uint8_t UserTxBuffer[CDC_DATA_FS_MAX_PACKET_SIZE];
    uint16_t writen;
  };

  static void init();

  static RxData tryReadData();

  static Result<TxBuffer *, int> allocateTxBuffer();
};

#endif // CDC_ACM_H
