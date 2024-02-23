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
    this->boxdraw_renderer.pass_action.colors->clear_value = {0, 0, 0.5, 1};
    switch (ui_mode)
    {
    case 0: // Debug
        show_debug(this);
        break;
    case 1: // Main Menu
        this->boxdraw_renderer.pass_action.colors->clear_value = {0, 0, 0, 1};
        boxdraw_flush(&this->boxdraw_renderer, this->editor.camera.vp);
        ui_mode = main_menu.show();
        break;
    case 2: // Editor
        if (editor.show(this->boxdraw_renderer, this->res, this->scene,
                        this->input_state))
        {
            ui_mode = 1;
        }
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

    ui_state = UiState::init("./assets/Roboto-Regular.ttf",
                             "./assets/Roboto-Bold.ttf", sapp_dpi_scale());
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
        // Invalidate the input state if the event was consumed by the UI.
        this->input_state = {};
        return;
    }

    this->input_state.pass_event(event);
}
