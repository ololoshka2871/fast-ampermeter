
#include <exception>

void std::terminate() {
  while (1)
    __asm__("BKPT");
}
