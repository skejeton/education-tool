#include "entry.hpp"
#include "catedu/core/math/math.hpp"
#include "catedu/misc/camera_input.hpp"
#include "catedu/rendering/3d/camera.hpp"
#include "catedu/scene/legacy_scene.hpp"
#include "catedu/sys/console.hpp"
#include "catedu/sys/fs/read_file_temp.hpp"
#include "catedu/sys/sg_tricks.hpp"
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
                     "G       G\n"
                     "G       G\n"
                     "G       G\n"
                     "G       G\n"
                     "G       G\n"
                     "G       G\n"
                     "G       G\n"
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

Camera camera = Camera::init(45);
void show_debug(Entry *entry)
{
    const float width = sapp_widthf();
    const float height = sapp_heightf();

    camera.set_aspect(width / height);
    camera_input_apply(&camera, &entry->input_state);

    Object *player =
        entry->scene.get_object(entry->scene.find_object("player"));
    PhysicsBody *body = entry->scene.physics.bodies.get(player->entity.body_id);
    if (entry->input_state.key_states[SAPP_KEYCODE_LEFT].held)
    {
        body->area.pos.x -= 0.1;
    }
    if (entry->input_state.key_states[SAPP_KEYCODE_RIGHT].held)
    {
        body->area.pos.x += 0.1;
    }
    if (entry->input_state.key_states[SAPP_KEYCODE_UP].held)
    {
        body->area.pos.y += 0.1;
    }
    if (entry->input_state.key_states[SAPP_KEYCODE_DOWN].held)
    {
        body->area.pos.y -= 0.1;
    }

    entry->scene.update(entry->res);
    entry->scene.render(entry->boxdraw_renderer, entry->res, true);

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
        // scamera_input_apply(&this->editor.camera, &this->input_state);
        boxdraw_flush(&this->boxdraw_renderer, this->editor.camera.vp);
        editor.show(this->boxdraw_renderer, this->res, this->scene,
                    this->input_state);
        break;
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
}

void Entry::init()
{
    console_create_or_bind_existing();
    sg_tricks_init();

    res = load_resource_spec("./assets/tileset.png");

    ui_state = UiState::init("./assets/Roboto-Regular.ttf", sapp_dpi_scale());
    main_menu = GuiMainMenu::init(&ui_state);
    editor = GuiEditor::init(&ui_state);
    game_gui = GuiGame::init(&ui_state);

    READ_FILE_TEMP(file, "./assets/world.dat",
                   { scene = LegacyScene::load_data_to_scene(file); })

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
