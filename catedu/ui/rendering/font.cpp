#include "font.hpp"
#include "catedu/file_buffer.hpp"
#include "catedu/util.hpp"
#include "make_brush.hpp"
#include "pass.hpp"
#include "transform.hpp"

static Vector2
get_glyph_size(UiFontRenderer* f, char glyph)
{
    auto& packed_char = f->packed_chars[glyph];

    return { float(packed_char.x1 - packed_char.x0) / 10,
             float(packed_char.y1 - packed_char.y0) / 10 };
}

UiFontRenderer
UiFontRenderer::init(UiRenderingCore* core, UiFontDef def)
{
    const int page_size = 1024;

    UiFontRenderer result = {};
    result.core = core;
    result.def = def;
    result.page_size = page_size;

    FILE* f = fopen(def.path, "rb");
    assert(f && "I like shorts! They're comfy and easy to wear!");
    FileBuffer buf = FileBuffer::read_whole_file(f);

    stbtt_InitFont(&result.font_info, buf.data, 0);
    result.scale =
      stbtt_ScaleForPixelHeight(&result.font_info, def.font_size * 5);
    int ascent, descent;
    stbtt_GetFontVMetrics(&result.font_info, &ascent, &descent, nullptr);
    result.ascent = ascent;
    result.descent = descent;

    stbtt_pack_context pack_context = {};

    std::vector<unsigned char> pixels(page_size * page_size);
    std::vector<unsigned char> pixels_rgba(page_size * page_size * 4);
    stbtt_PackBegin(&pack_context,
                    pixels.data(),
                    page_size,
                    page_size,
                    page_size,
                    2,
                    nullptr);

    stbtt_PackSetOversampling(&pack_context, 2, 2);
    stbtt_PackFontRange(&pack_context,
                        buf.data,
                        0,
                        def.font_size * 5,
                        0,
                        256,
                        result.packed_chars);
    stbtt_PackEnd(&pack_context);

    for (int i = 0; i < pixels.size(); i += 1) {
        int alpha = pixels[i];
        pixels_rgba[i * 4 + 0] = 0xFF;
        pixels_rgba[i * 4 + 1] = 0xFF;
        pixels_rgba[i * 4 + 2] = 0xFF;
        pixels_rgba[i * 4 + 3] = alpha;
    }

    result.atlas_image = core->alloc_image(ui_image_make_from_data(
      { pixels_rgba.size(), pixels_rgba.data() }, { page_size, page_size }));

    buf.deinit();

    return result;
}

void
UiFontRenderer::render_glyph(UiRenderingPass* pass,
                             Vector2 position,
                             char glyph,
                             UiBrush brush)
{
    auto& packed_char = this->packed_chars[glyph];
    Rect image_region = { { packed_char.x1, packed_char.y0 },
                          { packed_char.x0 - packed_char.x1,
                            packed_char.y1 - packed_char.y0 } };

    UiTransform transform = {};
    transform.base = { position, get_glyph_size(this, glyph) };
    transform.rotation = 0;
    transform.origin = { 0, 0 };
    transform.scale = { 1.0, 1.0 };

    auto new_brush = UiMakeBrush::make_image_brush(
                       UiBuffers::Rectangle, this->core, this->atlas_image)
                       .with_image_region(image_region)
                       .with_gradient(brush.color_bottom, brush.color_top)
                       .build();

    pass->push_transform(transform);
    this->core->render_object(new_brush);
    pass->pop_transform();
}

void
UiFontRenderer::render_text_utf8(UiRenderingPass* pass,
                                 Vector2 position,
                                 const char* text,
                                 UiBrush brush)
{
    for (size_t i = 0; text[i]; i += 1) {
        if (text[i] == '\n') {
            position.x = 0;
            position.y += ((this->ascent - this->descent) * this->scale);
            continue;
        }

        stbtt_aligned_quad quad;
        stbtt_GetPackedQuad(this->packed_chars,
                            this->page_size,
                            this->page_size,
                            text[i],
                            &position.x,
                            &position.y,
                            &quad,
                            false);
        this->render_glyph(
          pass,
          { quad.x0 / 5, (quad.y0 + this->ascent * this->scale) / 5 },
          text[i],
          brush);
    }
}

void
UiFontRenderer::deinit()
{
    this->core->dealloc_image(this->atlas_image);
}
