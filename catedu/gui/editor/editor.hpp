#pragma once
#include "../debugtree/debugtree.hpp"
#include "catedu/gui/dialogeditor/dialogeditor.hpp"
#include "catedu/rendering/3d/camera.hpp"
#include "catedu/rendering/3d/pbr.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "catedu/scene/scene.hpp"
#include "catedu/sys/input.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"
#include "catedu/ui/user.hpp"
#include "playtest.hpp"
#include "stencil.hpp"
#include <sokol/sokol_app.h>

struct EditAction
{
    bool final; // if true, this action is the last in a sequence

    enum Type
    {
        None,
        CreateEntity,
        DeleteEntity,
        MoveEntity,
        PlaceTile,
        Count_,
    } type;

    union Cmd {
        struct CreateEntity
        {
            TableId entity_id;
            Object entity;
        } create_entity;
        struct DeleteEntity
        {
            TableId entity_id;
            Object entity_data;
        } delete_entity;
        struct MoveEntity
        {
            TableId entity;
            Vector2 current_pos;
            Vector2 pos;
        } move_entity;
        struct PlaceTile
        {
            TableId tilemap_entity;
            Vector2i pos;
            int prev_id; // 0 is empty
            int tile_id; // 0 is empty
        } place_tile;
    } cmd;
};

struct TilemapEdit
{
    StencilEdit stencil;
    TableId tile;
    int scroll;
};

struct SelectionState
{
    ObjTilemap *tilemap_selected;
    Vector2 position;
};

enum EditorTab
{
    EDITOR_TAB_CONFIG,
    EDITOR_TAB_BUILD,
    EDITOR_TAB_CHARACTERS,
    EDITOR_TAB_SCRIPT
};

enum Backdrop
{
    BACKDROP_VOID,
    BACKDROP_GRASS
};

struct GuiEditor
{
    bool show_help_window;
    bool playtesting;
    bool playtest_no_player;
    Playtest playtest;
    bool create_entity;
    bool placing_object;
    bool dirty;
    bool exit_requested;
    bool tried_to_return_back;
    bool show_debug;
    UiState *ui_state;
    TilemapEdit tilemap_edit;
    TableId selection;
    size_t entity_list_page;
    Camera camera;
    GuiDebugTree debug_tree;
    Vector2 object_cursor_at;
    EditAction actions[1 << 10];
    size_t action_buoy;
    size_t action_count;
    DialogEditor dialog_editor;
    EditorTab tab;
    Backdrop backdrop;

    static GuiEditor init(UiState *ui_state);
    void save(Scene &scene);
    void start_playtest(Scene &scene, bool *reload_module, void *umka);
    void stop_playtest(Playtest &playtest);

    bool show_build_mode(UiUser &user, catedu::pbr::Renderer &renderer,
                         ResourceSpec &resources, Scene &scene, void *umka,
                         bool *reload_module, SelectionState &sel);
    bool show(catedu::pbr::Renderer &renderer, ResourceSpec &resources,
              Scene &scene, UiUser &user, void *umka, bool *reload_module);
    void deinit();
};
