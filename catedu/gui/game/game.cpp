#include "game.hpp"
#include "catedu/ui/rendering/make_brush.hpp"

/*
const char* message = "Use arrow keys to move. <^v>. Well, I think you already "
                      "figured it out.\nPress [ESC] to close this dialog.";

                      */

AutoLayoutElement
make_element(AutoLayout layout,
             Vector2 size,
             bool autox,
             bool autoy,
             Vector2 align = { 0, 0 },
             float p = 3);

GuiGame
GuiGame::init(UiState* ui_state)
{
    return { ui_state };
}

void
GuiGame::deinit()
{
    // Nothing yet
}

void
GuiGame::show_dialogue(const char* text, const char* ans, char* ans_out)
{
    this->dialog_open = true;
    strcpy(dialog_text, text);
    this->ans_out = NULL;
    if (ans) {
        strcpy(dialog_ans, ans);
        this->dialog_has_ans = true;
        assert(ans_out);
        this->ans_out = ans_out;
    }
}

int
GuiGame::show()
{
    int ui_mode = 2;

    UiUser user = UiUser::init(*this->ui_state);
    user.begin_pass();
    user.begin_generic(make_element({ AutoLayout::Column },
                                    { sapp_widthf(), sapp_heightf() },
                                    false,
                                    false,
                                    { 0, 1 },
                                    0),
                       UiMakeBrush::make_solid({ 0, 0, 0, 0 }),
                       UiMakeBrush::make_solid({ 0, 0, 0, 0 }));

    if (this->dialog_open) {
        user.begin_generic(make_element({ AutoLayout::Column },
                                        { sapp_widthf() - 30, 300 },
                                        false,
                                        false,
                                        { 0, 0 },
                                        5),
                           UiMakeBrush::make_gradient({ 0.5, 0.5, 0.5, 0.5f },
                                                      { 0.6, 0.6, 0.6, 0.5f }),
                           UiMakeBrush::make_solid({ 0, 0, 0, 1.0 }));
        user.label("Entity says:",
                   { 3, 3 },
                   UiMakeBrush::make_gradient({ 0.5, 0.5, 0.9, 1.0f },
                                              { 0.5, 0.5, 1.0, 1.0f }));

        if (this->ans_out) {
            user.input("ans", this->ans_out, 16);
        }

        user.label(this->dialog_text, { 2, 2 });
        user.end_generic();
    } else {
        ui_mode = user.button("Exit") ? 0 : ui_mode;
    }

    user.end_generic();
    user.end_pass();

    return ui_mode;
}
