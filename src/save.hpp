///
/// BRIEF: Saving/Loading of levels.
/// 

#ifndef H_CATEDU_SAVE
#define H_CATEDU_SAVE

#include <cstdint>
#include <cassert>
#include <cstring>
#include <functional>
#include <stdint.h>

enum struct BinaryIOMode {
  WRITE,
  READ
};

struct BinaryFormat {
  uint8_t *origin;
  uint8_t *data;
  size_t cap;
  BinaryIOMode mode;

  static BinaryFormat begin_write()
  {
    BinaryFormat format = {};
    format.mode = BinaryIOMode::WRITE;
    format.cap = 128;
    format.origin = (uint8_t*)malloc(128);
    format.data = format.origin;
    return format;
  }

  static BinaryFormat begin_read(void *origin, size_t max)
  {
    BinaryFormat format = {};
    format.mode = BinaryIOMode::READ;
    format.cap = max;
    format.origin = (uint8_t*)origin;
    format.data = format.origin;
    return format;
  }

  bool can_fit(size_t n)
  {
    return (this->data - origin)+n < cap;
  }

  void resize(size_t n) {
    size_t offs = data - origin;
    while (!can_fit(n)) {
      cap *= 2;
      origin = (uint8_t*)realloc(origin, cap);
      data = origin + offs;
    }
  }

  void pass_bytes(void *data, size_t n)
  {
    if (!can_fit(n)) {
      if (this->mode == BinaryIOMode::READ) {
        assert(false && "Read overflow");
      } else {
        resize(n);
      }
    }

    if (this->mode == BinaryIOMode::READ) {
      memcpy(data, this->data, n);
    } else {
      memcpy(this->data, data, n);
    }
    this->data += n;
  }

  template <class T>
  void pass_value(T *data)
  {
    pass_bytes(data, sizeof *data);
  }

  inline void pass_pointer(void **data, size_t n)
  {
    pass_value(&n);

    if (mode == BinaryIOMode::READ) {
      uint8_t *bytes = (uint8_t*)malloc(n);
      pass_bytes(bytes, n);
      *data = bytes;
    } else {
      uint8_t *bytes = (uint8_t*)*data;
      pass_bytes(bytes, n);
    }
  }

  inline void pass_c_string(char **string)
  {
    assert(*string && "NULL string");

    pass_pointer((void**)string, mode == BinaryIOMode::WRITE ? strlen(*string)+1 : 0);
  }
};

#endif
