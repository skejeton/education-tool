#ifndef HPP_CATEDU_GUI_EDITOR_EDITOR
#define HPP_CATEDU_GUI_EDITOR_EDITOR

#include "catedu/input.hpp"
#include "catedu/ui/user.hpp"
#include "catedu/staging/world/resource_spec.hpp"
#include "catedu/staging/world/world.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"
#include <sokol/sokol_app.h>

struct GuiEditor
{
    UiState *ui_state;
    enum Tab
    {
        Tiles,
        Entity
    } tab;
    TableId selection;
    bool create_entity;

    static GuiEditor init(UiState *ui_state);
    void deinit();

    void show_tab_tiles(UiUser &user, ResourceSpec &res);
    bool show_tab_entity(UiUser &user, WorldEntity *ent, ResourceSpec &res);
    int show(World &world, ResourceSpec &res);
};

#endif // HPP_CATEDU_GUI_EDITOR_EDITOR
