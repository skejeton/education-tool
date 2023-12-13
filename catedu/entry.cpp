#include "entry.hpp"
#include "boxdraw.hpp"
#include "camera.hpp"
#include "catedu/sys/sg_tricks.hpp"
#include "console.hpp"
#include "enet/enet.h"
#include "math.hpp"
#include "resources/resources.hpp"
#include <cstdlib>

void render_layer(const char *s, Vector3 pos, BoxdrawRenderer &boxdraw,
                  Texture tileset)
{
    int x = 0, y = 0;
    for (int i = 0; s[i]; i++)
    {
        if (s[i] == '\n')
        {
            x = 0;
            y++;
            continue;
        }
        else if (s[i] == 'G')
        {
            boxdraw_push(&boxdraw, boxdraw_cmdtexture(
                                       box3_extrude_from_point(
                                           Vector3{float(x), 0, float(y)} + pos,
                                           {0.5f, 0.5f, 0.5f}),
                                       tileset.cropped({0, 32, 32, 32})));
            x++;
        }
        else if (s[i] == '#')
        {
            boxdraw_push(&boxdraw, boxdraw_cmdtexture(
                                       box3_extrude_from_point(
                                           Vector3{float(x), 0, float(y)} + pos,
                                           {0.5f, 0.5f, 0.5f}),
                                       tileset.cropped({64, 32, 32, 32})));
            x++;
        }
        else if (s[i] == '_')
        {
            boxdraw_push(
                &boxdraw,
                boxdraw_cmdtexture(box3_extrude_from_point(
                                       Vector3{float(x), -0.45, float(y)} + pos,
                                       {0.5f, 0.05f, 0.5f}),
                                   tileset.cropped({32, 32, 32, 32})));
            x++;
        }
        else
        {
            x++;
        }
    }
}

void show_menu_animation(Entry *entry)
{
    const float width = sapp_widthf();
    const float height = sapp_heightf();

    static float time = 0;
    time += sapp_frame_duration();

    Camera camera = Camera::init(45);
    camera.set_aspect(width / height);
    camera.move(0, 10, -10);
    camera.rotate(0, -45);
    camera.rotate_around({0, 0, 0}, -time * 5, 0);

    const char *l0 = "GGGGGGGGG\n"
                     "GGGGGGGGG\n"
                     "GGGGGGGGG\n"
                     "GGGGGGGGG\n"
                     "GGGGGGGGG\n"
                     "GGGGGGGGG\n"
                     "GGGGGGGGG\n"
                     "GGGGGGGGG\n"
                     "GGGGGGGGG\n";
    const char *l1 = "###_###\n"
                     "#_____#\n"
                     "#_____#\n"
                     "#_____#\n"
                     "#_____#\n"
                     "#_____#\n"
                     "#######\n";
    const char *l2 = "### ###\n"
                     "#     #\n"
                     "#     #\n"
                     "#     #\n"
                     "#     #\n"
                     "#     #\n"
                     "#######\n";
    const char *l3 = "# # # #\n"
                     "       \n"
                     "#     #\n"
                     "       \n"
                     "#     #\n"
                     "       \n"
                     "# # # #\n";
    render_layer(l0, {-4, 0, -4}, entry->boxdraw_renderer, entry->res.tileset);
    render_layer(l1, {-3, 1, -3}, entry->boxdraw_renderer, entry->res.tileset);
    render_layer(l2, {-3, 2, -3}, entry->boxdraw_renderer, entry->res.tileset);
    render_layer(l3, {-3, 3, -3}, entry->boxdraw_renderer, entry->res.tileset);

    boxdraw_flush(&entry->boxdraw_renderer, camera.vp);
}

void show_debug(Entry *entry)
{
    if (entry->input_state.key_states[SAPP_KEYCODE_LEFT].held)
    {
        move_player(entry->world_state, {-0.05, 0});
    }
    if (entry->input_state.key_states[SAPP_KEYCODE_RIGHT].held)
    {
        move_player(entry->world_state, {0.05, 0});
    }
    if (entry->input_state.key_states[SAPP_KEYCODE_UP].held)
    {
        move_player(entry->world_state, {0, 0.05});
    }
    if (entry->input_state.key_states[SAPP_KEYCODE_DOWN].held)
    {
        move_player(entry->world_state, {0, -0.05});
    }

    const float width = sapp_widthf();
    const float height = sapp_heightf();

    Camera camera = Camera::init(45);
    camera.set_aspect(width / height);
    Rect player_rect = world_state_player_rect(entry->world_state);

    camera.move(0 + player_rect.pos.x, 10, -3 + player_rect.pos.y);

    camera.rotate(0, -75);

    render_physics_world_via_boxdraw(entry->world_state.physics,
                                     entry->boxdraw_renderer);

    boxdraw_flush(&entry->boxdraw_renderer, camera.vp);
}

void Entry::frame(void)
{
    switch (ui_mode)
    {
    case 0: // Debug
        show_debug(this);
        break;
    case 1: // Main Menu
        show_menu_animation(this);
        break;
    case 2: // Editor
    case 3: // Gameplay
        break;
    }

    this->input_state.update();
    sg_commit();
}

void Entry::cleanup(void)
{
    main_menu.deinit();
    res.deinit();
    boxdraw_destroy(&this->boxdraw_renderer);
    sg_tricks_deinit();
    sg_shutdown();
    enet_deinitialize();
}

void Entry::init()
{
    console_create_or_bind_existing();

    sg_desc desc = {};
    desc.context = sapp_sgcontext();
    desc.logger.func = slog_func;
    sg_setup(&desc);
    sg_tricks_init();

    res = load_resource_spec("./assets/tileset.png");
    WorldPrototype world_prototype = {"XXXXXXXXXXXXXXXX"
                                      "X      X       X"
                                      "X      X       X"
                                      "X      X       X"
                                      "X      X       X"
                                      "X    XXXXX     X"
                                      "X    X   X     X"
                                      "X    X P       X"
                                      "XX  XX         X"
                                      "X          D   X"
                                      "X              X"
                                      "X    D   D     X"
                                      "X              X"
                                      "X              X"
                                      "X              X"
                                      "X              "};

    world_state = world_prototype_to_world_state(world_prototype);
    enet_initialize();

    ui_state = UiState::init("./assets/Roboto-Regular.ttf");
    main_menu = GuiMainMenu::init(&ui_state);
    editor = GuiEditor::init(&ui_state);
    game_gui = GuiGame::init(&ui_state);
    this->boxdraw_renderer = boxdraw_create();
}

void Entry::input(const sapp_event *event)
{
    if (ui_state.feed_event(event))
    {
        return;
    }

    this->input_state.pass_event(event);
}
