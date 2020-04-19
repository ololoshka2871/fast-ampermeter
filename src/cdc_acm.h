#ifndef CDC_ACM_H
#define CDC_ACM_H

struct CDC_ACM {
public:
  struct RxData {
    RxData(const uint32_t buf_size) : buf_size(buf_size) {}

    RxData(const RxData &) = delete;
    RxData(RxData &&) = default;

    ~RxData();

    constexpr uint8_t *pData() const;
    uint32_t size() const { return buf_size; }

  private:
    uint32_t buf_size;
  };

  static void init();

  static RxData tryReadData();
};

#endif // CDC_ACM_H
