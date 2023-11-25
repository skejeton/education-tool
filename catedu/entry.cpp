#include "entry.hpp"
#include "boxdraw.hpp"
#include "catedu/core/math/point_intersect.hpp"
#include "catedu/sys/sg_tricks.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/resources/load_image.hpp"
#include "enet/enet.h"
#include "math.hpp"
#include <cstdlib>
#include <filesystem>

Texture tex;

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

    for (int i = -4; i < 5; i++)
    {
        for (int j = -4; j < 5; j++)
        {
            boxdraw_push(&entry->boxdraw_renderer,
                         boxdraw_cmdtexture(
                             box3_extrude_from_point({float(i), 0, float(j)},
                                                     {0.5f, 0.5f, 0.5f}),
                             tex.cropped({0, 32, 32, 32})));
        }
    }

    char *s = "### ###\n"
              "#     #\n"
              "#     #\n"
              "#     #\n"
              "#     #\n"
              "#     #\n"
              "#######\n";

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
            else if (s[i] == '#')
            {
                boxdraw_push(
                    &entry->boxdraw_renderer,
                    boxdraw_cmdtexture(
                        box3_extrude_from_point({float(x - 3), 1, float(y - 3)},
                                                {0.5f, 0.5f, 0.5f}),
                        tex.cropped({64, 32, 32, 32})));
                x++;
            }
            else
            {
                boxdraw_push(
                    &entry->boxdraw_renderer,
                    boxdraw_cmdtexture(box3_extrude_from_point(
                                           {float(x - 3), 0.55, float(y - 3)},
                                           {0.5f, 0.05f, 0.5f}),
                                       tex.cropped({32, 32, 32, 32})));
                x++;
            }
        }
    }

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
            else if (s[i] == '#')
            {
                boxdraw_push(
                    &entry->boxdraw_renderer,
                    boxdraw_cmdtexture(
                        box3_extrude_from_point({float(x - 3), 2, float(y - 3)},
                                                {0.5f, 0.5f, 0.5f}),
                        tex.cropped({64, 32, 32, 32})));
                x++;
            }
            else
            {
                x++;
            }
        }
    }

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
            else if (s[i] == '#')
            {
                if ((x + y) % 2 == 0)
                    boxdraw_push(
                        &entry->boxdraw_renderer,
                        boxdraw_cmdtexture(box3_extrude_from_point(
                                               {float(x - 3), 3, float(y - 3)},
                                               {0.5f, 0.5f, 0.5f}),
                                           tex.cropped({64, 32, 32, 32})));
                x++;
            }
            else
            {
                x++;
            }
        }
    }

    boxdraw_flush(&entry->boxdraw_renderer, camera.vp);
}

void show_editor(Entry *entry)
{
    const float width = sapp_widthf();
    const float height = sapp_heightf();

    static float time = 0;
    time += sapp_frame_duration();

    Camera camera = Camera::init(45);
    camera.set_aspect(width / height);
    camera.move(0 + entry->world.camera_pos.x, 10,
                -6 + entry->world.camera_pos.y);
    camera.rotate(0, -55);

    entry->world.render(entry->boxdraw_renderer);

    boxdraw_flush(&entry->boxdraw_renderer, camera.vp);
}

void save_world(Entry *entry)
{
    FILE *f = fopen("user/world.dat", "wb");
    assert(f);

    fwrite(entry->world.data, sizeof(entry->world.data), 1, f);
    fclose(f);
}

void load_world(Entry *entry)
{
    FILE *f = fopen("user/world.dat", "rb");
    if (!f)
    {
        return;
    }
    fprintf(stderr, "loading world.");

    fread(entry->world.data, sizeof(entry->world.data), 1, f);
    fclose(f);
}

