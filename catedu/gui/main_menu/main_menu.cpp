#include "main_menu.hpp"
#include "catedu/core/math/point_intersect.hpp"
#include "catedu/ui/rendering/colors.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/rendering/pass.hpp"


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
    static int frame = 0;
    frame++;
    transform.base = { 0, 0, 160, 160 };
    transform.origin = { 0.5, 0.5 };
    transform.rotation = (frame / 60.0f) * 360.0f / 2.0f;
    transform.scale = { 1, 1 };
    pass.push_transform(transform);

    {
        UiTransform transform = {};
        transform.scale = { 1, 1 };
        transform.base = { 30, 30, 100, 100 };
        pass.push_transform(transform);

        Vector2 relative_mouse_pos = pass.transform_point(mouse_pos);

        Vector4 top_color = { 0.9, 0.9, 0.9, 1.0 };
        Vector4 bottom_color = { 0.7, 0.7, 0.7, 1.0 };

        if (math_point_intersect_squircle(
              relative_mouse_pos, { 30, 30, 100, 100 }, 8)) {
            top_color = { 0.7, 0.7, 0.7, 1.0 };
            bottom_color = { 0.5, 0.5, 0.5, 1.0 };
        }

        pass.render_brush(UiMakeBrush::make_plain_brush(UiBuffers::Squircle)
                            .with_gradient(bottom_color, top_color)
                            .build());
        pass.pop_transform();
    }
    {
        UiTransform transform = {};
        transform.scale = { 1, 1 };
        transform.base = { 32, 32, 96, 96 };
        pass.push_transform(transform);
        pass.render_brush(
          UiMakeBrush::make_plain_brush(UiBuffers::Squircle)
            .with_gradient(into_transparent(UI_COLOR_WHITE), UI_COLOR_WHITE)
            .build());
        pass.pop_transform();
    }
    {
        UiTransform transform = {};
        transform.scale = { 10, 10 };
        transform.base = { 45, 45 };
        pass.push_transform(transform);
        this->font.render_glyph(
          &pass,
          { 0, 0 },
          '+',
          UiMakeBrush::make_plain_brush(UiBuffers::Rectangle)
            .with_gradient(into_transparent(plus_color, 0.2), plus_color)
            .build());
        pass.pop_transform();
    }

    pass.pop_transform();

    pass.end();
}
