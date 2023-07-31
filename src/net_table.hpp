//
// BRIEF: Network packet generation for Table container.
//

#ifndef HPP_CATEDU_NET_TABLE
#define HPP_CATEDU_NET_TABLE

#include "table.hpp"
#include <functional>
#include <vector>
#include "save.hpp"

template <typename T>
struct NetTableIdValue {
    TableId id;
    T value;
};


template <class T>
struct NetTableInit {
    std::vector<NetTableIdValue<T>> pairs;
};


template <class T>
struct NetTableAlloc {
    T value;
};


template <class T>
struct NetTableSet {
    TableId id;
    T value;
};


template <class T>
struct NetTableRemove {
    TableId id;
};


template <class T>
void net_table_init(BinaryFormat *format, Table<T> *table, std::function<void(BinaryFormat *fmt, T *element)> encoder) {

    if (format->mode == BinaryIOMode::WRITE) {
        size_t count = 0;
        for (size_t i = 0; i < table->capacity; i++) {
            if (table->slots[i].taken || table->slots[i].generation) {
                count++;
            }
        }

        format->pass_value(&count);
        for (size_t i = 0; i < table->capacity; i++) {
            if (table->slots[i].taken || table->slots[i].generation) {
                TableId id;
                id.generation = table->slots[i].generation;
                id.id = i+1;
                format->pass_value(&id);
                format->pass_value(&table->slots[i].taken);
                if (table->slots[i].taken) {
                    encoder(format, table->get(id));
                }
            }
        }
    } else {
        *table = {};

        size_t count;
        format->pass_value(&count);

        for (size_t i = 0; i < count; i++) {
            TableId id;
            format->pass_value(&id);
            bool taken;
            format->pass_value(&taken);

            if (taken) {
                T element;
                encoder(format, &element);
                table->allocate_at(id, element);
            } else {
                table->mark_generation_at(id);
            }
        }
    }
}

template <class T>
TableId net_table_alloc(BinaryFormat *format, Table<T> *dest, T *value, std::function<void(BinaryFormat *fmt, T *element)> encoder) {
    if (format->mode == BinaryIOMode::READ) {
        encoder(format, value);

        return dest->allocate(*value);
    } else {
        encoder(format, value);
    }

    return NULL_ID;
}

template <class T>
FileBuffer net_table_set_write(TableId id, T value, std::function<void(BinaryFormat *fmt, T *element)> encoder) {
    BinaryFormat format = BinaryFormat::begin_write();
    format.pass_value(&id);
    encoder(&format, &value);
    return format.leak_file_buffer();
}

template <class T>
void net_table_set_apply(FileBuffer input, Table<T> *dest, std::function<void(BinaryFormat *fmt, T *element)> decoder) {
    BinaryFormat format = BinaryFormat::begin_read(input.data, input.size);

    NetTableSet<T> set;
    format.pass_value(&set.id);

    // TODO: Prevent crashing
    decoder(&format, dest->get(set.id));
}

template <class T>
FileBuffer net_table_remove_write(TableId id) {
    BinaryFormat format = BinaryFormat::begin_write();
    format.pass_value(&id);
    return format.leak_file_buffer();
}

template <class T>
bool net_table_remove_apply(FileBuffer input, Table<T> *dest) {
    BinaryFormat format = BinaryFormat::begin_read(input.data, input.size);

    NetTableRemove<T> remove;
    format.pass_value(&remove.id);

    // TODO: Prevent crashing
    return dest->remove(remove.id);
}



#endif