void Entry::frame(void)
{
    this->world.camera_pos.x +=
        (target_camera_pos.x - this->world.camera_pos.x) *
        sapp_frame_duration() * 40;
    this->world.camera_pos.y +=
        (target_camera_pos.y - this->world.camera_pos.y) *
        sapp_frame_duration() * 40;
    switch (ui_mode)
    {
    case 0: // Main Menu
        show_menu_animation(this);
        ui_mode = this->main_menu.show();
        if (ui_mode == 2)
        {
            for (int i = 0; i < 64; i++)
            {
                for (int j = 0; j < 64; j++)
                {
                    if (this->world.data[i][j] == 4)
                    {
                        target_camera_pos = {float(i - 32), float(j - 32)};
                        this->world.camera_rot = 0;
                    }
                }
            }
        }
        break;
    case 1: // Editor
        this->world.is_editor = true;
        show_editor(this);
        ui_mode = this->editor.show(this->world.camera_pos);
        if (ui_mode == 0)
        {
            save_world(this);
        }
        break;
    case 2: // Editor
        this->world.is_editor = false;
        show_editor(this);
        ui_mode = this->game_gui.show();
        break;
    }

    sg_commit();
}

void Entry::cleanup(void)
{
    save_world(this);
    main_menu.deinit();
    tex.deinit();
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
    tex = Texture::init("./assets/test_spritesheet_01.png");

    enet_initialize();

    world = World::init(tex);
    load_world(this);
    ui_state = UiState::init("./assets/Roboto-Regular.ttf");
    main_menu = GuiMainMenu::init(&ui_state);
    editor = GuiEditor::init(&ui_state);
    game_gui = GuiGame::init(&ui_state);
    this->boxdraw_renderer = boxdraw_create();
}

void Entry::input(const sapp_event *event)
{
    if (event->key_code == SAPP_KEYCODE_1)
    {
        this->game_gui.show_dialogue(
            "Welcome onboard! Please hand your passport.");
        return;
    }

    if (event->key_code == SAPP_KEYCODE_2)
    {
        this->game_gui.show_dialogue("Great! What is your name traveller?", "",
                                     this->name);
        return;
    }

    if (event->key_code == SAPP_KEYCODE_3)
    {
        this->game_gui.show_dialogue(
            stdstrfmt("Welcome %s! Proceed to the door on the left.",
                      this->name)
                .c_str());
        return;
    }

    if (event->key_code == SAPP_KEYCODE_4)
    {
        this->world.data[-3 + 32][-1 + 32] = 2;
        this->world.data[-2 + 32][-1 + 32] = 2;

        return;
    }

    if (ui_state.feed_event(event))
    {
        return;
    }

    if (event->type == SAPP_EVENTTYPE_KEY_DOWN)
    {
        Vector2 camera_prev = target_camera_pos;
        if (event->key_code == SAPP_KEYCODE_LEFT)
        {
            target_camera_pos.x -= 1;
        }
        if (event->key_code == SAPP_KEYCODE_RIGHT)
        {
            target_camera_pos.x += 1;
        }
        if (event->key_code == SAPP_KEYCODE_UP)
        {
            target_camera_pos.y += 1;
        }
        if (event->key_code == SAPP_KEYCODE_DOWN)
        {
            target_camera_pos.y -= 1;
        }

        int t = this->world.data[int(target_camera_pos.x) + 32]
                                [int(target_camera_pos.y) + 32];
        if (ui_mode == 2 && (t == 1 || t == 3 || t == 5 || t == 6 || t == 7))
        {
            target_camera_pos = camera_prev;
        }
        if (event->key_code == SAPP_KEYCODE_D)
        {
            this->world.camera_rot += 1;
            if (this->world.camera_rot > 3)
            {
                this->world.camera_rot = 0;
            }
            return;
        }
        if (event->key_code == SAPP_KEYCODE_A)
        {
            this->world.camera_rot -= 1;
            if (this->world.camera_rot < 0)
            {
                this->world.camera_rot = 3;
            }
            return;
        }
        if (event->key_code == SAPP_KEYCODE_ESCAPE)
        {
            this->game_gui.dialog_open = false;
            return;
        }
        if (event->key_code == SAPP_KEYCODE_Q)
        {
            this->game_gui.dialog_open = !this->game_gui.dialog_open;
            return;
        }
        if (ui_mode == 1 && event->key_code == SAPP_KEYCODE_SPACE)
        {
            this->world.data[int(target_camera_pos.x) + 32]
                            [int(target_camera_pos.y) + 32] =
                this->editor.selection;
            return;
        }
    }
}
