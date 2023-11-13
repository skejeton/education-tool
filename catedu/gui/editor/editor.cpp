#include "editor.hpp"

// TEMPORARY
AutoLayoutElement
make_element(AutoLayout layout,
             Vector2 size,
             bool autox,
             bool autoy,
             Vector2 align = { 0, 0 },
             float p = 3);

AutoLayoutElement
make_auto(AutoLayout layout, Vector2 align = { 0, 0 });

const char* selections[] = { "Remove", "Bricks", "Tile", "Entity", "Player" };

GuiEditor
GuiEditor::init(UiState* ui_state)
{
    return { ui_state };
}

void
GuiEditor::deinit()
{
    // Nothing yet
}

int
GuiEditor::show()
{
    int ui_mode = 1;
    UiUser user = UiUser::init(*this->ui_state);
    user.begin_pass();

    for (int i = 0; i < sizeof(selections) / sizeof(selections[0]); i++) {
        selection = user.button(stdstrfmt("%c%s%c",
                                          i == selection ? '[' : ' ',
                                          selections[i],
                                          i == selection ? ']' : ' ')
                                  .c_str())
                      ? i
                      : selection;
    }

    ui_mode = user.button("Exit") ? 0 : ui_mode;

    user.end_pass();

    return ui_mode;
}
