#pragma once
#include "catedu/rendering/3d/pbr.hpp"
#include "generator.hpp"

struct ResourceSpec;

struct GenResources
{
    Model box;
    Model box_shaded;
    Model box_window;
    // Magenta checker cube for props with an unknown ID.
    Model box_invalid;
    // Red outline cube marking a tile that can't be placed on.
    Model selector;
    Model grass_floor;
    ResourceSpec *spec;
};

// One model instance that a generated object or prop is drawn with.
struct GenDraw
{
    const Model *model;
    // The model's name in the resource spec.
    const char *model_name;
    Matrix4 matrix;
    Vector4 color;
};

// Receives the model instances that make up an object, so the same
// description of the world can be both rendered and exported.
struct GenSink
{
    void (*draw)(void *user, const GenDraw &draw);
    void *user;
};

// A sink that draws every instance it receives with the renderer.
struct GenRenderSink
{
    Renderer *renderer;
    float time_of_day;

    GenSink sink();
};

void genobj_emit_object(GenSink sink, const GenResources &resources,
                        const GeneratedObject &object,
                        Matrix4 matrix = Matrix4::identity());

void genobj_emit_prop(GenSink sink, const GenResources &resources,
                      const char *id, Matrix4 matrix = Matrix4::identity());

void genobj_render_object(Renderer &renderer, const GenResources &resources,
                          const GeneratedObject &object,
                          Matrix4 matrix = Matrix4::identity(), float time_of_day = 12.0f);

void genobj_render_model(Renderer &renderer, const Model &model,
                         Matrix4 matrix, float time_of_day = 12.0f);

void genobj_render_prop(Renderer &renderer, const GenResources &resources,
                        const char *id, Matrix4 matrix = Matrix4::identity(),
                        float time_of_day = 12.0f);
