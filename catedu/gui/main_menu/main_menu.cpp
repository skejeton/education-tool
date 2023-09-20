#include "main_menu.hpp"
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
end_ro()
{
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

    Vector4 plus_color = { 0.2, 0.7, 0.2, 1 };

    UiTransform transform = {};
    transform.base = { 0, 0, 160, 160 };
    transform.origin = { 0.5, 0.5 };
    transform.scale = { 1, 1 };
    pass.push_transform(transform);

    render_shiny(this,
                 pass,
                 { 0, 0, sapp_widthf(), sapp_heightf() },
                 UiBuffers::Rectangle,
                 { 0.4, 0.1, 0.5, 1.0 });

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

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 4; j++) {
            render_shiny(this,
                         pass,
                         { pre_padding + padding + (tile_w + padding) * i,
                           padding + (tile_h + padding) * j,
                           tile_w,
                           tile_h },
                         UiBuffers::Squircle,
                         color_bluish(1.0, 0.8));
            render_shiny(
              this,
              pass,
              rect_shrink({ pre_padding + padding + (tile_w + padding) * i,
                            padding + (tile_h + padding) * j,
                            tile_w,
                            tile_h * (2.2f / 3.0f) },
                          { 10, 10 }),
              UiBuffers::Squircle);
            this->font.render_text_utf8(
              &pass,
              { pre_padding + padding + (tile_w + padding) * i + 20,
                padding + (tile_h + padding) * j + tile_h * (2.2f / 3.0f) +
                  10 },
              stdstrfmt("World %d", i + j * 3).c_str(),
              UiMakeBrush::make_plain_brush(UiBuffers::Rectangle)
                .with_solid(UI_COLOR_BLACK)
                .build(),
              { 4.0, 4.0 });
        }

    render_shiny(this,
                 pass,
                 { 0, sapp_heightf() - 100, sapp_widthf(), 100 },
                 UiBuffers::Rectangle);
    render_shiny(this,
                 pass,
                 { (sapp_widthf() - 150) / 2, sapp_heightf() - 150, 150, 150 },
                 UiBuffers::Ellipse);
    {
        UiTransform transform = {};
        transform.scale = { 15, 15 };
        transform.base = { (sapp_widthf() - 150) / 2 + 25,
                           sapp_heightf() - 150 + 25 };
        pass.push_transform(transform);
        this->font.render_glyph(
          &pass,
          { 0, 0 },
          '+',
          UiMakeBrush::make_plain_brush(UiBuffers::Ellipse)
            .with_gradient(into_transparent(plus_color, 0.2), plus_color)
            .build());
        pass.pop_transform();
    }

    pass.pop_transform();

    pass.end();
}
