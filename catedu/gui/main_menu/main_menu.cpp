#include "main_menu.hpp"
#include "catedu/core/math/interpolation.hpp"
#include "catedu/core/math/point_intersect.hpp"
#include "catedu/ui/rendering/colors.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/rendering/pass.hpp"
#include "sokol/sokol_app.h"

void
render_shiny(GuiMainMenu* gui,
             UiRenderingPass& pass,
             Rect base,
             UiBuffers shape,
             Vector4 mul_color = { 1, 1, 1, 1 })
{
    {
        UiTransform transform = {};
        transform.scale = { 1, 1 };
        transform.base = rect_shrink(base, { -4, -4 });
        transform.base.pos.y += 2;
        pass.push_transform(transform);

        Vector4 top_color = color_bluish(0.2, 0.1) * mul_color;
        Vector4 bottom_color = color_bluish(0.1, 0.1) * mul_color;

        pass.render_brush(UiMakeBrush::make_plain_brush(shape)
                            .with_gradient(bottom_color, top_color)
                            .build());
        pass.pop_transform();
    }

    {
        UiTransform transform = {};
        transform.scale = { 1, 1 };
        transform.base = base;
        pass.push_transform(transform);

        Vector4 top_color = color_bluish(0.95) * mul_color;
        Vector4 bottom_color = color_bluish(0.80) * mul_color;

        pass.render_brush(UiMakeBrush::make_plain_brush(shape)
                            .with_gradient(bottom_color, top_color)
                            .build());
        pass.pop_transform();
    }
    {
        UiTransform transform = {};
        transform.scale = { 1, 1 };
        transform.base = rect_shrink(base, { 4, 4 });
        pass.push_transform(transform);
        pass.render_brush(
          UiMakeBrush::make_plain_brush(shape)
            .with_gradient(into_transparent(color_bluish()) * mul_color,
                           color_bluish() * mul_color)
            .build());
        pass.pop_transform();
    }
}

void
GuiCore::begin_pass(UiRenderingPass* pass, Vector2 mouse_pos)
{
    this->pass = pass;
    this->mouse_pos = mouse_pos;
    this->objects.begin_cycle();
}

void
GuiCore::end_pass()
{
    this->objects.end_cycle();
}

GuiPassObject
GuiCore::begin(std::string id, Rect base, UiBuffers shape)
{
    GuiObject object = {};
    object.transform_target.origin = { 0.5, 0.5 };
    object.transform_target.scale = { 1, 1 };

    this->objects.push(id.c_str(), object);

    GuiObject* ptr = this->objects.value();
    ptr->shape = shape;
    ptr->transform_current.translate = math_rerp(
      ptr->transform_current.translate, ptr->transform_target.translate, 0.5);
    ptr->transform_current.base = base;
    ptr->transform_current.origin = ptr->transform_target.origin;
    ptr->transform_current.rotation = ptr->transform_target.rotation;
    ptr->transform_current.scale =
      math_rerp(ptr->transform_current.scale, ptr->transform_target.scale, 0.5);

    this->pass->push_transform(ptr->transform_current);
    ptr->pointer = this->pass->transform_point(this->mouse_pos);

    bool hovered = false;

    switch (shape) {
        case UiBuffers::Rectangle:
            hovered = math_point_intersect_rect(ptr->pointer, base);
            break;
        case UiBuffers::Ellipse:
            hovered = math_point_intersect_ellipse(
              ptr->pointer, base.pos + base.siz / 2, base.siz / 2);
            break;
        case UiBuffers::Squircle:
            hovered = math_point_intersect_squircle(ptr->pointer, base, 16);
            break;
    }

    return { &ptr->transform_target, hovered };
}

void
GuiCore::end()
{
    this->pass->pop_transform();
    this->objects.pop();
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
GuiMainMenu::show(Vector2 mouse_pos)
{
    UiRenderingPass pass = UiRenderingPass::begin(this->core);
    gui.begin_pass(&pass, mouse_pos);

    Vector4 plus_color = { 0.2, 0.7, 0.2, 1 };

    render_shiny(this,
                 pass,
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

        render_shiny(this,
                     pass,
                     { x - r, y - r, r * 2, r * 2 },
                     UiBuffers::Ellipse,
                     color);
    }

    float pre_padding =
      (sapp_widthf() > 1400 ? sapp_widthf() - 1400 : 0) / 2.0f;
    float padding = sapp_widthf() / 50;
    float tile_w = (sapp_widthf() - padding * 4 - pre_padding * 2) / 3.0f;
    float tile_h = tile_w * (3.0f / 4.0f);

    int world_count = 8;

    if (world_count == 0) {
        this->font.render_text_utf8(
          &pass,
          { pre_padding + padding, 100 },
          "No worlds exist. Create one with (+)",
          UiMakeBrush::make_plain_brush(UiBuffers::Rectangle)
            .with_solid(UI_COLOR_WHITE)
            .build(),
          { 4.0, 4.0 });
    }

    static UiTransform transforms[8];

    for (int w = 0; w < world_count; w++) {
        int i = w % 3;
        int j = w / 3;

        Rect base_rect = { pre_padding + padding + (tile_w + padding) * i,
                           padding + (tile_h + padding) * j + 50,
                           tile_w,
                           tile_h };

        GuiPassObject world_obj =
          gui.begin(stdstrfmt("world %d", w), base_rect, UiBuffers::Squircle);
        {
            if (world_obj.hovered) {
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

            render_shiny(this,
                         pass,
                         { 0, 0, tile_w, tile_h },
                         UiBuffers::Squircle,
                         color_bluish(1.0, 0.8));
            render_shiny(this, pass, preview_rect, UiBuffers::Squircle);
            this->font.render_text_utf8(
              &pass,
              name_pos,
              stdstrfmt("World %d", i + j * 3).c_str(),
              UiMakeBrush::make_plain_brush(UiBuffers::Rectangle)
                .with_solid(UI_COLOR_BLACK)
                .build(),
              { 3, 3 });
        }
        gui.end();
    }

    render_shiny(this,
                 pass,
                 { 0, sapp_heightf() - 100, sapp_widthf(), 100 },
                 UiBuffers::Rectangle,
                 { 1, 1, 1, 0.9 });

    {
        GuiPassObject world_obj = gui.begin("plus",
                                            { (sapp_widthf() - 150) / 2 + 25,
                                              sapp_heightf() - 150 + 25,
                                              150,
                                              150 },
                                            UiBuffers::Ellipse);
        if (world_obj.hovered) {
            world_obj.transform->scale = { 1.1, 1.1 };
            world_obj.transform->translate = { 0, -20 };
        } else {
            world_obj.transform->scale = { 1, 1 };
            world_obj.transform->translate = { 0, 0 };
        }
        render_shiny(this, pass, { 0, 0, 150, 150 }, UiBuffers::Ellipse);
        UiTransform transform = {};
        transform.scale = { 15, 15 };
        transform.base = { 25, 25 };
        pass.push_transform(transform);
        this->font.render_glyph(
          &pass,
          { 0, 0 },
          '+',
          UiMakeBrush::make_plain_brush(UiBuffers::Ellipse)
            .with_gradient(into_transparent(plus_color, 0.2), plus_color)
            .build());
        pass.pop_transform();
        gui.end();
    }

    gui.end_pass();
    pass.end();
}
