#pragma once
#include "catedu/core/math/math.hpp"
#include "catedu/core/storage/table.hpp"
#include "catedu/rendering/3d/boxdraw.hpp"
#include "resource_spec.hpp"

static inline void render_model_at(Vector3 pos, ResourceSpec &res, TableId id,
                                   BoxdrawRenderer &boxdraw, bool if_editor,
                                   bool if_shadow = false)

{
    SpecModel &model = res.models.get_assert(id);

    Box3 box = box3_translate(model.model, pos);
    Texture tex = res.tileset.cropped(model.texture_rect);

    if (model.if_editor_only && !if_editor)
    {
        return;
    }

    BoxdrawCommand cmd =
        boxdraw_cmdtexture(box, tex, model.if_transparent || if_shadow);
    cmd.top_color.w = if_shadow ? 0.5f : 1.0f;

    boxdraw_push(&boxdraw, cmd);

    if (model.if_tall)
    {
        cmd.box = box3_translate(box, {0, 1, 0});
        boxdraw_push(&boxdraw, cmd);
    }
}
