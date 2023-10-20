#ifndef HPP_CATEDU_CORE_STORAGE_ID_RETAINER
#define HPP_CATEDU_CORE_STORAGE_ID_RETAINER

#include "catedu/util.hpp"
#include <assert.h>
#include <map>
#include <stddef.h>
#include <string>
#include <variant>
#include <vector>

template<typename T>
struct IdRetainerNode
{
    size_t death;
    T* value;
};

template<typename T>
struct IdRetainer
{
    size_t cycle_number;
    std::vector<std::string> current_path;
    std::map<std::string, IdRetainerNode<T>> values;

    static IdRetainer<T> init();

    void begin_cycle();
    void end_cycle();

    T* value();

    /// @brief Creates a branch with the specified ID and value.
    /// @param id The ID of the branch.
    /// @param value The value of the branch.
    void push(const char* id, T value = {});
    void pop(int n = 1);
};

template<typename T>
inline IdRetainer<T>
IdRetainer<T>::init()
{
    return IdRetainer{ 0, {}, {} };
}

template<typename T>
inline void
IdRetainer<T>::begin_cycle()
{
    this->current_path.clear();
}

template<typename T>
inline void
IdRetainer<T>::end_cycle()
{
    // 1. Make sure that we haven't pre-emptively ending without finishing the
    // tree.
    assert(this->current_path.empty() &&
           "IdRetainer: Invalid end of cycle, not all values were popped");

    // 2. Remove all dead nodes.
    for (auto it = this->values.begin(); it != this->values.end();) {
        if (it->second.death <= this->cycle_number) {
            free(it->second.value);
            it = this->values.erase(it);
        } else {
            ++it;
        }
    }

    // 3. Proceed to the next cycle.
    this->cycle_number++;
}

template<typename T>
inline T*
IdRetainer<T>::value()
{
    std::string path = join_vector_into_string(this->current_path, "/");
    auto it = this->values.find(path);
    assert(it != this->values.end() && "IdRetainer: Invalid path");
    return it->second.value;
}

template<typename T>
inline void
IdRetainer<T>::push(const char* s, T value)
{
    this->current_path.push_back(s);
    std::string path = join_vector_into_string(this->current_path, "/");
    if (this->values.count(path) == 0) {
        T* v_ptr = (T*)malloc(sizeof(T));
        *v_ptr = value;
        this->values[path] = { this->cycle_number + 1, v_ptr };
    } else {
        this->values[path].death = this->cycle_number + 1;
    }
}

template<typename T>
inline void
IdRetainer<T>::pop(int n)
{
    assert(this->current_path.size() >= n &&
           "IdRetainer: Trying to pop more values than there are in the tree");

    for (size_t i = 0; i < n; i++) {
        this->current_path.pop_back();
    }
}

#endif
