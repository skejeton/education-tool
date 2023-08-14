#ifndef HPP_CATEDU_CORE_MEMORY_BUFFER
#define HPP_CATEDU_CORE_MEMORY_BUFFER

/**
 * @brief Generic data buffer with data and size.
 */
struct Buffer {
    /// @brief Buffer size.
    size_t size;
    /// @brief Buffer data.
    void *data;
};

#endif
