#include "entry.hpp"
#include "catedu/gui/transition/transition.hpp"
#include "catedu/resources/resources.hpp"
#include "catedu/scene/world_file.hpp"
#include "catedu/sys/sg_tricks.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/widgets.hpp"

void show_debug_panel(UiUser &user, RuntimeMode &mode)
{
    user.collection(AutoLayout::Column, [&]() {
        label(user, "Debug panel", {2, 2}, UiMakeBrush::make_solid(0xFFFFFFFF));

        label(user, "Select mode", {1, 1}, UiMakeBrush::make_solid(0xFFFFFF88));

        user.collection(AutoLayout::Row, [&]() {
            if (button(user, "Menu"))
            {
                mode = RuntimeMode::menu;
            }
            if (button(user, "Editor"))
            {
                mode = RuntimeMode::editor;
            }
            if (button(user, "Game"))
            {
                mode = RuntimeMode::game;
            }
        });
    });
}

void Entry::frame(void)
{
    bool reload_module = false;

    UiUser user = UiUser::init(ui_state);
    user.begin_pass();

    bool returned_to_menu = false;

    switch (mode)
    {
    case RuntimeMode::debug:
        show_debug_panel(user, mode);
        if (mode == RuntimeMode::editor)
        {
            this->editor = GuiEditor::init(&this->ui_state);
        }
        break;
    case RuntimeMode::menu:
        if (this->main_menu.show(user, transition, this->panorama.world,
                                 this->renderer, this->res))
        {
            this->editor = GuiEditor::init(&this->ui_state);
            this->mode = RuntimeMode::editor;
        }
        break;
    case RuntimeMode::editor:
    case RuntimeMode::game:
        if (editor.show(user, transition, this->renderer, this->res))
        {
            mode = RuntimeMode::menu;
            returned_to_menu = true;
        }
        break;
    }

    transition.show(user, user.state->input);

    user.end_pass();

    // FIXME: We need to defer `editor.deinit`, because `deinit` removes the
    // target images, which are still in use during the UI pass.
    if (returned_to_menu)
    {
        this->editor.deinit();
    }

    if (reload_module)
    {
        this->umka_module.reload();
    }
}

void Entry::cleanup(void)
{
    main_menu.deinit();
    if (mode == RuntimeMode::editor)
    {
        editor.deinit();
    }
    res.deinit();
    ui_state.deinit();
    this->renderer.deinit();
    sg_tricks_deinit();

    this->umka_module.destroy();

    this->panorama.destroy();

    // NOTE: Some memory will show as "leak" because this function is called
    // before the sokol deinitializers are called.
    // TODO: Trace the memory allocations before initialization and correctly
    // display them.
    fprintf(stderr, "Memory leaked: %zu\n",
            ALLOCATOR_MALLOC.tracer.total_bytes_allocated);
}

void Entry::init()
{
    this->umka_module = UmkaModule::create(&this->umka_bindings_data);

    sg_tricks_init();

    res = load_resource_spec("./assets/tileset.png");

    ui_state = UiState::init("./assets/Roboto-Regular.ttf",
                             "./assets/Roboto-Bold.ttf", sapp_dpi_scale());

    main_menu = GuiMainMenu::init(&ui_state);

    if (mode == RuntimeMode::editor)
    {
        editor = GuiEditor::init(&ui_state);
    }

    panorama = WorldFile::load("assets/world.dat");

    transition = GuiTransition::create();

    this->renderer = catedu::pbr::Renderer::init();
}

void Entry::input(const sapp_event *event)
{
    if (event->type == SAPP_EVENTTYPE_QUIT_REQUESTED)
    {
        if (this->editor.dispatcher.dirty)
        {
            sapp_cancel_quit();
            this->editor.exit_requested = true;
        }
    }

    if (event->type == SAPP_EVENTTYPE_UNFOCUSED)
    {
        this->ui_state.input.reset();
    }

    if (ui_state.feed_event(event))
    {
        return;
    }
}
