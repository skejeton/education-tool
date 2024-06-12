#pragma once
#include "catedu/core/math/math.hpp"
#include "catedu/core/storage/table.hpp"
#include "catedu/rendering/3d/pbr.hpp"
#include "resource_spec.hpp"

static inline void render_model_at(Vector3 pos, ResourceSpec &res, TableId id,
                                   Renderer &renderer, bool if_editor,
                                   bool if_shadow = false, int rotation = 0)

{
    SpecModel &model = res.models.get_assert(id);

    Params vs_params;
    vs_params.model =
        Matrix4::translate(pos) * Matrix4::rotate_y(rotation * (MATH_TAU / 4));
    vs_params.lightness = 0.0f;
    vs_params.color_mul = {1.0f, 1.0f, 1.0f, if_shadow ? 0.1f : 1.0f};
    renderer.render_model(model.model, vs_params);
}
