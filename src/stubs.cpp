
#include <exception>

namespace std {

void terminate() {
  while (1)
    __asm__("BKPT");
}

void __throw_bad_function_call() { terminate(); }

} // namespace std
