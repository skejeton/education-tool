///
/// BRIEF: Table for allocation and management of rows
///

#ifndef H_CATEDU_TABLE
#define H_CATEDU_TABLE

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct TableId
{
    size_t id;
    size_t generation;

    bool operator==(TableId other)
    {
        return (id ? generation == other.generation : true) && id == other.id;
    }

    bool operator!=(TableId other) { return !(*this == other); }

    bool valid() { return id != 0; }
};

#define NULL_ID (TableId{ 0 })

template<class T>
struct Table
{
    T* values;
    struct Slot
    {
        bool taken;
        size_t generation;
    }* slots;
    size_t capacity;
    size_t count;

    void deinit()
    {
        free(values);
        free(slots);
    }

    bool scale()
    {
        size_t new_capacity = capacity;
        if (new_capacity == 0)
            new_capacity = 32;
        else
            new_capacity *= 2;

        T* new_values = (T*)realloc(values, sizeof(T) * new_capacity);
        // NOTE: Allocation error
        if (new_values == NULL) {
            return false;
        }
        values = new_values;

        Slot* new_slots = (Slot*)realloc(slots, sizeof(Slot) * new_capacity);
        // NOTE: Allocation error
        if (new_slots == NULL) {
            return false;
        }
        slots = new_slots;

        memset(slots + capacity, 0, (new_capacity - capacity) * sizeof(Slot));

        capacity = new_capacity;
        return true;
    }

    TableId allocate_at(TableId id, T value)
    {
        while (capacity < id.id) {
            if (!scale())
                return NULL_ID;
        }

        count++;
        slots[id.id - 1].generation = id.generation;
        slots[id.id - 1].taken = true;
        values[id.id - 1] = value;
        return id;
    }

    TableId find_next_id()
    {
        TableId id = { 0 };
        for (size_t i = 0; i < capacity; i++) {
            if (!slots[i].taken) {
                id = { i + 1 };
                id.generation = slots[i].generation;
                break;
            }
        }

        if (id.id == 0) {
            id = { capacity + 1 };
        }

        return id;
    }

    void mark_generation_at(TableId id)
    {
        while (capacity < id.id) {
            assert(scale());
        }

        slots[id.id - 1].generation = id.generation;
    }

    TableId allocate(T value)
    {
        TableId id = { 0 };
        for (size_t i = 0; i < capacity; i++) {
            if (!slots[i].taken) {
                id = { i + 1 };
                break;
            }
        }

        if (id.id == 0) {
            size_t old_capacity = capacity;
            if (!scale()) {
                return NULL_ID;
            }
            id = { old_capacity + 1 };
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
        if (id.id - 1 < capacity && slots[id.id - 1].taken &&
            slots[id.id - 1].generation == id.generation) {
            slots[id.id - 1].taken = false;
            slots[id.id - 1].generation++;
            count--;
            return true;
        }
        return false;
    }

    T* get(TableId id)
    {
        if (id.id - 1 < capacity && slots[id.id - 1].taken &&
            slots[id.id - 1].generation == id.generation) {
            return &values[id.id - 1];
        }
        return nullptr;
    }
};

template<class T>
struct TableIterator
{
    Table<T>* table;
    TableId id;
    bool is_going;

    bool going() { return is_going; }

    void next()
    {
        is_going = false;

        id.id += 1;
        while ((id.id - 1) < table->capacity) {
            if (table->slots[id.id - 1].taken) {
                id.generation = table->slots[id.id - 1].generation;
                is_going = true;
                break;
            }
            id.id += 1;
        }
    }

    static TableIterator init(Table<T>* table)
    {
        TableIterator iterator = { table, { 0 }, false };
        iterator.next();
        return iterator;
    }
};

#endif
