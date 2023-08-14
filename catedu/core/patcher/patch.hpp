#ifndef HPP_CATEDU_CORE_PATCHER_PATCH
#define HPP_CATEDU_CORE_PATCHER_PATCH

#include <stddef.h>
#include "catedu/core/memory/buffer.hpp"

/**
 * @brief Raw generated patch that can be applied.
 */
struct Patch {
    /// @brief Patch buffer size.
    size_t size;
    /// @brief Patch buffer data.
    void *data;
};

struct PatchSectionData {
    Buffer buffer;
};

struct PatchSectionScope {
    size_t subsection_count;
};

struct PatchSection {
    enum class Type {
        Data,
        Scope
    } type;
    char *name;
    union {
        PatchSectionData data;
        PatchSectionScope scope;
    };
};

#endif
