///
/// @file pbr.hpp
/// @brief Declarations for the PBR (Physically Based Rendering) module.
///

#pragma once

// The include order matters here.
// clang-format off
#include "sokol/sokol_gfx.h"
#include "catedu/core/math/math.hpp"
#include "catedu/shaders.hxx"
// clang-format on

#include "catedu/rendering/3d/camera.hpp"
#include "model.hpp"

namespace catedu::pbr
{

struct Renderer
{
    sg_pipeline pipeline;
    sg_pipeline offscreen_pipeline;
    sg_pass_action pass_action;
    Camera camera;

    static Renderer init();
    void deinit();

    void begin_pass();
    void begin_pass_offscreen(sg_pass_action pa, sg_attachments att);
    void render_model(Model &model, pbr_vs_params_t vs_params);
    void end_pass();
};

} // namespace catedu::pbr
