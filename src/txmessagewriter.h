#ifndef TXMESSAGEWRITER_H
#define TXMESSAGEWRITER_H

template <typename T> struct TxMessageWriter {
  TxMessageWriter() {}

  template <typename U> void write(const T &msg, const U &wait_fun) {
    wait_fun();
  }
};

#endif // TXMESSAGEWRITER_H
