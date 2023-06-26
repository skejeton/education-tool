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
#include <unordered_map>
#include "table.hpp"

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
    return (this->data - origin)+n <= cap;
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
   
    printf("%5s ", mode == BinaryIOMode::READ ? "READ" : "WRITE");

    uint8_t *data_raw = (uint8_t*)data;
    for (size_t i = 0; i < sizeof(*data); i++) {
      printf("%02x ", data_raw[i]);
    }
    printf("\n");
  }

  inline void pass_pointer(void **data, size_t n)
  {
    if (mode == BinaryIOMode::WRITE && *data == nullptr) {
      n = (size_t)-1;
      pass_value(&n);
    } 

    pass_value(&n);
    if (mode == BinaryIOMode::READ) {
      if (n == (size_t)-1) {
        *data = nullptr;
      }

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
    size_t len = 0;
    if (mode == BinaryIOMode::WRITE && *string) {
      len = strlen(*string) + 1;
    }

    pass_pointer((void**)string, len);
  }
};

struct IdMapper {
  std::unordered_map<size_t, size_t> id_map;
  size_t id_increment;

  void push_id(BinaryFormat *format, size_t id)
  {
    format->pass_value(&id_increment);
    if (format->mode == BinaryIOMode::WRITE) {
      printf("WRITE-REG %zu %zu\n", id, id_increment);
      id_map[id] = id_increment++;
    }
    else {
      printf("READ-REG %zu %zu\n", id_increment, id);
      id_map[id_increment] = id;
    }
    printf("SIZ %zu\n", id_map.size());
  }

  size_t get_id(size_t original) {
    return id_map.at(original);
  }
};

template <class T>
struct TableSaver {
  BinaryFormat *format;
  TableIterator<T> iter;
  IdMapper id_mapper;
  size_t count;

  static TableSaver init(BinaryFormat *format, Table<T> *table)
  {
    TableSaver saver = {format, TableIterator<T>::init(table)};

    saver.count = table->count;
    format->pass_value(&saver.count);
    
    return saver;
  }

  bool going()
  {
    if (format->mode == BinaryIOMode::READ) {
      return count;
    } else {
      return iter.going();
    }
  }

  void next()
  {
    if (format->mode == BinaryIOMode::READ) {
      assert(count && "Too much .next() calls");
      count--;
    } else {
      iter.next();
    }
  }

  T* save()
  {
    if (format->mode == BinaryIOMode::WRITE) {
      id_mapper.push_id(format, iter.id.id);
      return iter.table->get(iter.id);
    } else {
      TableId id = iter.table->allocate({});
      id_mapper.push_id(format, id.id);
      return iter.table->get(id);
    }
  }

  void pass_id(TableId *id) 
  {
    printf("pass id");
    if (format->mode == BinaryIOMode::WRITE) {
      size_t write_id = id_mapper.get_id(id->id);
      format->pass_value(&write_id);
    } else {
      size_t read_id;
      format->pass_value(&read_id);
      *id = {id_mapper.get_id(read_id)};
      printf("READ-ID: %zu %zu\n", read_id, id->id);
    }
  }
};

#endif
