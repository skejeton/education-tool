#ifndef HPP_CATEDU_UI_RESOURCES_LOAD_IMAGE
#define HPP_CATEDU_UI_RESOURCES_LOAD_IMAGE

#include "catedu/ui/rendering/core.hpp"
#include "catedu/core/memory/buffer.hpp"

UiImageId ui_resources_load_image(UiRenderingCore *core, const char *path);
UiImageId ui_resources_load_image_from_memory(UiRenderingCore *core, Buffer buffer, Vector2i size);

#endif
