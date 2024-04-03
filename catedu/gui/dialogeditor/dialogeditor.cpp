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
    if (begin_show_window(user, {"Conversation", {300, 0, 400, 400}}))
    {
        label(user, "Entity ID", {1, 1});
        input(user, "entityid", list.entityid, sizeof list.entityid);
        if (button(user, "Add Dialog"))
        {
            list.dialogs.allocate(create_dialog("New Dialog", "New Text"));
        }

        TableId del = NULL_ID;

        for (auto [i, dialog] : iter(list.dialogs))
        {
            user.push_id(i.id);

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
        }

        if (del != NULL_ID)
        {
            list.dialogs.remove(del);
        }
    }
    end_show_window(user);

    Dialog *dialog = list.dialogs.get(selected);
    if (dialog)
    {
        if (begin_show_window(user, {"Edit Dialog", {700, 0, 400, 400}}))
        {
            label(user, "Name");
            input(user, "Name", dialog->name, sizeof(dialog->name));
            label(user, "Text");
            input(user, "Text", dialog->text, sizeof(dialog->text));
        }
        end_show_window(user);
    }
}
