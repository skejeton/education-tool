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
FileBuffer net_table_init_write(Table<T> *table, std::function<void(BinaryFormat *fmt, T *element)> encoder) {
    BinaryFormat format = BinaryFormat::begin_write();
    TableIterator<T> iter = TableIterator<T>::init(table);

    format.pass_value(&table->count);

    for (; iter.going(); iter.next()) {
        format.pass_value(iter.id);
        encoder(&format, iter.value);
    }

    return format.leak_file_buffer();
}

template <class T>
void net_table_init_apply(FileBuffer input, Table<T> *dest, std::function<void(BinaryFormat *fmt, T *element)> decoder) {
    BinaryFormat format = BinaryFormat::begin_read(input.data, input.size);

    size_t count;
    format.pass_value(&count);

    NetTableInit<T> init;

    *dest = {};

    for (size_t i = 0; i < count; i++) {
        size_t id;
        format.pass_value(&id);
        T element;
        decoder(&format, &element);

        init.pairs.push_back({id, element});
    }

    for (auto &initializer : init) {
        dest->allocate_at(initializer.id, initializer.value);
    }
}

template <class T>
FileBuffer net_table_alloc_write(T value, std::function<void(BinaryFormat *fmt, T *element)> encoder) {
    BinaryFormat format = BinaryFormat::begin_write();
    encoder(&format, &value);
    return format.leak_file_buffer();
}

template <class T>
void net_table_alloc_apply(FileBuffer input, Table<T> *dest, std::function<void(BinaryFormat *fmt, T *element)> decoder) {
    BinaryFormat format = BinaryFormat::begin_read(input.data, input.size);

    T value;
    decoder(&format, &value);

    dest->allocate(value);
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
bool net_table_set_apply(FileBuffer input, Table<T> *dest) {
    BinaryFormat format = BinaryFormat::begin_read(input.data, input.size);

    NetTableRemove<T> remove;
    format.pass_value(&remove.id);

    // TODO: Prevent crashing
    return dest->remove(remove.id);
}



#endif
