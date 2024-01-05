#include "entry.hpp"
#include "camera.hpp"
#include "catedu/sys/sg_tricks.hpp"
#include "console.hpp"
#include "math.hpp"
#include "resources/resources.hpp"
#include <cstdlib>
#include "catedu/core/fs/read_file_temp.hpp"
#include "catedu/camera_input.hpp"

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

    entry->scene.render(entry->boxdraw_renderer, entry->res);

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
}

void Entry::init()
{
    console_create_or_bind_existing();
    sg_tricks_init();

    res = load_resource_spec("./assets/tileset.png");

    ui_state = UiState::init("./assets/Roboto-Regular.ttf");
    main_menu = GuiMainMenu::init(&ui_state);
    editor = GuiEditor::init(&ui_state);
    game_gui = GuiGame::init(&ui_state);

    READ_FILE_TEMP(file, "./data/world.dat", {
        scene = LegacyScene::init(file);
    })

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
