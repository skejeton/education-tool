#include "catedu/core/text_editing/text_editing.hpp"
#include "tau/tau.h"

// Test case for SingleLineTextEditor
TEST(SingleLineTextEditorTest, BasicTest)
{
    // 1. Create a SingleLineTextEditor object
    char buffer[32];
    SingleLineTextEditor editor(buffer, sizeof(buffer));

    // 2. Basic insertion
    editor.insert("Hello, world!");
    CHECK_STREQ(buffer, "Hello, world!");

    // 3. Basic deletion
    editor.erase_back();
    CHECK_STREQ(buffer, "Hello, world");

    // 4. Advanced deletion.
    editor.move_cursor(-6);
    editor.erase_back();
    CHECK_STREQ(buffer, "Hello world");

    // 5. Advanced insertion.
    editor.move_cursor(1);
    editor.insert("cruel ");
    CHECK_STREQ(buffer, "Hello cruel world");

    // 6. Moving the cursor.
    editor.move_cursor(-1);
    CHECK_EQ(editor.cursor.column, 11);

    // 7. Moving the cursor forward.
    editor.move_cursor(1);
    CHECK_EQ(editor.cursor.column, 12);

    // 8. Moving the all the way back.
    editor.move_cursor(-9999);
    CHECK_EQ(editor.cursor.column, 0);

    // 9. Moving the all the way forward.
    editor.move_cursor(9999);
    CHECK_EQ(editor.cursor.column, 17);

    // 10. Overflowing the buffer.
    editor.insert("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    CHECK_STREQ(buffer, "Hello cruel worldAAAAAAAAAAAAAA");
    CHECK_EQ(editor.cursor.column, 31);

    // 11. Removing everything.
    for (int i = 0; i < 100; i++)
    {
        editor.erase_back();
    }
    CHECK_STREQ(buffer, "");
    CHECK_EQ(editor.cursor.column, 0);
}
