#include "entry.hpp"
#include "boxdraw.hpp"
#include "catedu/core/math/point_intersect.hpp"
#include "catedu/sys/sg_tricks.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/resources/load_image.hpp"
#include "enet/enet.h"
#include "math.hpp"
#include "resources/resources.hpp"
#include <cstdlib>
#include <filesystem>

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
                             entry->res.tileset.cropped({0, 32, 32, 32})));
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
                        entry->res.tileset.cropped({64, 32, 32, 32})));
                x++;
            }
            else
            {
                boxdraw_push(&entry->boxdraw_renderer,
                             boxdraw_cmdtexture(
                                 box3_extrude_from_point(
                                     {float(x - 3), 0.55, float(y - 3)},
                                     {0.5f, 0.05f, 0.5f}),
                                 entry->res.tileset.cropped({32, 32, 32, 32})));
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
                        entry->res.tileset.cropped({64, 32, 32, 32})));
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
                        boxdraw_cmdtexture(
                            box3_extrude_from_point(
                                {float(x - 3), 3, float(y - 3)},
                                {0.5f, 0.5f, 0.5f}),
                            entry->res.tileset.cropped({64, 32, 32, 32})));
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
    if (entry->zoomout)
    {
        camera.move(0 + entry->world.camera_pos.x, 10,
                    -6 + entry->world.camera_pos.y);
    }
    else
    {
        camera.move(0 + entry->world.camera_pos.x, 7,
                    -4 + entry->world.camera_pos.y);
    }

    camera.rotate(0, -55);

    entry->world.render(entry->res, entry->boxdraw_renderer);

    boxdraw_flush(&entry->boxdraw_renderer, camera.vp);
}

void save_world(Entry *entry)
{
    FILE *f = fopen("user/world.dat", "wb");
    assert(f);

    fwrite(entry->world.middle, sizeof(entry->world.middle), 1, f);
    fwrite(entry->world.ground, sizeof(entry->world.ground), 1, f);

    size_t entity_count = entry->world.entities.count;
    fwrite(&entity_count, sizeof(size_t), 1, f);

    auto it = TableIterator<WorldEntity>::init(&entry->world.entities);

    for (; it.going(); it.next())
    {
        auto ent = it.table->get_assert(it.id);

        fwrite(&ent, sizeof(WorldEntity), 1, f);
    }

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

    fread(entry->world.middle, sizeof(entry->world.middle), 1, f);
    fread(entry->world.ground, sizeof(entry->world.ground), 1, f);

    size_t entity_count = 0;
    fread(&entity_count, sizeof(size_t), 1, f);

    for (size_t i = 0; i < entity_count; i++)
    {
        WorldEntity ent = {};
        fread(&ent, sizeof(WorldEntity), 1, f);

        entry->world.entities.allocate(ent);
    }

    fclose(f);
}

void Entry::frame(void)
{
    this->world.if_grass = this->script_data.activate_grass;
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
            this->script_data = {};
            this->script_data.currency = 900000;
            auto it = TableIterator<WorldEntity>::init(&this->world.entities);
            for (; it.going(); it.next())
            {
                auto ent = it.table->get_assert(it.id);
                if (strcmp(ent.id, "player") == 0)
                {
                    target_camera_pos = ent.pos;
                }
            }
        }
        break;
    case 1: // Editor
        this->world.is_editor = true;
        show_editor(this);
        ui_mode = this->editor.show(this->world, this->res);
        if (this->editor.create_entity)
        {
            WorldEntity ent = {};
            ent.pos = target_camera_pos;

            this->world.entities.allocate(ent);
        }
        if (ui_mode == 0)
        {
            save_world(this);
        }
        break;
    case 2: // Gameplay
        this->world.is_editor = false;
        show_editor(this);
        {
            char *script;
            ui_mode = this->game_gui.show(script_data.currency,
                                          script_data.reals, &script);
            if (script)
            {
                ScriptEvent sevent = {};
                sevent.script_name = script;
                sevent.any = true;
                this->script_data.activate_dialog = false;
                this->script_data.dialog = {};
                this->script_data.backtrack = false;
                this->script_data.target_pos = target_camera_pos;
                printf("script: %s\n", script);
                run_script(this->script_data, sevent);
                if (this->script_data.activate_dialog)
                {
                    this->game_gui.show_dialog(this->script_data.dialog);
                }
                this->target_camera_pos = this->script_data.target_pos;
            }
        }
        break;
    }

    sg_commit();
}

