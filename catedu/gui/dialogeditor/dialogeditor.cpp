#include "dialogeditor.hpp"
#include "catedu/ui/widgets.hpp"

Dialog create_dialog(const char *name, const char *text)
{
    Dialog dialog = {};
    snprintf(dialog.name, sizeof(dialog.name), "%s", name);
    snprintf(dialog.text, sizeof(dialog.text), "%s", text);
    return dialog;
}

DialogEditor DialogEditor::init()
{
    return {};
}

void DialogEditor::deinit()
{
}

void DialogEditor::show(UiUser &user, DialogList &list)
{
    begin_show_window(user, {"Dialog Editor", {300, 0, 400, 400}});
    {
        if (button(user, "Add Dialog"))
        {
            list.dialogs.allocate(create_dialog("New Dialog", "New Text"));
        }

        for (auto [i, dialog] : iter(list.dialogs))
        {
            if (button(user, "Delete"))
            {
                list.dialogs.remove(i);
                continue;
            }

            label(user, dialog.name);
        }
    }

    end_show_window(user);
}
