/// ***
/// @brief Text editing operations
/// ***

#pragma once
#include <stdint.h>
#include <stddef.h>

struct BasicTextBuffer
{
    char *data;
    size_t buffer_cap;
    size_t string_cap;
    size_t size;
};

struct SingleLineTextCursor
{
    int32_t column;
};

//
// TODO: Make the interface less procedural.
//
struct SingleLineTextEditor
{
    BasicTextBuffer buffer;
    SingleLineTextCursor cursor;

    /// @brief Creates a single line text editor from a buffer.
    SingleLineTextEditor(char *data, size_t buffer_cap);

    /// @brief Inserts a string at the cursor position.
    /// @return The number of characters inserted.
    size_t insert(const char *str);

    /// @brief Erases a character before the cursor.
    /// @return The number of characters erased.
    size_t erase_back();

    /// @brief Moves the cursor.
    void move_cursor(int delta);

    /// @brief Sets the cursor position.
    void set_cursor(SingleLineTextCursor cursor);
};
