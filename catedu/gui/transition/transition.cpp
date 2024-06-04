#include "transition.hpp"
#include "catedu/ui/layout/autolayout.hpp"
#include "catedu/ui/rendering/make_brush.hpp"

GuiTransition GuiTransition::create()
{
    return {};
}

void GuiTransition::destroy()
{
}

void GuiTransition::begin()
{
    alpha_target = 1.0f;
}

bool GuiTransition::going()
{
    return alpha_target > 0.5f && alpha > 0.001f;
}

bool GuiTransition::switching()
{
    return alpha >= 0.999 && alpha_target >= 0.5f;
}

void GuiTransition::show(UiUser &user, Input &input)
{
    if (alpha_target > 0.5f)
    {
        input.reset();
    }

    if (alpha >= 0.999 && alpha_target >= 0.5f)
    {
        alpha_target = 0.0f;
    }

    float speed = 8;
    if (alpha_target < 0.5f)
        speed = 2;

    alpha = slerp(alpha, alpha_target, speed, sapp_frame_duration());

    AutoLayoutElement element = {};
    element.width = {AutoLayoutDimension::Pixel, sapp_widthf()};
    element.height = {AutoLayoutDimension::Pixel, sapp_heightf()};
    element.position = AutoLayoutPosition::Absolute;
    user.begin_generic(
        element, UiMakeBrush::make_solid(Color(Vector4{0, 0, 0, alpha})), {});
    user.end_generic();
}
