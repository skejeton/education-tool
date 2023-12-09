#include "world.hpp"

void render_model_at(Vector3 pos, ResourceSpec &res, TableId id,
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

void render_tile_at(Vector3 pos, ResourceSpec &res, TableId id,
                    BoxdrawRenderer &boxdraw, bool if_editor)
{
    // FIXME(T1): Incredibly inefficient.
    SpecTile &tile = res.tiles.get_assert(id);
    render_model_at(pos, res, tile.model_id, boxdraw, if_editor);
}

bool is_tile_transparent(ResourceSpec &res, TableId id)
{
    SpecTile &tile = res.tiles.get_assert(id);
    SpecModel &model = res.models.get_assert(tile.model_id);
    return model.if_transparent;
}

World World::init()
{
    return {};
}

void World::deinit()
{
}

Vector2i World::map_index_to_pos(int i, int j)
{
    return {i - 32, j - 32};
}

void World::render(ResourceSpec &res, BoxdrawRenderer &boxdraw)
{
    // FIXME(T2): Currently can't use models for the grass here since the model
    //            is different size and scale.
    // This is to draw the grass.
    if (this->if_grass)
    {
        boxdraw_push(
            &boxdraw,
            boxdraw_cmdtexture(
                box3_extrude_from_point({0, 0, 0}, {32 - 0.5, 0.5, 32 - 0.5}),
                res.tileset.cropped({0, 32, 32, 32}).tiled({64, 64})));
    }

    if (!is_editor)
    {
        TableId player_model = res.find_model_by_name("player");
        render_model_at({float(camera_pos.x), 1, float(camera_pos.y)}, res,
                        player_model, boxdraw, true);
    }

    for (auto [id, ent] : iter(entities))
    {
        TableId model = res.find_model_by_name(ent.model_name);
        if (model.id == 0)
        {
            model = res.find_model_by_name("invalid");
        }
        render_model_at({float(ent.pos.x), 1, float(ent.pos.y)}, res, model,
                        boxdraw, is_editor);
    }
    // NOTE: Render in reverse order for proper glass rendering. This is a hack
    //       until I get per-pixel transparency working.
    // Good old @Copypaste...
    for (int i = 0; i < 64; i++)
    {
        for (int j = 63; j >= 0; j--)
        {
            if (this->ground[i][j] == 0)
            {
                continue;
            }

            Vector2i pos = this->map_index_to_pos(i, j);

            render_tile_at({float(pos.x), 1, float(pos.y)}, res,
                           {(size_t)this->ground[i][j]}, boxdraw, is_editor);
        }
    }

    for (int i = 0; i < 64; i++)
    {
        for (int j = 63; j >= 0; j--)
        {
            if (this->middle[i][j] == 0 ||
                is_tile_transparent(res, {(size_t)this->middle[i][j]}))
            {
                continue;
            }

            Vector2i pos = this->map_index_to_pos(i, j);

            render_tile_at({float(pos.x), 1, float(pos.y)}, res,
                           {(size_t)this->middle[i][j]}, boxdraw, is_editor);
        }
    }

    for (int i = 0; i < 64; i++)
    {
        for (int j = 63; j >= 0; j--)
        {
            if (this->middle[i][j] == 0 ||
                !is_tile_transparent(res, {(size_t)this->middle[i][j]}))
            {
                continue;
            }

            Vector2i pos = this->map_index_to_pos(i, j);

            render_tile_at({float(pos.x), 1, float(pos.y)}, res,
                           {(size_t)this->middle[i][j]}, boxdraw, is_editor);
        }
    }

    if (is_editor)
    {
        TableId selector_model = res.find_model_by_name("selector");
        render_model_at({float(camera_pos.x), 1, float(camera_pos.y)}, res,
                        selector_model, boxdraw, is_editor);
    }
}
