#pragma once
#include "buffer.hpp"
#include <inttypes.h>

/**
 * @brief A sequential memory reader/writer.
 */
struct SerialMem
{
    /**
     * @brief Operation type checker.
     */
    enum Operation
    {
        /// @brief Allow both reads and writes at the same time.
        Any,
        /// @brief Allow only reads, error out on writes.
        Read,
        /// @brief Allow only writes, error out on reads.
        Write
    };

    Buffer buffer;
    size_t at;
#ifdef _DEBUG
    /// @brief To make sure we don't read/write on same serial,
    ///        this is almost a never good idea,
    Operation filter_;
#endif

    /***
     * @brief Creates a serial reader/writer from a buffer.
     * @param buffer The buffer to create the serial reader/writer from.
     * @param only_operation (Debug only), restict to a specific operation,
     *        refer to documentation for `Operation`
     */
    static SerialMem from_buffer(Buffer buffer, Operation filter = Any)
    {
        SerialMem result = {};
#ifdef _DEBUG
        result.filter_ = filter;
#endif
        result.buffer = buffer;

        return result;
    }

    /**
     * @brief Writes data from the buffer.
     * @return Amount of bytes couldn't fit.
     */
    [[nodiscard]] size_t write_buffer(Buffer buf);

    /**
     * @brief Reads data to the buffer.
     * @return Amount of bytes couldn't read.
     */
    [[nodiscard]] size_t read_buffer(Buffer buf);

    /**
     * @brief Writes a value.
     * @return Amount of bytes couldn't fit.
     * @warning SAFETY: This function doesn't convert to a uniform endianness.
     */
    template <class T> [[nodiscard]] size_t write_value(T value)
    {
        return write_buffer({sizeof(value), &value});
    }

    /**
     * @brief Reads a value.
     * @return Amount of bytes couldn't read.
     * @warning SAFETY: This function doesn't convert to a uniform endianness.
     */
    template <class T> [[nodiscard]] size_t read_value(T *value)
    {
        return read_buffer({sizeof(*value), value});
    }

    /// @return Gives pointer to the current position in the buffer
    void *pointer();
};
