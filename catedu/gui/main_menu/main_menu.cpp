#include "main_menu.hpp"
#include "catedu/core/math/interpolation.hpp"
#include "catedu/core/math/point_intersect.hpp"
#include "catedu/core/text_editing/text_editing.hpp"
#include "catedu/ui/rendering/colors.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/rendering/pass.hpp"
#include "sokol/sokol_app.h"
#include <array>

bool
pass_event(GuiMainMenu& menu, const sapp_event* event)
{
    GuiInput& input = menu.gui.input;
    bool pressed;

    switch (event->type) {
        case SAPP_EVENTTYPE_CHAR:
            if (input.text_input_at == (sizeof input.text_input) - 1) {
                break;
            }
            input.text_input[input.text_input_at++] = event->char_code;
            input.text_input[input.text_input_at] = '\0';
            return true;
        case SAPP_EVENTTYPE_MOUSE_MOVE:
            input.mouse_pos = { event->mouse_x, event->mouse_y };
            return true;
        case SAPP_EVENTTYPE_MOUSE_DOWN:
        case SAPP_EVENTTYPE_MOUSE_UP:
            pressed = event->type == SAPP_EVENTTYPE_MOUSE_DOWN;
            input.mouse_buttons[event->mouse_button].input_mouse(pressed);
            return true;
        default:
            input.inp.pass_event(event);
            return true;
    }
    return false;
}

struct SimpleObject
{
    Rect rectangle;
    UiBuffers shape;
    Vector4 color_top;
    Vector4 color_bottom;
};

struct ObjectGenerationProperties
{
    Rect base;
    Vector4 mul_color;
    UiBuffers shape;
};

std::array<SimpleObject, 3>
obj_generate_shiny(ObjectGenerationProperties props)
{
    SimpleObject shadow, inner, shine;

    shadow.shape = props.shape;
    shadow.rectangle = rect_shrink(props.base, { -4, -4 });
    shadow.color_top = color_bluish(0.2, 0.1) * props.mul_color;
    shadow.color_bottom = color_bluish(0.1, 0.1) * props.mul_color;

    inner.shape = props.shape;
    inner.rectangle = props.base;
    inner.color_top = color_bluish(0.95) * props.mul_color;
    inner.color_bottom = color_bluish(0.80) * props.mul_color;

    shine.shape = props.shape;
    shine.rectangle = rect_shrink(props.base, { 4, 4 });
    shine.rectangle.pos.y += shine.rectangle.siz.y / 2;
    shine.rectangle.siz.y /= 2;
    if (props.shape == UiBuffers::Ellipse) {
        shine.rectangle.pos.x += shine.rectangle.siz.x / (2 * 3);
        shine.rectangle.siz.x /= 1.5;
    }
    shine.color_top = color_bluish(1.0, 0.3) * props.mul_color;
    shine.color_bottom = color_bluish(0.8, 0.6) * props.mul_color;

    return { shadow, inner, shine };
}

std::array<SimpleObject, 1>
obj_generate_default(ObjectGenerationProperties props)
{
    SimpleObject back;

    back.shape = props.shape;
    back.rectangle = props.base;
    back.color_top = color_bluish(1.0) * props.mul_color;
    back.color_bottom = color_bluish(0.9) * props.mul_color;

    return { back };
}

void
render_simple_objects(UiRenderingPass& pass, SimpleObject* objects, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        SimpleObject& object = objects[i];

        UiTransform transform = {};
        transform.scale = { 1, 1 };
        transform.base = object.rectangle;

        pass.push_transform(transform);
        pass.render_brush(
          UiMakeBrush::make_plain_brush(object.shape)
            .with_gradient(object.color_bottom, object.color_top)
            .build());
        pass.pop_transform();
    }
}

void
render_default(UiRenderingPass& pass,
               Rect base,
               UiBuffers shape,
               Vector4 mul_color = { 1, 1, 1, 1 })
{
    auto objects = obj_generate_default({ base, mul_color, shape });

    render_simple_objects(pass, objects.data(), objects.size());
}

