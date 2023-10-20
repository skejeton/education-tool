#ifndef HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU
#define HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU

#include "catedu/core/storage/id_retainer.hpp"
#include "catedu/diag/diag_monitor.hpp"
#include "catedu/input.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"
#include <sokol/sokol_app.h>


enum class GuiMenuScreen
{
    SelectWorld,
    World,
};

struct GuiObjectInputState
{
    bool hovered;
    bool pressed;
    bool released;
    bool clicked;
};

struct ButtonState
{
    bool hovered;
    bool pressed; // AKA Down.
    bool clicked; // AKA Just pressed.
    bool released;

    void input_button(bool hovered, ButtonState button);
    void input_mouse(bool pressed);
    void update();
};

struct GuiObject
{
    UiTransform transform_current;
    UiTransform transform_target;
    Vector2 pointer; // Trasformed mouse position.
    UiBuffers shape;

    ButtonState button;
};

struct GuiPassObject
{
    // This points to transform_target in GuiPassObject, and will be
    // interpolated accordingly.
    UiTransform* transform;
};

struct GuiInput
{
    ButtonState mouse_buttons[3];
    Vector2 mouse_pos;

    int text_input_at;
    char text_input[32];

    Input inp;
};

struct GuiCore
{
    IdRetainer<GuiObject> objects;
    UiRenderingPass* pass;
    GuiInput input;
    GuiObject* button_obj;
    std::string button_obj_id;
    GuiObject* button_obj_candidate;
    DiagMonitorPass diag_pass;

    void begin_pass(UiRenderingPass* pass);
    void end_pass();

    GuiPassObject begin(std::string id, Rect base, UiBuffers shape);
    void end();

    // Inputs
    ButtonState request_button();
};

struct GuiMainMenu
{
    UiRenderingCore* core;
    UiFontRenderer font;
    GuiCore gui;
    GuiMenuScreen screen;
    IdRetainer<bool> collapses;

    static GuiMainMenu init();
    void show();
};

/// @return If the event was used.
bool
pass_event(GuiMainMenu& menu, const sapp_event* event);

#endif // HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU
