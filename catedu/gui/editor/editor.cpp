#include "editor.hpp"
#include "catedu/genobj/building.hpp"
#include "catedu/genobj/render.hpp"
#include "catedu/misc/camera_input.hpp"
#include "catedu/rendering/3d/pbr.hpp"
#include "catedu/ui/widgets.hpp"
#include "edit_building.hpp"
#include "offscreen.hpp"
#include "world_file.hpp"
#include <catedu/genobj/ground.hpp>
#include <catedu/genobj/player.hpp>
#include <catedu/genobj/road.hpp>
#include <umka_api.h>

bool icon_button(UiUser &user, const char *name, const char *icon,
                 Vector4 color = {1.0, 1.0, 1.0, 1.0}, float scale = 1);

AutoLayoutElement create_main_element(UiUser &user)
{
    AutoLayoutElement element = {};
    element.position = AutoLayoutPosition::Absolute;
    element.width.type = AutoLayoutDimension::Pixel;
    element.width.value = sapp_widthf() / user.state->dpi_scale;
    element.height.type = AutoLayoutDimension::Pixel;
    element.height.value = sapp_heightf() / user.state->dpi_scale;
    return element;
}

enum class RectSide
{
    Left,
    Right,
    Top,
    Bottom
};

bool rect_side_is_horizontal(RectSide &side)
{
    return side == RectSide::Bottom || side == RectSide::Top;
}

void begin_toolbar(UiUser &user, const char *name, RectSide side)
{
    float align_y = 0, align_x = 0;

    if (rect_side_is_horizontal(side))
    {
        align_y = side == RectSide::Bottom;
        align_x = 0.5;
    }
    else
    {
        align_x = side == RectSide::Right;
        align_y = 0.5;
    }

    AutoLayoutElement element = create_main_element(user);
    element.align_height = align_y;
    element.align_width = align_x;
    user.begin_generic(element, {}, {});
    user.push_id(name);

    AutoLayoutElement toolbar = {};
    toolbar.layout.type =
        rect_side_is_horizontal(side) ? AutoLayout::Row : AutoLayout::Column;
    toolbar.width.type = AutoLayoutDimension::Auto;
    toolbar.height.type = AutoLayoutDimension::Auto;
    toolbar.padding = {2, 2, 2, 2};
    toolbar.margin = {2, 2, 2, 2};
    toolbar.border = {1, 1, 1, 1};
    user.begin_generic(toolbar, UiMakeBrush::make_solid({1.0, 1.0, 1.0, 0.6}),
                       {}, user.state->element_storage.id());
}

void end_toolbar(UiUser &user)
{
    user.end_generic();
    user.pop_id();
    user.end_generic();
}

bool icon_button(UiUser &user, const char *name, const char *icon,
                 Vector4 color, float scale)
{
    AutoLayoutElement el = {};
    el.padding = {2, 2, 2, 2};
    el.margin = {2, 2, 2, 2};
    el.border = {1, 1, 1, 1};

    begin_button_frame(user, name, el, color);
    img(user, icon, {scale, scale});
    return end_button_frame(user);
}

GenResources get_genres(ResourceSpec &resources)
{
    GenResources result = {};
    result.box =
        resources.models.get(resources.find_model_by_name("cube"))->model;
    return result;
}

bool object_icon_button(UiUser &user, const char *name, SubEditor::Type type,
                        SubEditor::Type &current,
                        catedu::pbr::Renderer &renderer,
                        ResourceSpec &resources)
{
    AutoLayoutElement el = {};
    el.width = {AutoLayoutDimension::Pixel, 190};
    el.height = {AutoLayoutDimension::Pixel, 150};
    el.align_width = 0.5;
    el.align_height = 0.5;
    el.padding = {2, 2, 2, 2};
    el.margin = {2, 2, 2, 2};
    el.border = {1, 1, 1, 1};

    UiImageId id;

    Vector4 color = {1.0, 1.0, 1.0, 1.0};
    if (current == type)
    {
        color = {0.8, 1.0, 0.8, 1.0};
    }

    begin_button_frame(user, name, el, color);
    {
        Camera camera = Camera::init(5);
        camera.set_aspect(190.f / 150.f);

        if (type == SubEditor::Type::Building)
        {
            camera.move(0, 0, -20);
            camera.rotate_around({0, 0, 0}, 45, -45);
            camera.move(0, 0.5, 0);
        }
        else
        {
            camera.move(0, 0, -10);
            camera.rotate_around({0, 0, 0}, 45, -45);
            camera.move(0, 0.1, 0);
        }

        renderer.camera = camera;
        renderer.begin_pass_offscreen(offscreen_pass_action(),
                                      offscreen_alloc(id));

        GeneratedObject obj;
        switch (type)
        {
        case SubEditor::Type::Building:
            obj = genmesh_generate_building(5);
            break;
        case SubEditor::Type::Road:
            obj = genmesh_generate_road();
            break;
        case SubEditor::Type::Player:
            obj = genmesh_generate_player();
            break;
        }
        genobj_render_object(renderer, get_genres(resources), obj,
                             Matrix4::scale(0.1f));

        renderer.end_pass();
    }

    img(user, id, {1, 1});
    if (end_button_frame(user))
    {
        current = type;
        return true;
    }
    return false;
}

