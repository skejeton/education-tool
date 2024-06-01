#pragma once
#include "catedu/rendering/3d/pbr.hpp"
#include "generator.hpp"

struct GenResources
{
    catedu::Model box;
    catedu::Model box_shaded;
};

void genobj_render_object(catedu::pbr::Renderer &renderer,
                          const GenResources &resources,
                          const GeneratedObject &object,
                          Matrix4 matrix = Matrix4::identity());
