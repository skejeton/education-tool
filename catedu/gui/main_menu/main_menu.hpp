#ifndef HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU
#define HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU

#include "catedu/core/storage/id_retainer.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"

struct GuiObject
{
    UiTransform transform_current;
    UiTransform transform_target;
    Vector2 pointer; // Trasformed mouse position.
    UiBuffers shape;
};

struct GuiPassObject
{
    // This points to transform_target in GuiPassObject, and will be
    // interpolated accordingly.
    UiTransform* transform;

    bool hovered;
};

struct GuiCore
{
    IdRetainer<GuiObject> objects;
    UiRenderingPass* pass;
    Vector2 mouse_pos;

    void begin_pass(UiRenderingPass* pass, Vector2 mouse_pos);
    void end_pass();

    GuiPassObject begin(std::string id, Rect base, UiBuffers shape);
    void end();
};

struct GuiMainMenu
{
    UiRenderingCore* core;
    UiFontRenderer font;
    GuiCore gui;

    static GuiMainMenu init();
    void show(Vector2 mouse_pos);
};

#endif // HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU
