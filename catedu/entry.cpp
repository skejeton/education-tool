#include "entry.hpp"
#include "catedu/core/math/math.hpp"
#include "catedu/misc/camera_input.hpp"
#include "catedu/rendering/3d/camera.hpp"
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

void umka_begin_window(UmkaStackSlot *params, UmkaStackSlot *result)
{
    assert(entry_ptr->ui_user);

    WindowInfo info = {};

    Rect sr = sapp_screen_rect_scaled(sapp_dpi_scale());
    info.rect.siz.y = params[0].realVal;
    info.rect.siz.x = params[1].realVal;
    info.rect.pos.x = (sr.siz.x - info.rect.siz.x) / 2;
    info.rect.pos.y = (sr.siz.y - info.rect.siz.y) / 2;
    info.title = (const char *)params[2].ptrVal;

    result->uintVal = begin_show_window(*entry_ptr->ui_user, info);

    AutoLayoutElement element = {};
    element.layout.type = AutoLayout::Row;
    entry_ptr->ui_user->begin_generic(element, {}, {});
}

void umka_end_window(UmkaStackSlot *params, UmkaStackSlot *result)
{
    assert(entry_ptr->ui_user);

    entry_ptr->ui_user->end_generic();

    end_show_window(*entry_ptr->ui_user);
}

void umka_msgbox(UmkaStackSlot *params, UmkaStackSlot *result)
{
    assert(entry_ptr->ui_user);

    char *text = (char *)params[0].ptrVal;
    char *title = (char *)params[1].ptrVal;
    typedef UmkaDynArray(char *) CharArr;
    CharArr *buttons = (CharArr *)params[2].ptrVal;
    int length = umkaGetDynArrayLen(buttons);
    int type = params[3].uintVal;

    const char *ptrs[32] = {0};
    for (int i = 0; i < std::min(31, length); i++)
    {
        ptrs[i] = buttons->data[i];
    }

    result->intVal =
        msgbox(*entry_ptr->ui_user, title, text, MsgBoxType(type), ptrs);
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
    umkaAddFunc(entry.umka, "_msgbox", &umka_msgbox);
    umkaAddFunc(entry.umka, "_begin_window", &umka_begin_window);
    umkaAddFunc(entry.umka, "_end_window", &umka_end_window);

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
        if (ui_mode == MENU_EDITOR)
        {
            READ_FILE_TEMP(world, "assets/world.dat",
                           { scene = Scene::load(world); });
        }
        break;
    case MENU_EDITOR:
        if (editor.show(this->boxdraw_renderer, this->res, this->scene,
                        &this->ui_user, this->umka, &reload_module))
        {
            ui_mode = MENU_MAIN_MENU;
            this->scene.deinit();
        }
        break;
    case MENU_GAME:
        break;
    }

    if (reload_module)
    {
        reload_umka(*this);
    }
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

    if (ui_mode == MENU_EDITOR)
    {
        READ_FILE_TEMP(world, "assets/world.dat",
                       { scene = Scene::load(world); });
    }

    this->boxdraw_renderer = boxdraw_create();
}

void Entry::input(const sapp_event *event)
{
    if (event->type == SAPP_EVENTTYPE_QUIT_REQUESTED)
    {
        if (this->editor.dirty)
        {
            sapp_cancel_quit();
            this->editor.exit_requested = true;
        }
    }

    if (ui_state.feed_event(event))
    {
        return;
    }
}