GuiEditor GuiEditor::init(UiState *ui_state)
{
    GuiEditor result = {};
    result.ui_state = ui_state;
    result.debug_tree = GuiDebugTree::init();
    result.dispatcher = WorldFile::load("assets/world.dat");

    printf("Init editor\n");

    offscreen_init_targets(ui_state->core);

    return result;
}

void show_build_panel(UiUser &user, GuiEditor &editor, ResourceSpec &resources,
                      catedu::pbr::Renderer &renderer)
{
    object_icon_button(user, "Building", SubEditor::Type::Building,
                       editor.sub_editor.type, renderer, resources);
    object_icon_button(user, "Road", SubEditor::Type::Road,
                       editor.sub_editor.type, renderer, resources);
    object_icon_button(user, "Player", SubEditor::Type::Player,
                       editor.sub_editor.type, renderer, resources);
}

void show_left_panel(UiUser &user, GuiEditor &editor, ResourceSpec &resources,
                     catedu::pbr::Renderer &renderer)
{
    AutoLayoutElement element = {};
    element.clip = true;
    element.width = {AutoLayoutDimension::Pixel, 200};
    element.height = {AutoLayoutDimension::Pixel,
                      sapp_screen_rect_scaled(user.state->dpi_scale).siz.y};
    user.state->element_storage.push("Left Panel", {});
    user.begin_generic(element, {}, {}, user.state->element_storage.id());

    show_build_panel(user, editor, resources, renderer);

    user.end_generic();
    user.state->element_storage.pop();
}

void show_popups(UiUser &user, GuiEditor &editor, bool &return_back)
{
    if (editor.exit_requested)
    {
        const char *options[] = {"Yes", "No", NULL};
        switch (msgbox(user, "Exit requested",
                       "Are you sure you want to exit?\nAll unsaved changes "
                       "will be lost.",
                       MsgBoxType::Warning, options))
        {
        case 0:
            editor.exit_requested = false;
            editor.dispatcher.dirty = false;
            sapp_request_quit();
            break;
        case 1:
            editor.exit_requested = false;
            break;
        }
    }

    if (return_back)
        editor.tried_to_return_back = true;

    if (editor.tried_to_return_back && editor.dispatcher.dirty)
    {
        return_back = false;
        const char *options[] = {"Yes", "No", NULL};
        switch (msgbox(user, "Return to main menu",
                       "Are you sure you want to return to the main menu?\nAll "
                       "unsaved changes will be lost.",
                       MsgBoxType::Warning, options))
        {
        case 0:
            return_back = true;
            editor.dispatcher.dirty = false;
            break;
        case 1:
            editor.tried_to_return_back = false;
            break;
        }
    }

    if (editor.show_debug)
    {
        editor.debug_tree.reset();
        editor.debug_tree.value("frees", ALLOCATOR_MALLOC.tracer.total_frees);
        editor.debug_tree.value("allocs",
                                ALLOCATOR_MALLOC.tracer.total_allocations);
        editor.debug_tree.size("bytes",
                               ALLOCATOR_MALLOC.tracer.total_bytes_allocated);
        // begin_show_window(user, info);
        editor.debug_tree.show(user);
        // end_show_window(user);
    }
}

