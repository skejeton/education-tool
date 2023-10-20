#include "text_editing.hpp"
#include <algorithm>
#include <assert.h>

static size_t
remaining_space(SingleLineTextEditor& editor)
{
    // 1. Check if there is any space left.
    assert(editor.buffer.string_cap >= editor.buffer.size);

    // 2. Calculate the remaining space.
    return editor.buffer.string_cap - editor.buffer.size;
};

static SingleLineTextCursor
sanitize_cursor(SingleLineTextEditor& editor, SingleLineTextCursor cursor)
{
    // 1. Clamp the cursor position.
    // ASSUMPTION: The buffer size does not exceed the maximum value of int32_t.
    cursor.column = std::min(cursor.column, int32_t(editor.buffer.size));
    cursor.column = std::max(cursor.column, 0);

    // 2. Done
    return cursor;
}

SingleLineTextEditor::SingleLineTextEditor(char* data, size_t buffer_cap)
{
    // 1. Check the parameters.
    assert(buffer_cap != 0 && "Buffer capacity must be greater than zero.");

    // 2. Initialize the editor.
    this->buffer.data = data;
    this->buffer.buffer_cap = buffer_cap;
    this->buffer.string_cap = buffer_cap - 1;
    this->buffer.size = 0;

    // 3. Initialize the cursor.
    this->cursor = { 0 };
}

size_t
SingleLineTextEditor::insert(const char* str)
{
    // 1. Calculate the number of characters to insert.
    size_t characters_to_insert = std::min(strlen(str), remaining_space(*this));

    printf(
      "characters_to_insert: %zu %zu\n", remaining_space(*this), strlen(str));

    // 2. Reserve space for the characters.
    char* insertion_point = this->buffer.data + this->cursor.column;
    memmove(insertion_point + characters_to_insert,
            insertion_point,
            this->buffer.size - this->cursor.column);

    // 3. Copy the characters.
    memcpy(insertion_point, str, characters_to_insert);

    // 4. Update the cursor.
    this->cursor.column += characters_to_insert;

    // 5. Update the buffer size.
    this->buffer.size += characters_to_insert;

    // 6. Set null terminator.
    this->buffer.data[this->buffer.size] = '\0';

    // 7. Done
    return characters_to_insert;
}

size_t
SingleLineTextEditor::erase_back()
{
    // 1. Check if there is anything to erase.
    if (this->cursor.column == 0) {
        return 0;
    }

    // 2. Update the cursor.
    char* deletion_point = this->buffer.data + this->cursor.column - 1;
    memmove(deletion_point,
            deletion_point + 1,
            this->buffer.size - this->cursor.column);

    // 4. Update the cursor.
    this->cursor.column -= 1;

    // 3. Update the buffer size.
    this->buffer.size -= 1;

    // 4. Set null terminator.
    this->buffer.data[this->buffer.size] = '\0';

    // 5. Done
    return 1;
}

void
SingleLineTextEditor::move_cursor(int delta)
{
    // 1. Move the cursor by the delta.
    this->set_cursor({ this->cursor.column + delta });

    // 2. Done
    return;
}

void
SingleLineTextEditor::set_cursor(SingleLineTextCursor cursor)
{
    // 1. Update the cursor.
    this->cursor = sanitize_cursor(*this, cursor);

    // 2. Done
    return;
}
