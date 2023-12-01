#include "load_image.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "lib/stb/stb_image.h"

UiImageId ui_resources_load_image(UiRenderingCore *core, const char *path)
{
    // Load image with STB
    int width, height, channels;
    stbi_uc *data = stbi_load(path, &width, &height, &channels, 4);
    assert(data);

    // Create image
    return ui_resources_load_image_from_memory(
        core, {(size_t)(width * height * 4), data}, {width, height});
}

UiImageId ui_resources_load_image_from_memory(UiRenderingCore *core,
                                              Buffer buffer, Vector2i size)
{
    return core->alloc_image(ui_image_make_from_data(buffer, size));
}
