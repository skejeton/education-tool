#include "entry.hpp"
#include "catedu/core/math/math.hpp"
#include "catedu/misc/camera_input.hpp"
#include "catedu/rendering/3d/camera.hpp"
#include "catedu/scene/legacy_scene.hpp"
#include "catedu/sys/console.hpp"
#include "catedu/sys/fs/read_file_temp.hpp"
#include "catedu/sys/sg_tricks.hpp"
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
    uint32_t color = params[0].uintVal;
    char *string = (char *)params[1].ptrVal;

    // Color is in the format 0xRRGGBBAA
    Vector4 color_vec = {
        (float)((color >> 24) & 0xFF) / 255.0f,
        (float)((color >> 16) & 0xFF) / 255.0f,
        (float)((color >> 8) & 0xFF) / 255.0f,
        (float)((color >> 0) & 0xFF) / 255.0f,
    };

    assert(entry_ptr->ui_user);

    label(*entry_ptr->ui_user, string, {1, 1},
          UiMakeBrush::make_solid(color_vec));
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

    if (!umkaInit(entry.umka, "assets/main.um", NULL, 1024 * 1024, NULL, 0,
                  NULL, false, false, NULL))
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

    char *assm = umkaAsm(entry.umka);
    fprintf(stderr, "UMKA ASM: %s\n", assm);
}

void reload_umka(Entry &entry)
{
    umkaFree(entry.umka);
    load_umka(entry);
}

void Entry::frame(void)
{
    bool reload_module = false;
    this->boxdraw_renderer.pass_action.colors->clear_value = {0, 0, 0.5, 1};
    switch (ui_mode)
    {
    case MENU_DEBUG:
        break;
    case MENU_MAIN_MENU:
        this->boxdraw_renderer.pass_action.colors->clear_value = {0, 0, 0, 1};
        boxdraw_flush(&this->boxdraw_renderer, this->editor.camera.vp);
        ui_mode = main_menu.show();
        break;
    case MENU_EDITOR:
        if (editor.show(this->boxdraw_renderer, this->res, this->scene,
                        this->input_state, &this->ui_user, this->umka,
                        &reload_module))
        {
            ui_mode = MENU_MAIN_MENU;
        }
        break;
    case MENU_GAME:
        break;
    }

    if (reload_module)
    {
        reload_umka(*this);
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

    umkaFree(this->umka);
}

void Entry::init()
{
    console_create_or_bind_existing();

    entry_ptr = this;

    load_umka(*this);

    sg_tricks_init();

    res = load_resource_spec("./assets/tileset.png");

    ui_state = UiState::init("./assets/Roboto-Regular.ttf",
                             "./assets/Roboto-Bold.ttf", sapp_dpi_scale());
    main_menu = GuiMainMenu::init(&ui_state);
    editor = GuiEditor::init(&ui_state);
    game_gui = GuiGame::init(&ui_state);

    this->boxdraw_renderer = boxdraw_create();
}

void Entry::input(const sapp_event *event)
{
    if (ui_state.feed_event(event))
    {
        // Invalidate the input state if the event was consumed by the UI.
        this->input_state = {};

        // Move the mouse out of the screen to avoid the cursor being detected
        // in top left corner of the screen.
        this->input_state.mouse_pos.x = FLT_MAX;
        this->input_state.mouse_pos.y = FLT_MAX;

        return;
    }

    this->input_state.pass_event(event);
}