void
render_shiny(UiRenderingPass& pass,
             Rect base,
             UiBuffers shape,
             Vector4 mul_color = { 1, 1, 1, 1 })
{
    auto objects = obj_generate_shiny({ base, mul_color, shape });

    render_simple_objects(pass, objects.data(), objects.size());
}

float
render_key_value(UiRenderingPass& pass,
                 UiFontRenderer& font,
                 Vector2 pos,
                 Vector2 scale,
                 float padding,
                 const char* key,
                 const char* value)
{
    Rect bounds_k = font.bounds_text_utf8(pos, key, scale);
    Rect bounds_v = font.bounds_text_utf8(
      bounds_k.pos + Vector2{ bounds_k.siz.x + padding, 0 }, value, scale);
    bounds_v.siz.x += padding * 2;
    bounds_k.siz.x += bounds_v.siz.x + padding * 5;
    bounds_k.siz.y += padding * 2;
    bounds_v.pos.x += padding * 2;
    bounds_v.pos.y += padding;

    render_default(pass,
                   { pos.x, pos.y, bounds_k.siz.x, bounds_k.siz.y },
                   UiBuffers::Rectangle,
                   { 1, 1, 1, 0.9 });

    font.render_text_utf8(&pass,
                          pos + Vector2{ padding * 2, padding },
                          key,
                          UiMakeBrush::make_plain_brush(UiBuffers::Rectangle)
                            .with_solid({ 0.2, 0.2, 0.2, 1 })
                            .build(),
                          scale);

    pos = bounds_v.pos;

    render_default(pass,
                   { pos.x, pos.y, bounds_v.siz.x, bounds_v.siz.y },
                   UiBuffers::Rectangle,
                   { 0.0, 0.8, 0.0, 0.9 });

    font.render_text_utf8(&pass,
                          pos + Vector2{ padding, 0 },
                          value,
                          UiMakeBrush::make_plain_brush(UiBuffers::Rectangle)
                            .with_solid(UI_COLOR_WHITE)
                            .build(),
                          scale);

    return bounds_k.siz.y + padding;
}

float
render_collapse(GuiCore& gui,
                UiRenderingPass& pass,
                UiFontRenderer& font,
                Vector2 pos,
                Vector2 scale,
                float padding,
                const char* key,
                bool& collapsed)
{
    Rect bounds = font.bounds_text_utf8(pos, key, scale);
    bounds.siz.x += padding * 2;
    bounds.siz.y += padding * 2;
    Rect collapse = { pos, { bounds.siz.y, bounds.siz.y } };
    bounds.pos.x += collapse.siz.x + padding;

    gui.begin(stdstrfmt("collapse %s", key), collapse, UiBuffers::Rectangle);
    ButtonState button = gui.request_button();
    render_shiny(pass,
                 at0(collapse),
                 UiBuffers::Squircle,
                 collapsed ? color_bluish(0.8) : UI_COLOR_GREEN);

    if (button.clicked) {
        collapsed = !collapsed;
    }
    gui.end();
    render_shiny(pass, bounds, UiBuffers::Rectangle, { 1, 1, 1, 0.9 });

    font.render_text_utf8(&pass,
                          bounds.pos + Vector2{ padding, padding },
                          key,
                          UiMakeBrush::make_plain_brush(UiBuffers::Rectangle)
                            .with_solid({ 0.2, 0.2, 0.2, 1 })
                            .build(),
                          scale);

    return bounds.siz.y + padding;
}

void
draw_starfield_background(UiRenderingPass& pass)
{
    render_default(pass,
                   { 0, 0, sapp_widthf(), sapp_heightf() },
                   UiBuffers::Rectangle,
                   { 0.0, 0.1, 0.5, 1.0 });

    srand(50);
    for (int i = 0; i < 100; i++) {
        float dist = rand() % 7 + 1;
        float r = (1.0 / dist) * 3;
        float sec = sapp_frame_count() / 60.0f;
        float x = int(rand() + sec * 500 / dist) % (int)sapp_width();
        float y = rand() % (int)sapp_height();

        Vector4 color = color_bluish(1.0, 0.7 / dist);

        render_default(
          pass, { x - r, y - r, r * 2, r * 2 }, UiBuffers::Ellipse, color);
    }
}

