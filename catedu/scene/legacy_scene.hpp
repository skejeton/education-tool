#pragma once
#include "catedu/core/memory/buffer.hpp"
#include "catedu/rendering/3d/boxdraw.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "scene.hpp"

struct LegacyScene
{
    static Scene load_data_to_scene(Buffer file);
};
