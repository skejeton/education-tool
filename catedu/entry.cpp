#include "entry.hpp"
#include "catedu/core/math/math.hpp"
#include "catedu/misc/camera_input.hpp"
#include "catedu/rendering/3d/camera.hpp"
#include "catedu/sys/fs/read_file_temp.hpp"
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
    uint32_t color = params[0].uintVal;
    char *string = (char *)params[1].ptrVal;

    // Color is in the format 0xRRGGBBAA

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

void umka_getobj(UmkaStackSlot *params, UmkaStackSlot *result)
{
    char *id = (char *)params[2].ptrVal;
    double *x = (double *)params[1].ptrVal;
    double *y = (double *)params[0].ptrVal;

    ObjectId obj_id = entry_ptr->editor.playtest.scene.find_object(id);

    if (obj_id == NULL_ID)
    {
        result->uintVal = 0;
        return;
    }

    Object *obj = entry_ptr->editor.playtest.scene.get_object(obj_id);

    if (obj->type != Object::Entity)
    {
        result->uintVal = 0;
        return;
    }

    PhysicsBody *body = entry_ptr->editor.playtest.scene.physics.bodies.get(
        obj->entity.body_id);
    assert(body);
    *x = body->area.pos.x;
    *y = body->area.pos.y;

    result->uintVal = 1;
}

void umka_setobj(UmkaStackSlot *params, UmkaStackSlot *result)
{
    char *id = (char *)params[0].ptrVal;
    double x = params[1].realVal;
    double y = params[2].realVal;

    ObjectId obj_id = entry_ptr->editor.playtest.scene.find_object(id);

    if (obj_id == NULL_ID)
    {
        result->uintVal = 0;
        return;
    }

    Object *obj = entry_ptr->editor.playtest.scene.get_object(obj_id);

    if (obj->type != Object::Entity)
    {
        result->uintVal = 0;
        return;
    }

    PhysicsBody *body = entry_ptr->editor.playtest.scene.physics.bodies.get(
        obj->entity.body_id);
    assert(body);
    body->area.pos.x = x;
    body->area.pos.y = y;
    obj->entity.pos.x = x;
    obj->entity.pos.y = y;

    result->uintVal = 1;
}

void umka_sayobj(UmkaStackSlot *params, UmkaStackSlot *result)
{
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
    umkaAddFunc(entry.umka, "_msgbox", &umka_msgbox);
    umkaAddFunc(entry.umka, "_begin_window", &umka_begin_window);
    umkaAddFunc(entry.umka, "_end_window", &umka_end_window);
    umkaAddFunc(entry.umka, "_getobj", &umka_getobj);
    umkaAddFunc(entry.umka, "_setobj", &umka_setobj);
    umkaAddFunc(entry.umka, "_sayobj", &umka_sayobj);

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

    switch (ui_mode)
    {
    case MENU_DEBUG:
        break;
    case MENU_MAIN_MENU:
        ui_mode = main_menu.show(user);
        if (ui_mode == MENU_EDITOR)
        {
            this->editor = GuiEditor::init(&this->ui_state);

            FILE *f = fopen("assets/world.dat", "rb");
            if (f != NULL)
            {
                READ_FILE_TEMP(world, "assets/world.dat",
                               { scene = Scene::load(world); });
                fclose(f);
            }
        }
        break;
    case MENU_EDITOR:
        if (editor.show(this->renderer, this->res, this->scene, user,
                        this->umka, &reload_module))
        {
            ui_mode = MENU_MAIN_MENU;
            returned_to_menu = true;
        }
        break;
    case MENU_GAME:
        break;
    }

    this->ui_user = NULL;
    user.end_pass();

    if (returned_to_menu)
    {
        this->editor.deinit();
        this->scene.deinit();
    }

    if (reload_module)
    {
        reload_umka(*this);
    }
}

void Entry::cleanup(void)
{
    main_menu.deinit();
    if (ui_mode == MENU_EDITOR)
    {
        editor.deinit();
        scene.deinit();
    }
    res.deinit();
    ui_state.deinit();
    this->renderer.deinit();
    sg_tricks_deinit();

    umkaFree(this->umka);
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

    if (ui_mode == MENU_EDITOR)
    {
        editor = GuiEditor::init(&ui_state);

        FILE *f = fopen("assets/world.dat", "rb");
        if (f != NULL)
        {
            READ_FILE_TEMP(world, "assets/world.dat",
                           { scene = Scene::load(world); });
            fclose(f);
        }
    }

    this->renderer = catedu::pbr::Renderer::init();
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
