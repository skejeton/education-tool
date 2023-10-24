#pragma once
#include "catedu/math.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"
#include "catedu/ui/rendering/pass.hpp"

struct UiState
{
    UiRenderingCore core;
    UiFontRenderer font;

    static UiState init(const char* font_path);
};

struct UiUser
{
    UiState* state;
    UiRenderingPass pass;

    static UiUser init(UiState& state);

    void begin_pass();
    void end_pass();

    bool put_button(const char* text);
};
