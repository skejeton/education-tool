#pragma once

#include "catedu/genobj/render.hpp"
#include "catedu/rendering/3d/pbr.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "world.hpp"

GenResources get_genres(ResourceSpec &resources);

// Where an object sits in its place.
Matrix4 place_object_matrix(const Object &object);
// Emits the model instances an object is drawn with, relative to `matrix`.
void emit_place_object(GenSink sink, const GenResources &resources,
                       const Object &object, Matrix4 matrix);
// Emits the floor every place stands on.
void emit_place_ground(GenSink sink, const GenResources &resources,
                       const Place &place);
void render_place(Place &place, Renderer &renderer, ResourceSpec &resources, float time_of_day);
