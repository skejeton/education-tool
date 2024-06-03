#include "entry.hpp"
#include "catedu/gui/editor/world_file.hpp"
#include "catedu/gui/transition/transition.hpp"
#include "catedu/misc/camera_input.hpp"
#include "catedu/rendering/3d/camera.hpp"
#include "catedu/sys/sg_tricks.hpp"
#include "catedu/ui/rendering/colors.hpp"
#include "catedu/ui/widgets.hpp"
#include "resources/resources.hpp"
#include <cstdlib>

Camera camera = Camera::init(45);

Entry *entry_ptr;

void on_umka_warning(UmkaError *warning)
{
    fprintf(stderr, "UMKA WARNING: %s\n", warning->msg);
}

void umka_button(UmkaStackSlot *params, UmkaStackSlot *result)
{
    char *string = (char *)params[0].ptrVal;

    assert(entry_ptr->ui_user);

    result->uintVal = button(*entry_ptr->ui_user, string);
}

void umka_label(UmkaStackSlot *params, UmkaStackSlot *result)
{
    // Color is in the format 0xRRGGBBAA
    uint32_t color = params[0].uintVal;
    char *string = (char *)params[1].ptrVal;

    assert(entry_ptr->ui_user);

    label(*entry_ptr->ui_user, string, {1, 1},
          UiMakeBrush::make_solid(to_color(color)));
}

void umka_nextrow(UmkaStackSlot *params, UmkaStackSlot *result)
{
    assert(entry_ptr->ui_user);

    entry_ptr->ui_user->end_generic();
    AutoLayoutElement element = {};
    element.layout.type = AutoLayout::Row;
    entry_ptr->ui_user->begin_generic(element, {}, {});
}

void load_umka(Entry &entry)
{
    entry.umka = umkaAlloc();

    assert(entry.umka);

    if (!umkaInit(entry.umka, "assets/script/main.um", NULL, 1024 * 1024, NULL,
                  0, NULL, false, false, NULL))
    {
        return;
    }

    umkaAddFunc(entry.umka, "button", &umka_button);
    umkaAddFunc(entry.umka, "label", &umka_label);
    umkaAddFunc(entry.umka, "nextrow", &umka_nextrow);

    if (!umkaCompile(entry.umka))
    {
        return;
    }
}

void reload_umka(Entry &entry)
{
    umkaFree(entry.umka);
    load_umka(entry);
}

void show_debug_panel(UiUser &user, RuntimeMode &mode)
{
    user.collection(AutoLayout::Column, [&]() {
        label(user, "Debug panel", {2, 2},
              UiMakeBrush::make_solid({1, 1, 1, 1}));

        label(user, "Select mode", {1, 1},
              UiMakeBrush::make_solid({1, 1, 1, 0.5}));

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
    // Clear the screen
    sg_pass pass = {0};
    pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
    pass.action.colors[0].store_action = SG_STOREACTION_STORE;
    pass.action.colors[0].clear_value = {0.0f, 0.0f, 0.0f, 1.0f};
    pass.swapchain = sglue_swapchain();
    sg_begin_pass(&pass);
    sg_end_pass();

    bool reload_module = false;

    UiUser user = UiUser::init(ui_state);
    user.begin_pass();
    this->ui_user = &user;

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

    this->ui_user = NULL;
    user.end_pass();

    if (returned_to_menu)
    {
        this->editor.deinit();
    }

    if (reload_module)
    {
        reload_umka(*this);
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

    umkaFree(this->umka);

    this->panorama.destroy();

    fprintf(stderr, "Memory leaked: %zu\n",
            ALLOCATOR_MALLOC.tracer.total_bytes_allocated);
}

void Entry::init()
{
    entry_ptr = this;

    load_umka(*this);

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
