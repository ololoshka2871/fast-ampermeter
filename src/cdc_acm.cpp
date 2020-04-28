#include <algorithm>
#include <cstring>

#include "hw_includes.h"

#include "cdc_acm.h"

#include "usbd_core.h"

#include "usbd_desc.h"

#include "protocol.pb.h"

#include "usb_cdc_interface.h"

static constexpr auto txBufSize =
    std::min<size_t>(CDC_DATA_FS_MAX_PACKET_SIZE,
                     ru_sktbelpa_fast_freqmeter_SingleMeasure_size
                         *HISTORY_SIZE // Слишком много?
    );

static uint8_t UserTxBuffer[txBufSize];

uint8_t *CDC_ACM::TxBuffer::pUserTxBuffer = UserTxBuffer;

static CDC_ACM::TxBuffer txBuf;

void CDC_ACM::init() {
  extern USBD_CDC_ItfTypeDef USBD_CDC_fops;

  /* Init Device Library */
  USBD_Init(&USBD_Device, &VCP_Desc, 0);

  /* Add Supported Class */
  USBD_RegisterClass(&USBD_Device, &USBD_CDC);

  /* Add CDC Interface Class */
  USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);

  /* Start Device Process */
  USBD_Start(&USBD_Device);
}

CDC_ACM::RxData CDC_ACM::tryReadData() {
  RxData res(usb_data_ressived);
  usb_data_ressived = 0;
  return res;
}

Result<CDC_ACM::TxBuffer *, int> CDC_ACM::allocateTxBuffer() {
  auto hcdc = static_cast<USBD_CDC_HandleTypeDef *>(USBD_Device.pClassData);

  return hcdc->TxState == 0U ? Result<CDC_ACM::TxBuffer *, int>(Ok(&txBuf))
                             : Err((int)USBD_BUSY);
}

CDC_ACM::RxData::~RxData() {
  // Говорим драйверу, что мы закончили работу с буфером и можно его освободить
  if (size()) {
    release_buf();
  }
}

CDC_ACM::RxData &CDC_ACM::RxData::operator=(CDC_ACM::RxData &&rr) {
  if (!(size() == 0 && rr.size() != 0)) {
    release_buf();
  }
  buf_size = rr.buf_size;
  rr.buf_size = 0;
  return *this;
}

uint8_t *CDC_ACM::RxData::pData() const { return UserRxBuffer; }

void CDC_ACM::RxData::release_buf() { USBD_CDC_ReceivePacket(&USBD_Device); }

bool CDC_ACM::TxBuffer::write(uint8_t byte) {
  if (isFull()) {
    std::terminate();
  }

  pUserTxBuffer[writen++] = byte;
  return isFull();
}

size_t CDC_ACM::TxBuffer::write(uint8_t *buf, size_t count) {
  auto to_write = std::min<size_t>(txBufSize - 1 - writen, count);

  std::memcpy(&pUserTxBuffer[writen], buf, to_write);
  writen += to_write;

  return to_write;
}

bool CDC_ACM::TxBuffer::isFull() const { return writen >= txBufSize; }

void CDC_ACM::TxBuffer::send() {
  USBD_CDC_SetTxBuffer(&USBD_Device, pUserTxBuffer, writen);

  USBD_CDC_TransmitPacket(&USBD_Device);

  writen = 0;
}
