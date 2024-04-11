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
    return {-1};
}

void DialogEditor::deinit()
{
}

void DialogEditor::show(UiUser &user, DialogList &list)
{
    if (begin_show_window(user, {"Conversation", {300, 0, 250, 400}}))
    {
        if (button(user, "Add Dialog"))
        {
            list.dialogs.push(create_dialog("New Dialog", "New Text"));
        }

        int del = -1;
        int i = 0;

        for (auto dialog : iter(list.dialogs))
        {
            user.push_id(i);

            AutoLayoutElement element = {};
            element.layout.type = AutoLayout::Row;
            user.begin_generic(element, {}, {});

            if (button(user, "Delete"))
            {
                del = i;
            }

            if (button(user, "Edit"))
            {
                selected = i;
            }

            label(user, dialog.name);

            user.end_generic();
            user.pop_id();
            i++;
        }

        if (del != -1)
        {
            printf("deleting %d\n", del);
            list.dialogs.del(del);
            if (del < selected)
            {
                selected--;
            }
        }
    }
    end_show_window(user);

    if (selected != -1)
    {
        Dialog *dialog = &list.dialogs[selected];
        if (begin_show_window(user, {"Edit Dialog", {700, 0, 250, 400}}))
        {
            label(user, "Name");
            input(user, "Name", dialog->name, sizeof(dialog->name));
            label(user, "Text");
            input(user, "Text", dialog->text, sizeof(dialog->text));
        }
        end_show_window(user);
    }
}
