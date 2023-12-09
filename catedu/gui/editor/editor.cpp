#include "editor.hpp"

// TEMPORARY
AutoLayoutElement make_element(AutoLayout layout, Vector2 size, bool autox,
                               bool autoy, Vector2 align = {0, 0}, float p = 3);

AutoLayoutElement make_auto(AutoLayout layout, Vector2 align = {0, 0});
GuiEditor GuiEditor::init(UiState *ui_state)
{
    return {ui_state};
}

void GuiEditor::deinit()
{
    // Nothing yet
}

void GuiEditor::show_tab_tiles(UiUser &user, ResourceSpec &res)
{
    user.begin_generic(make_auto({AutoLayout::Column}), {}, {});

    // TODO(T4): Gotta be a better way to iterate over empty tile ids instead of
    //           adding a button before the loop?
    selection =
        user.button(stdstrfmt("%cremove%c", selection.id == 0 ? '[' : ' ',
                              selection.id == 0 ? ']' : ' ')
                        .c_str())
            ? TableId{0}
            : selection;

    for (auto [id, tile] : iter(res.tiles))
    {
        selection =
            user.button(stdstrfmt("%c%s%c", id == selection ? '[' : ' ',
                                  tile.name, id == selection ? ']' : ' ')
                            .c_str())
                ? id
                : selection;
    }
    user.end_generic();
}

bool GuiEditor::show_tab_entity(UiUser &user, WorldEntity *ent,
                                ResourceSpec &res)
{
    bool rem = false;
    user.begin_generic(make_auto({AutoLayout::Column}), {}, {});

    if (ent)
    {
        user.begin_generic(make_auto({AutoLayout::Row}), {}, {});
        user.label("Id", {2, 2});
        user.input("Id", ent->id, 32);
        user.end_generic();

        user.begin_generic(make_auto({AutoLayout::Row}), {}, {});
        user.label("Model", {2, 2});
        user.input("Model", ent->model_name, 32);
        user.end_generic();

        user.begin_generic(make_auto({AutoLayout::Row}), {}, {});
        user.label("InterScr", {2, 2});
        user.input("InterScr", ent->interact_script, 32);
        user.end_generic();

        user.begin_generic(make_auto({AutoLayout::Row}), {}, {});
        user.label("Name", {2, 2});
        user.input("Name", ent->name, 32);
        user.end_generic();

        if (user.button("Remove"))
        {
            rem = true;
        }
    }
    else
    {
        if (user.button("Create"))
        {
            this->create_entity = true;
        }

        user.label("Point at an entity or create one.", {2, 2});
    }

    user.end_generic();
    return rem;
}

int GuiEditor::show(World &world, ResourceSpec &res)
{
    this->create_entity = false;
    int ui_mode = 2;
    UiUser user = UiUser::init(*this->ui_state);
    user.begin_pass();
    user.begin_generic(make_auto({AutoLayout::Column}), {}, {});

    user.begin_generic(make_auto({AutoLayout::Row}), {}, {});
    tab = user.button(tab == Tiles ? ">Tiles" : "Tiles") ? Tiles : tab;
    tab = user.button(tab == Entity ? ">Entity" : "Entity") ? Entity : tab;
    ui_mode = user.button("Exit₷") ? 1 : ui_mode;
    user.end_generic();

    TableId pointing_ent = {0};
    for (auto [id, ent] : iter(world.entities))
    {
        auto v = Vector2i{(int)round(world.camera_pos.x),
                          (int)round(world.camera_pos.y)};
        pointing_ent = v == vector2_to_vector2i(ent.pos) ? id : pointing_ent;
    }

    switch (tab)
    {
    case Tiles:
        show_tab_tiles(user, res);
        break;
    case Entity:
        if (show_tab_entity(user, world.entities.get(pointing_ent), res))
        {
            world.entities.remove(pointing_ent);
        }
        break;
    }

    user.label(
        stdstrfmt("%g %g", world.camera_pos.x, world.camera_pos.y).c_str());

    user.end_generic();
    user.end_pass();

    return ui_mode;
}
