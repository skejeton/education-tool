#pragma once
#include "catedu/math.hpp"
#include "resource_spec.hpp"
#include "catedu/table.hpp"
#include "catedu/rendering/3d/boxdraw.hpp"

static inline void render_model_at(Vector3 pos, ResourceSpec &res, TableId id,
                     BoxdrawRenderer &boxdraw, bool if_editor)

{
    SpecModel &model = res.models.get_assert(id);

    Box3 box = box3_translate(model.model, pos);
    Texture tex = res.tileset.cropped(model.texture_rect);

    if (model.if_editor_only && !if_editor)
    {
        return;
    }

    boxdraw_push(&boxdraw, boxdraw_cmdtexture(box, tex));
    if (model.if_tall)
    {
        box = box3_translate(box, {0, 1, 0});
        boxdraw_push(&boxdraw, boxdraw_cmdtexture(box, tex));
    }
}
