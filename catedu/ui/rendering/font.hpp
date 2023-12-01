#ifndef HPP_CATEDU_UI_RENDERING_FONT
#define HPP_CATEDU_UI_RENDERING_FONT

#include "catedu/file_buffer.hpp"
#include "core.hpp"
#include "pass.hpp"
#include "stb/stb_truetype.h"

struct UiFontDef
{
    const char *path;
    float font_size;
};

struct UiFontChunk
{
    UiImageId image;
    size_t image_size;
    stbtt_packedchar packed_chars[256];
};

struct UiFontRenderer
{
    // Maps font page to image
    UiRenderingCore *core;
    stbtt_fontinfo font_info;
    FileBuffer buf;
    UiFontDef def;
    UiFontChunk *chunks[256];
    float ascent;
    float descent;
    float scale;

    static UiFontRenderer init(UiRenderingCore *core, UiFontDef def);
    void render_glyph(UiRenderingPass *pass, Vector2 position, int glyph,
                      UiBrush brush, Vector2 scale = {1.0, 1.0});
    void render_text_utf8(UiRenderingPass *pass, Vector2 position,
                          const char *text, UiBrush brush,
                          Vector2 scale = {1.0, 1.0});
    Rect bounds_text_utf8(Vector2 position, const char *text, Vector2 scale,
                          size_t until = SIZE_MAX);
    void deinit();
};

#endif
