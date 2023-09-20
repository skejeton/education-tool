#ifndef HPP_CATEDU_UI_RENDERING_FONT
#define HPP_CATEDU_UI_RENDERING_FONT

#include "core.hpp"
#include "pass.hpp"
#include "stb/stb_truetype.h"
#include <unordered_map>
#include <vector>

struct UiFontDef
{
    const char* path;
    float font_size;
};

struct UiFontRenderer
{
    // Maps font page to image
    UiRenderingCore* core;
    stbtt_fontinfo font_info;
    UiImageId atlas_image;
    UiFontDef def;
    stbtt_packedchar packed_chars[256];
    size_t page_size;
    float ascent;
    float descent;
    float scale;

    static UiFontRenderer init(UiRenderingCore* core, UiFontDef def);
    void render_glyph(UiRenderingPass* pass,
                      Vector2 position,
                      char glyph,
                      UiBrush brush,
                      Vector2 scale = { 1.0, 1.0 });
    void render_text_utf8(UiRenderingPass* pass,
                          Vector2 position,
                          const char* text,
                          UiBrush brush,
                          Vector2 scale = { 1.0, 1.0 });
    void deinit();
};

#endif