void
world_tile(GuiCore& gui,
           UiRenderingPass& pass,
           UiFontRenderer& font,
           std::string id,
           std::string name,
           Rect base_rect)
{
    gui.begin(id, base_rect, UiBuffers::Squircle);
    bool hovered = gui.request_button().hovered;

    Rect at_0 = at0(base_rect);

    GuiPassObject world_obj = gui.begin("tile", at_0, UiBuffers::Squircle);
    {
        if (hovered) {
            world_obj.transform->scale = { 1.1, 1.1 };
            world_obj.transform->translate = { 0, -20 };
        } else {
            world_obj.transform->scale = { 1, 1 };
            world_obj.transform->translate = { 0, 0 };
        }

        Rect rect = { { 0, 0 }, base_rect.siz };
        Rect preview_rect = rect_shrink(
          { { 0, 0 }, base_rect.siz * Vector2{ 1, 2.3 / 3 } }, { 10, 10 });
        Vector2 name_pos = { preview_rect.pos.x + 20,
                             preview_rect.pos.y + preview_rect.siz.y + 20 };

        render_shiny(pass, at_0, UiBuffers::Squircle, color_bluish(1.0, 0.8));
        render_shiny(pass, preview_rect, UiBuffers::Squircle);
        font.render_text_utf8(
          &pass,
          name_pos,
          name.c_str(),
          UiMakeBrush::make_plain_brush(UiBuffers::Rectangle)
            .with_solid(UI_COLOR_BLACK)
            .build(),
          { 3, 3 });
    }
    gui.end();
    gui.end();
}

void
go_to_screen(GuiMainMenu& menu, GuiMenuScreen screen)
{
    menu.screen = screen;
    // For example, here, we could record the history.
}

void
GuiCore::begin_pass(UiRenderingPass* pass)
{
    this->pass = pass;
    this->button_obj_candidate = nullptr;
    this->objects.begin_cycle();
}

void
GuiCore::end_pass()
{
    this->button_obj = this->button_obj_candidate;
    this->objects.end_cycle();
}

GuiPassObject
GuiCore::begin(std::string id, Rect base, UiBuffers shape)
{
    GuiObject object = {};
    object.transform_target.origin = { 0.5, 0.5 };
    object.transform_target.scale = { 1, 1 };

    this->objects.push(id.c_str(), object);

    GuiObject* obj = this->objects.value();

    obj->shape = shape;
    obj->transform_current.translate = math_rerp(
      obj->transform_current.translate, obj->transform_target.translate, 0.5);
    obj->transform_current.base = base;
    obj->transform_current.origin = obj->transform_target.origin;
    obj->transform_current.rotation = obj->transform_target.rotation;
    obj->transform_current.scale =
      math_rerp(obj->transform_current.scale, obj->transform_target.scale, 0.5);

    this->pass->push_transform(obj->transform_current);
    obj->pointer = this->pass->transform_point(this->input.mouse_pos);

    return { &obj->transform_target };
}

void
GuiCore::end()
{
    this->pass->pop_transform();
    this->objects.pop();
}

ButtonState
GuiCore::request_button()
{
    GuiObject* obj = this->objects.value();

    bool hovered = false;

    Rect base = obj->transform_current.base;

    switch (obj->shape) {
        case UiBuffers::Rectangle:
            hovered = math_point_intersect_rect(obj->pointer, base);
            break;
        case UiBuffers::Ellipse:
            hovered = math_point_intersect_ellipse(
              obj->pointer, base.pos + base.siz / 2, base.siz / 2);
            break;
        case UiBuffers::Squircle:
            hovered = math_point_intersect_squircle(obj->pointer, base, 16);
            break;
    }

    obj->button.update();
    if (obj == this->button_obj) {
        obj->button.input_button(hovered, this->input.mouse_buttons[0]);
    } else {
        obj->button.hovered = false;
    }

    if (hovered) {
        this->button_obj_candidate = obj;
        this->button_obj_id = this->objects.current_path.back();
    }

    return obj->button;
}

