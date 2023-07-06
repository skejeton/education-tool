///
/// BRIEF: Temporary ID binder, allows you to map `int` ID into an actual type.
///

#ifndef H_TEMP_ID_BINDER_CATEDU
#define H_TEMP_ID_BINDER_CATEDU

template <class T>
struct TempIdBinder {
    T *items;
    size_t items_count;
    size_t items_cap;

    static TempIdBinder init() {
        return {};
    }

    int allocate(T value) {
        if (items == nullptr) {
            items_cap = 128;
            items = (T*)realloc(items, items_cap * sizeof(T));
        }

        if (items_count == items_cap) {
            items_cap *= 2;
            items = (T*)realloc(items, items_cap * sizeof(T));
        }

        items[items_count++] = value;

        return items_count-1;
    }

    T *get(int id) {
        if (id < 0 || id >= items_count) {
            return nullptr;
        }

        return &items[id];
    }
};

#endif