void Entry::cleanup(void)
{
    save_world(this);
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

    enet_initialize();

    load_world(this);
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

    if (event->type == SAPP_EVENTTYPE_KEY_DOWN)
    {
        Vector2 camera_prev = target_camera_pos;
        bool just_moved = false;
        if (ui_mode == 1 || (ui_mode == 2 && !this->game_gui.dialog.open))
        {
            if (event->key_code == SAPP_KEYCODE_LEFT)
            {
                target_camera_pos.x -= 1;
                just_moved = true;
            }
            if (event->key_code == SAPP_KEYCODE_RIGHT)
            {
                target_camera_pos.x += 1;
                just_moved = true;
            }
            if (event->key_code == SAPP_KEYCODE_UP)
            {
                target_camera_pos.y += 1;
                just_moved = true;
            }
            if (event->key_code == SAPP_KEYCODE_DOWN)
            {
                target_camera_pos.y -= 1;
                just_moved = true;
            }
        }

        size_t t = this->world.middle[int(target_camera_pos.x) + 32]
                                     [int(target_camera_pos.y) + 32];

        // FIXME: Quick hack to check if the tile is there, otherwise get_assert
        //        will crash the program. Should be a better way to do this.
        if (ui_mode == 2 && res.tiles.get({t}) &&
            res.tiles.get_assert({t}).if_obstacle)
        {
            target_camera_pos = camera_prev;
        }
        if (event->key_code == SAPP_KEYCODE_Z)
        {
            this->zoomout = !this->zoomout;
        }
        if (event->key_code == SAPP_KEYCODE_X)
        {
            this->game_gui.show_dialog(
                {"Hello",
                 "World",
                 "Bogos binted",
                 {{"OK", "ok"}, {"Cancel", "cancel"}, {"asdf", nullptr}}});
        }

        if (ui_mode == 1 && event->key_code == SAPP_KEYCODE_SPACE)
        {
            // FIXME: Same problem with get_assert as above.
            SpecTile *tile = res.tiles.get(editor.selection);
            if (tile == nullptr || tile->if_ground)
            {
                this->world.ground[int(target_camera_pos.x) + 32]
                                  [int(target_camera_pos.y) + 32] =
                    this->editor.selection.id;
            }

            if (tile == nullptr || !tile->if_ground)
            {
                this->world.middle[int(target_camera_pos.x) + 32]
                                  [int(target_camera_pos.y) + 32] =
                    this->editor.selection.id;
            }

            return;
        }
        if (ui_mode == 2)
        {
            auto it = TableIterator<WorldEntity>::init(&this->world.entities);
            for (; it.going(); it.next())
            {
                auto ent = it.table->get_assert(it.id);
                if (vector2_to_vector2i(ent.pos) ==
                    vector2_to_vector2i(target_camera_pos))
                {
                    ScriptEvent sevent = {};
                    sevent.script_name = ent.interact_script;
                    sevent.interacted = event->key_code == SAPP_KEYCODE_SPACE;
                    sevent.walked_on = just_moved;
                    sevent.any = true;
                    if (!sevent.interacted && !sevent.walked_on)
                    {
                        break;
                    }
                    this->script_data.activate_dialog = false;
                    this->script_data.dialog = {};
                    this->script_data.backtrack = false;
                    printf("script: %s\n", sevent.script_name);
                    run_script(this->script_data, sevent);
                    if (this->script_data.activate_dialog)
                    {
                        this->game_gui.show_dialog(this->script_data.dialog);
                    }

                    if (this->script_data.backtrack)
                    {
                        target_camera_pos = camera_prev;
                    }
                }
            }
        }
    }
}
