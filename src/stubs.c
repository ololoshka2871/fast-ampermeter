
#include <stdlib.h>

#include "softreset.h"

void abort(void) { SoftReset(); }
