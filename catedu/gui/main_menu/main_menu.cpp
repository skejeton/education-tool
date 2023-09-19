#include "main_menu.hpp"
#include "catedu/ui/rendering/colors.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/rendering/pass.hpp"

GuiMainMenu
GuiMainMenu::init()
{
    GuiMainMenu result = {};
    result.core = UiRenderingCore::init();
    result.font =
      UiFontRenderer::init(&result.core, { "./assets/Roboto-Regular.ttf", 16 });
}

void
GuiMainMenu::show()
{
    UiRenderingPass pass = UiRenderingPass::begin(&this->core);

    this->font.render_text_utf8(
      &pass,
      { 40, 40 },
      "Hello world",
      UiMakeBrush::make_plain_brush(UiBuffers::Rectangle)
        .with_solid(UI_COLOR_YELLOW)
        .build());

    pass.end();
}
