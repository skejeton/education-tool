#include "world.hpp"

Rect tile_texture_rect(int tile_id)
{
    switch (tile_id)
    {
    case 0: // nothing
        return {};
        break;
    case 1: // brick
        return {64, 32, 32, 32};
        break;
    case 2: // tile
        return {32, 32, 32, 32};
        break;
    case 3: // entity
        return {64, 0, 32, 32};
        break;
    case 4: // player
        return {96, 32, 32, 32};
        break;
    case 5: // counter
        return {64, 64, 32, 32};
        break;
    case 6: // glass
        return {64 + 32, 64, 32, 32};
        break;
    case 7: // door
        return {64 - 32, 64, 32, 32};
        break;
    }
}

Box3 tile_model(int tile_id, Vector3 pos)
{
    switch (tile_id)
    {
    case 0: // nothing
        return {};
        break;
    case 1: // brick
    case 3: // entity
    case 4: // player
    case 6: // glass
        return box3_extrude_from_point(pos, {0.5, 0.5, 0.5});
        break;
    case 5: // counter
        return box3_extrude_from_point(pos + Vector3{0, -0.10, 0},
                                       {0.5, 0.35, 0.5});
        break;
    case 7: // door
        return box3_extrude_from_point(pos, {0.5, 0.5, 0.1});
    case 2: // tile
        return box3_extrude_from_point(pos + Vector3{0, -0.45, 0},
                                       {0.5, 0.05, 0.5});
        break;
    }
}

World World::init(Texture tileset)
{
    return {tileset};
}

void World::deinit()
{
}

Vector2i World::map_index_to_pos(int i, int j)
{
    return {i - 32, j - 32};
}

void World::render(BoxdrawRenderer &boxdraw)
{
    boxdraw_push(
        &boxdraw,
        boxdraw_cmdtexture(
            box3_extrude_from_point({0, 0, 0}, {32 - 0.5, 0.5, 32 - 0.5}),
            this->tileset.cropped({0, 32, 32, 32}).tiled({64, 64})));
    for (int i = 63; i >= 0; i--)
    {
        for (int j = 63; j >= 0; j--)
        {
            if (this->data[i][j] == 0 || (!is_editor && this->data[i][j] == 4))
            {
                continue;
            }

            Vector2i pos = this->map_index_to_pos(i, j);

            if (this->data[i][j] == 1 || this->data[i][j] == 6 ||
                this->data[i][j] == 7)
            {
                boxdraw_push(&boxdraw,
                             boxdraw_cmdtexture(
                                 tile_model(this->data[i][j],
                                            {float(pos.x), 2, float(pos.y)}),
                                 this->tileset.cropped(
                                     tile_texture_rect(this->data[i][j]))));
            }

            boxdraw_push(
                &boxdraw,
                boxdraw_cmdtexture(tile_model(this->data[i][j],
                                              {float(pos.x), 1, float(pos.y)}),
                                   this->tileset.cropped(
                                       tile_texture_rect(this->data[i][j]))));
        }
    }

    boxdraw_push(&boxdraw,
                 boxdraw_cmdtexture(
                     box3_extrude_from_point(
                         {float(camera_pos.x), 1, float(camera_pos.y)},
                         {0.51, 0.51, 0.51}),
                     is_editor ? this->tileset.cropped({96, 0, 32, 32})
                               : this->tileset.cropped({96, 32, 32, 32})));
}
