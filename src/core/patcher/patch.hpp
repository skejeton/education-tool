#ifndef HPP_CATEDU_CORE_PATCHER_PATCH
#define HPP_CATEDU_CORE_PATCHER_PATCH

#include <stddef.h>

/**
 * @brief Generic data buffer with data and size.
 */
struct Buffer {
    /// @brief Buffer size.
    size_t size;
    /// @brief Buffer data.
    void *data;
};

/**
 * @brief Raw generated patch that can be applied.
 */
struct Patch {
    /// @brief Patch buffer size.
    size_t size;
    /// @brief Patch buffer data.
    void *data;
};

#endif
