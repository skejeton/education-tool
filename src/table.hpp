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
  size_t generation;

  bool operator==(TableId other) {
    return id == other.id;
  }
};

template <class T>
struct Table {
  T* values;
  struct Slot {
    bool taken;
    size_t generation;
  } *slots;
  size_t capacity;
  size_t count;

  TableId allocate(T value)
  {
    TableId id = { 0 };
    for (size_t i = 0; i < capacity; i++) {
      if (!slots[i].taken) {
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

      Slot *new_slots = (Slot*)realloc(slots, sizeof(Slot)*capacity);
      // NOTE: Allocation error
      if (new_slots == NULL) {
        return { 0 };
      }
      slots = new_slots;

      memset(slots + old_capacity, 0, (capacity - old_capacity) * sizeof(size_t));
      id = { old_capacity+1 };
    }

    count++;
    slots[id.id - 1].taken = true;
    id.generation = slots[id.id - 1].generation;
    values[id.id - 1] = value;
    return id;
  }

  /// RETURNS: Status value indicating whether the element was freed correctly
  bool remove(TableId id)
  {
    bool status = false;
    if (id.id - 1 < capacity && slots[id.id - 1].taken) {
      slots[id.id - 1].generation++;
      return true;
    }
    return false;
  }

  T* get(TableId id)
  {
    if (id.id - 1 < capacity && slots[id.id - 1].taken && slots[id.id - 1].generation == id.generation) {
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
      if (table->slots[id.id - 1].taken) {
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

