//
// Created by skeje on 7/14/2023.
//

#ifndef HPP_CATEDU_IMGUI_TRICKS
#define HPP_CATEDU_IMGUI_TRICKS

#include "imgui/imgui.h"
#include "math.hpp"
#include <cstddef>
#include <vector>

struct ImGuiTricks
{
    // For parent window, `Begin` is called,
    // for child window, `BeginChild` will be called.
    size_t window_nesting;

    // Because of an implementation detail PushID/PopID don't work
    // when called outside of window context.
    std::vector<int> id_stack;

    /// @brief To be called before placing any widget, and used as a window
    /// container.
    /// @description This function is designed to be used as a regular ImGui
    /// window as a container.
    /// @param rect the widget rectangle.
    /// @param title the window string.
    /// @returns Window content rectangle
    Rect begin_window(Rect rect, const char* title);

    /// @brief Needed for discriminating elements in a list.
    ///        If you have a list of generated windows with same ID, use this
    ///        function.
    /// @param value The identification value.
    void push_id(int value);

    /// @brief Removes ID from ID stack. (refer to: `push_id`)
    void pop_id();

    /// @brief To be called before placing any widget.
    /// @description Creates a place where you can safely call ImGUI widget
    /// calls.
    /// @param rect the widget rectangle.
    /// @param id the widget string id.
    void begin_widget(Rect rect, const char* id);

    /// @brief To be called after placing any widget.
    void end_widget();

    /// @brief Checks if the UI is without errors at the end of the cycle,
    ///        throws assertion otherwise.
    void check_end();
};

#endif // HPP_CATEDU_IMGUI_TRICKS