GuiMainMenu
GuiMainMenu::init()
{
    GuiMainMenu result = {};
    result.core = new UiRenderingCore(UiRenderingCore::init());
    result.font =
      UiFontRenderer::init(result.core, { "./assets/Roboto-Regular.ttf", 16 });
    return result;
}

void
render_text_field(Rect rect,
                  SingleLineTextEditor& editor,
                  UiFontRenderer& font,
                  UiRenderingPass& pass)
{
    // 1. Calculate cursor screen position.
    Rect bounds = font.bounds_text_utf8(rect.pos + Vector2{ 10, 5 },
                                        editor.buffer.data,
                                        { 2.0, 2.0 },
                                        editor.cursor.column);

    // 2. Render background.
    render_shiny(pass, rect, UiBuffers::Squircle);

    // 3. Render text.
    font.render_text_utf8(
      &pass,
      bounds.pos,
      editor.buffer.data,
      UiMakeBrush::make_plain_brush(UiBuffers::Rectangle)
        .with_gradient({ 0.1, 0.1, 0.1, 1 }, { 0.0, 0.0, 0.1, 1 })
        .build(),
      { 2.0, 2.0 });

    // 4. Render cursor.
    render_default(
      pass,
      { bounds.pos.x + bounds.siz.x, bounds.pos.y, 2, bounds.siz.y },
      UiBuffers::Squircle,
      { 1.0, 0.0, 0.0, 1.0 });
}

void
put_text_input(const char* name,
               GuiMainMenu& menu,
               UiRenderingPass& pass,
               Rect rect)
{
    static char buf[16] = { 0 };
    static SingleLineTextEditor editor(buf, sizeof buf);

    editor.insert(menu.gui.input.text_input);

    if (menu.gui.input.inp.key_states[SAPP_KEYCODE_LEFT].pressed) {
        editor.move_cursor(-1);
    } else if (menu.gui.input.inp.key_states[SAPP_KEYCODE_RIGHT].pressed) {
        editor.move_cursor(1);
    } else if (menu.gui.input.inp.key_states[SAPP_KEYCODE_BACKSPACE].pressed) {
        editor.erase_back();
    }

    menu.gui.begin(name, rect, UiBuffers::Squircle);
    menu.gui.request_button();
    render_text_field(rect, editor, menu.font, pass);
    menu.gui.end();
}

void
main_screen(GuiMainMenu& menu, UiRenderingPass& pass)
{
    Vector4 plus_color = { 0.2, 0.7, 0.2, 1 };

    float pre_padding =
      (sapp_widthf() > 1400 ? sapp_widthf() - 1400 : 0) / 2.0f;
    float padding = sapp_widthf() / 50;
    float tile_w = (sapp_widthf() - padding * 4 - pre_padding * 2) / 3.0f;
    float tile_h = tile_w * (3.0f / 4.0f);

    static float content_max = 0;

    int world_count = 8;

    if (world_count == 0) {
        menu.font.render_text_utf8(
          &pass,
          { pre_padding + padding, 100 },
          "No worlds exist. Create one with (+)",
          UiMakeBrush::make_plain_brush(UiBuffers::Rectangle)
            .with_solid(UI_COLOR_WHITE)
            .build(),
          { 4.0, 4.0 });
    }

    content_max = 0;
    for (int w = 0; w < world_count; w++) {
        int i = w % 3;
        int j = w / 3;

        Rect base_rect = { pre_padding + padding + (tile_w + padding) * i,
                           padding + (tile_h + padding) * j + 50,
                           tile_w,
                           tile_h };
        world_tile(menu.gui,
                   pass,
                   menu.font,
                   stdstrfmt("w%d", w),
                   stdstrfmt("World %d", w),
                   base_rect);

        content_max =
          std::max(content_max, base_rect.pos.y + base_rect.siz.y + padding);
    }

    render_shiny(pass,
                 { 0, sapp_heightf() - 100, sapp_widthf(), 100 },
                 UiBuffers::Rectangle,
                 { 1, 1, 1, 0.9 });

    {
        menu.gui.begin(
          "plus",
          { (sapp_widthf() - 150) / 2, sapp_heightf() - 150 + 25, 150, 150 },
          UiBuffers::Ellipse);

        ButtonState button_state = menu.gui.request_button();

        GuiPassObject world_obj =
          menu.gui.begin("+", { 0, 0, 150, 150 }, UiBuffers::Ellipse);
        if (button_state.hovered) {
            world_obj.transform->scale = { 1.1, 1.1 };
            world_obj.transform->translate = { 0, -20 };
        } else {
            world_obj.transform->scale = { 1, 1 };
            world_obj.transform->translate = { 0, 0 };
        }
        if (button_state.clicked) {
            go_to_screen(menu, GuiMenuScreen::World);
        }

        render_shiny(pass, { 0, 0, 150, 150 }, UiBuffers::Ellipse);
        UiTransform transform = {};
        transform.scale = { 15, 15 };
        transform.base = { 25, 25 };
        pass.push_transform(transform);
        menu.font.render_glyph(
          &pass,
          { 0, 0 },
          '+',
          UiMakeBrush::make_plain_brush(UiBuffers::Ellipse)
            .with_gradient(into_transparent(plus_color, 0.2), plus_color)
            .build());
        pass.pop_transform();
        menu.gui.end();
        menu.gui.end();
    }
}

