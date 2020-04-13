
#include "hw_includes.h"

static void retarget_put_char(int ch) {
#ifndef STM32F0
    ITM_SendChar((uint32_t)ch);
#else
    (void)ch;
#endif
}

int _write(int fd, char *ptr, int len) {
  (void)fd;
  while (*ptr && len--) {
    retarget_put_char((int)*ptr);
    ptr++;
  }
  return len;
}

int _write_r(struct _reent *r, int file, const void *ptr, size_t len) {
  (void)r;
  _write(file, (char *)ptr, len);
  return len;
}
