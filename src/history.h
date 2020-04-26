#ifndef HISTORY_H
#define HISTORY_H

#include <array>
#include <cstdlib>
#include <tuple>

template <size_t size> struct History {
  struct HistoryElement {
    float voltage;
    float current;

    static constexpr HistoryElement empty{};
  };

  History() : historyData() { reset(); }

  void add(int64_t &pos, float voltage, float current) {
    last_writen_element = pos;
    auto wp = pos % size;

    auto &he = historyData.at(wp);

    he.voltage = voltage;
    he.current = current;
  }

  void reset() { last_writen_element = 0; }

  int64_t start() const {
    return last_writen_element > size ? last_writen_element - size : 0;
  }
  int32_t elements() const {
    return last_writen_element > size ? size : last_writen_element;
  }

  std::pair<int64_t, HistoryElement *const> getLastMeasure() {
    if (last_writen_element) {
      auto rp = (last_writen_element - 1) % size;
      return std::pair(last_writen_element, &historyData[rp]);
    }
    return std::pair(0, &historyData[0]);
  }

  const HistoryElement &read(int64_t pos) const {
    if ((pos > last_writen_element - 1) || (pos < last_writen_element - size)) {
      return HistoryElement::empty;
    }
    auto rp = (pos - 1) % size;
    return historyData[rp];
  }

private:
  int64_t last_writen_element;
  std::array<HistoryElement, size> historyData;
};

#endif // HISTORY_H
