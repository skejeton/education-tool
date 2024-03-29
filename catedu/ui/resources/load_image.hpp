#pragma once
#include "catedu/core/memory/buffer.hpp"
#include "catedu/ui/rendering/core.hpp"

UiImageId ui_resources_load_image(UiRenderingCore *core, const char *path);
UiImageId ui_resources_load_image_from_memory(UiRenderingCore *core,
                                              Buffer buffer, Vector2i size);

