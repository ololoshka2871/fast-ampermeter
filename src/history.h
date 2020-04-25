#ifndef HISTORY_H
#define HISTORY_H

#include <array>
#include <cstdlib>
#include <tuple>

template <size_t size> struct History {
  struct HistoryElement {
    float voltage;
    float current;
  };

  History() : historyData() { reset(); }

  void add(int64_t &pos, float voltage, float current) {
    writen = pos;
    auto wp = pos % size;

    auto &he = historyData.at(wp);

    he.voltage = voltage;
    he.current = current;
  }

  void reset() { writen = 0; }

  int64_t start() const { return writen > size ? writen - size : 0; }
  int32_t elements() const { return writen > size ? size : writen; }

  std::pair<int64_t, HistoryElement *const> getLastMeasure() {
    if (writen) {
      auto rp = (writen - 1) % size;
      return std::pair(writen, &historyData[rp]);
    }
    return std::pair(0, &historyData[0]);
  }

private:
  int64_t writen;
  std::array<HistoryElement, size> historyData;
};

#endif // HISTORY_H
