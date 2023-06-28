///
/// BRIEF: Table for allocation and management of rows
///

#ifndef H_CATEDU_TABLE
#define H_CATEDU_TABLE

#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct TableId {
  size_t id;

  bool operator==(TableId other) {
    return id == other.id;
  }
};

template <class T>
struct Table {
  T* values;
  bool* values_taken;
  size_t capacity;
  size_t count;

  TableId allocate(T value)
  {
    TableId id = { 0 };
    for (size_t i = 0; i < capacity; i++) {
      if (!values_taken[i]) {
        id = { i + 1 };
      }
    }


    if (id.id == 0) {
      size_t old_capacity = capacity;
      if (capacity == 0) capacity = 32;
      else capacity *= 2;

      T* new_values = (T*)realloc(values, sizeof(T)*capacity);
      // NOTE: Allocation error
      if (new_values == NULL) {
        return { 0 };
      }
      values = new_values;

      bool *new_values_taken = (bool*)realloc(values_taken, sizeof(size_t)*capacity);
      // NOTE: Allocation error
      if (new_values_taken == NULL) {
        return { 0 };
      }
      values_taken = new_values_taken;

      memset(values_taken + old_capacity, 0, (capacity - old_capacity) * sizeof(size_t));
      id = { old_capacity+1 };
    }

    count++;
    values_taken[id.id - 1] = true;
    values[id.id - 1] = value;
    return id;
  }

  /// RETURNS: Status value indicating whether the element was freed correctly
  bool remove(TableId id)
  {
    bool status = false;
    if (id.id - 1 < capacity) {
      status = values_taken[id.id - 1];
      values_taken[id.id - 1] = false;
      count--;
    }
    return status;
  }

  T* get(TableId id)
  {
    if (id.id - 1 < capacity) {
      return &values[id.id - 1];
    }
    return nullptr;
  }
};

template <class T>
struct TableIterator {
  Table<T>* table;
  TableId id;
  bool is_going;

  void seek_next_value() {
    is_going = false;

    id.id += 1;
    while ((id.id - 1) < table->capacity) {
      if (table->values_taken[id.id - 1]) {
        is_going = true;
        break;
      }
      id.id += 1;
    }
  }

  bool going() {
    return is_going;
  }

  void next() {
    seek_next_value();
  }

  static TableIterator init(Table<T>* table) {
    TableIterator iterator = { table, { 0 }, false };
    iterator.seek_next_value();
    return iterator;
  }
};

#endif

