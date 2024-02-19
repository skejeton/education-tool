#pragma once

#include <assert.h>

#define OOM_HANDLER(v) (assert((v) != NULL && "Out of memory"), (v))
