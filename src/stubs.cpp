
#include <exception>

namespace std {

void terminate() noexcept {
  while (1)
    __asm__("BKPT");
}

void __throw_bad_function_call() { terminate(); }

void __throw_out_of_range_fmt(char const *, ...) { terminate(); }

} // namespace std
