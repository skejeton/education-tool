#pragma once

#include "catedu/genobj/render.hpp"
#include "world.hpp"
#include "catedu/rendering/3d/pbr.hpp"
#include "catedu/rendering/resource_spec.hpp"

GenResources get_genres(ResourceSpec &resources);
void render_place(Place &place, catedu::pbr::Renderer &renderer,
                  ResourceSpec &resources);
