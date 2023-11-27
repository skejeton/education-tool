#include "game.hpp"
#include "catedu/ui/rendering/make_brush.hpp"

AutoLayoutElement make_element(AutoLayout layout, Vector2 size, bool autox,
                               bool autoy, Vector2 align = {0, 0}, float p = 3);
AutoLayoutElement make_auto(AutoLayout layout, Vector2 align = {0, 0});

GuiGame GuiGame::init(UiState *ui_state)
{
    return {ui_state};
}

void GuiGame::deinit()
{
    // Nothing yet
}

void GuiGame::show_dialog(Dialog dialog)
{
    dialog.open = true;
    this->dialog = dialog;
}

int GuiGame::show(int currency, int reals, char **script)
{
    *script = nullptr;
    int ui_mode = 2;

    UiUser user = UiUser::init(*this->ui_state);
    user.begin_pass();

    user.begin_generic(make_element({AutoLayout::Column},
                                    {sapp_widthf(), sapp_heightf()}, false,
                                    false, {0, 1}, 0),
                       UiMakeBrush::make_solid({0, 0, 0, 0}),
                       UiMakeBrush::make_solid({0, 0, 0, 0}));

    user.begin_generic(make_auto({AutoLayout::Column}), {}, {});
    user.label(stdstrfmt("S$%d", currency).c_str(), {3, 3});
    user.label(stdstrfmt("R$%d", reals).c_str(), {3, 3});

    if (this->dialog.open)
    {
        user.begin_generic(make_element({AutoLayout::Column},
                                        {sapp_widthf() - 30, 300}, false, false,
                                        {0, 0}, 5),
                           UiMakeBrush::make_gradient({0.5, 0.5, 0.5, 0.5f},
                                                      {0.6, 0.6, 0.6, 0.5f}),
                           UiMakeBrush::make_solid({0, 0, 0, 1.0}));
        user.label(this->dialog.title, {3, 3},
                   UiMakeBrush::make_gradient({0.5, 0.5, 0.9, 1.0f},
                                              {0.5, 0.5, 1.0, 1.0f}));

        user.label(this->dialog.text, {2, 2});
        user.label(this->dialog.translation, {1.5, 1.5},
                   UiMakeBrush::make_gradient({1.0, 1.0, 1.0, 0.8f},
                                              {1.0, 1.0, 1.0, 0.8f}));

        user.begin_generic(make_auto({AutoLayout::Row}), {}, {});
        for (int i = 0; i < 8; i++)
        {
            if (this->dialog.buttons[i].text)
            {
                if (user.button(this->dialog.buttons[i].text))
                {
                    this->dialog.open = false;
                    *script = this->dialog.buttons[i].script;
                }
            }
        }
        user.end_generic();

        user.end_generic();
    }
    else
    {
        ui_mode = user.button("Exit") ? 0 : ui_mode;
    }
    user.end_generic();

    user.end_generic();
    user.end_pass();

    return ui_mode;
}