void show_editor_controls(UiUser &user, GuiEditor &editor, bool &return_back)
{
    begin_toolbar(user, "Controls", RectSide::Bottom);

    if (icon_button(user, "Undo", "assets/gui/undo.png"))
    {
        editor.dispatcher.undo();
    }

    if (icon_button(user, "Redo", "assets/gui/redo.png"))
    {
        editor.dispatcher.redo();
    }

    if (icon_button(user, "Home", "assets/gui/home.png"))
    {
        return_back = true;
    }

    if (icon_button(user, "Playtest", "assets/gui/play.png"))
    {
        // TODO: Find a better way of handling playtest memory.
        if (editor.playtesting)
        {
            editor.playtesting = false;
            editor.playtest.world.destroy();
            editor.playtest = {};
        }
        else
        {
            editor.playtesting = true;
            editor.playtest = Playtest::create(editor.dispatcher.world.clone());
        }
    }

    Vector4 color = {1.0, 1.0, 1.0, 1.0};
    if (editor.dispatcher.dirty)
    {
        color = {1.0, 0.8, 0.8, 1.0};
    }

    if (icon_button(user, "Save", "assets/gui/save.png", color))
    {
        WorldFile::save("assets/world.dat", editor.dispatcher);
    }

    end_toolbar(user);
}

void show_editor_ui(GuiEditor &editor, UiUser &user, ResourceSpec &resources,
                    catedu::pbr::Renderer &renderer, Input &input,
                    bool &return_back)
{
    renderer.camera = editor.editor_camera.cam;
    renderer.begin_pass();

    GenResources gen_resources = get_genres(resources);

    if (user.hovered() && !editor.playtesting)
    {
        sapp_lock_mouse(input.k[INPUT_MB_MIDDLE].held);
        if (input.k[INPUT_MB_MIDDLE].held)
        {
            editor.editor_camera.handle_controls(input,
                                                 {sapp_width(), sapp_height()});
            input.clear(INPUT_MB_MIDDLE);
            input.clear(INPUT_MB_LEFT);
            input.clear(INPUT_MB_RIGHT);

            editor.sub_editor.show(user, renderer, editor.dispatcher,
                                   gen_resources, input,
                                   editor.editor_camera.cam);
        }
        else
        {
            editor.sub_editor.show(user, renderer, editor.dispatcher,
                                   gen_resources, input,
                                   editor.editor_camera.cam);
            editor.editor_camera.handle_controls(input,
                                                 {sapp_width(), sapp_height()});
        }
    }

    World *world = &editor.dispatcher.world;

    if (editor.playtesting)
    {
        editor.playtest.update(input);
        world = &editor.playtest.world;
    }

    for (auto &object : iter(world->objects))
    {
        GeneratedObject mesh = {};

        switch (object.type)
        {
        case Object::Type::Road:
            mesh = genmesh_generate_road();
            break;
        case Object::Type::Building:
            mesh = genmesh_generate_building(object.floors);
            break;
        case Object::Type::Player:
            mesh = genmesh_generate_player();
            break;
        }

        genobj_render_object(
            renderer, gen_resources, mesh,
            Matrix4::translate({(float)object.x, 0, (float)object.y}));
    }

    genobj_render_object(renderer, gen_resources, genmesh_generate_ground());

    renderer.end_pass();

    show_editor_controls(user, editor, return_back);
    show_left_panel(user, editor, resources, renderer);

    if (input.k[SAPP_KEYCODE_F3].pressed)
    {
        editor.show_debug = !editor.show_debug;
    }
}

bool show_main_editor(GuiEditor &editor, UiUser &user, ResourceSpec &resources,
                      catedu::pbr::Renderer &renderer, Input &input)
{
    AutoLayoutElement element = create_main_element(user);
    user.state->element_storage.push("Main", {});
    user.begin_generic(element, {}, {}, user.state->element_storage.id());

    bool return_back = false;
    show_editor_ui(editor, user, resources, renderer, input, return_back);
    user.end_generic();
    user.state->element_storage.pop();

    return return_back;
}

bool GuiEditor::show(catedu::pbr::Renderer &renderer, ResourceSpec &resources,
                     UiUser &user)
{
    offscreen_clear();

    bool return_back =
        show_main_editor(*this, user, resources, renderer, user.state->input);

    show_popups(user, *this, return_back);

    return return_back;
}

void GuiEditor::deinit()
{
    offscreen_deinit_targets(this->ui_state->core);

    dispatcher.destroy();
    debug_tree.deinit();
}

void SubEditor::show(UiUser &user, catedu::pbr::Renderer &renderer,
                     Dispatcher &disp, GenResources &gen_resources,
                     Input &input, Camera &camera)
{
    switch (type)
    {
    case Type::Building:
        edit_building.show(user, renderer, disp, gen_resources, input, camera);
        break;
    case Type::Road:
        edit_basic.show(user, renderer, disp, gen_resources, input, camera,
                        Object::Type::Road);
        break;
    case Type::Player:
        edit_basic.show(user, renderer, disp, gen_resources, input, camera,
                        Object::Type::Player);
        break;
    }
}