void
GuiMainMenu::show()
{
    // 1 - Begin pass.
    UiRenderingPass pass = UiRenderingPass::begin(core);
    gui.begin_pass(&pass);

    // NOTE: Terrible scoping here.
    //       The diagnostics pass is created every frame and only used during
    //       the frame, however, it is not considered destroyed until the end of
    //       the frame.
    gui.diag_pass = DiagMonitorPass::begin();

    // 2 - Handle pass.
    draw_starfield_background(pass);
    switch (this->screen) {

        case GuiMenuScreen::SelectWorld:
            main_screen(*this, pass);
            break;
        case GuiMenuScreen::World:
            put_text_input("World name", *this, pass, { 100, 100, 400, 50 });
            // TODO
            break;
    }

    gui.diag_pass.push("candid_next", gui.button_obj_id.c_str());

    // 3 - Render diagnostics.
    float y = 0;
    int depth = 0;

    this->collapses.begin_cycle();
    this->collapses.push("collapses", false);

    for (auto& kv : gui.diag_pass.iterate()) {
        bool* val;
        float x = depth * 20;
        switch (kv.type) {
            case DiagMonitorObject::Type::SectionBegin:
                val = this->collapses.value();
                this->collapses.push(kv.name.c_str(), false);
                depth++;
                if (*val == true) {
                    *this->collapses.value() = true;
                    continue;
                }

                y += render_collapse(gui,
                                     pass,
                                     font,
                                     { 10 + x, 10 + y },
                                     { 2, 2 },
                                     7,
                                     kv.name.c_str(),
                                     *this->collapses.value());
                break;
            case DiagMonitorObject::Type::SectionEnd:
                this->collapses.pop();
                depth--;
                break;
            case DiagMonitorObject::Type::Value:
                if (*this->collapses.value() == true) {
                    continue;
                }
                y += render_key_value(pass,
                                      font,
                                      { 10 + x, 10 + y },
                                      { 2, 2 },
                                      7,
                                      kv.name.c_str(),
                                      kv.value.c_str());
                break;
        }
    }
    this->collapses.pop();
    this->collapses.end_cycle();

    // 4 - End pass.
    gui.diag_pass.end();
    gui.input.mouse_buttons[0].update();
    gui.input.mouse_buttons[1].update();
    gui.input.mouse_buttons[2].update();
    gui.input.text_input[0] = '\0';
    gui.input.text_input_at = 0;
    gui.input.inp.update();

    pass.end();
    gui.end_pass();
}

void
ButtonState::input_button(bool p_hovered, ButtonState button)
{
    hovered = p_hovered && button.hovered;

    if (hovered) {
        *this = button;
    }
}

void
ButtonState::input_mouse(bool p_pressed)
{
    hovered = true;
    pressed = p_pressed;
    clicked = p_pressed;
    released = !p_pressed;
}

void
ButtonState::update()
{
    clicked = false;
    released = false;
}
