#include "world.hpp"

Rect
tile_texture_rect(int tile_id)
{
    switch (tile_id) {
        case 0: // nothing
            return {};
            break;
        case 1: // brick
            return { 64, 32, 32, 32 };
            break;
        case 2: // tile
            return { 32, 32, 32, 32 };
            break;
        case 3: // entity
            return { 64, 0, 32, 32 };
            break;
        case 4: // player
            return { 96, 32, 32, 32 };
            break;
    }
}

Box3
tile_model(int tile_id, Vector3 pos)
{
    switch (tile_id) {
        case 0: // nothing
            return {};
            break;
        case 1: // brick
            return box3_extrude_from_point(pos, { 0.5, 0.5, 0.5 });
            break;
        case 2: // tile
            return box3_extrude_from_point(pos + Vector3{ 0, -0.45, 0 },
                                           { 0.5, 0.05, 0.5 });
            break;
        case 3: // entity
            return box3_extrude_from_point(pos, { 0.5, 0.5, 0.5 });
            break;
        case 4: // player
            return box3_extrude_from_point(pos, { 0.5, 0.5, 0.5 });
            break;
    }
}

World
World::init(Texture tileset)
{
    return { tileset };
}

void
World::deinit()
{
}

Vector2i
World::map_index_to_pos(int i, int j)
{
    return { i - 32, j - 32 };
}

void
World::render(BoxdrawRenderer& boxdraw)
{
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            Vector2i pos = this->map_index_to_pos(i, j);
            boxdraw_push(
              &boxdraw,
              boxdraw_cmdtexture(
                box3_extrude_from_point({ float(pos.x), 0, float(pos.y) },
                                        { 0.5, 0.5, 0.5 }),
                this->tileset.cropped({ 0, 32, 32, 32 })));

            if (this->data[i][j] == 0 ||
                (!is_editor && this->data[i][j] == 4)) {
                continue;
            }

            boxdraw_push(
              &boxdraw,
              boxdraw_cmdtexture(
                tile_model(this->data[i][j], { float(pos.x), 1, float(pos.y) }),
                this->tileset.cropped(tile_texture_rect(this->data[i][j]))));
        }
    }

    boxdraw_push(
      &boxdraw,
      boxdraw_cmdtexture(
        box3_extrude_from_point({ float(camera_pos.x), 1, float(camera_pos.y) },
                                { 0.51, 0.51, 0.51 }),
        is_editor ? this->tileset.cropped({ 96, 0, 32, 32 })
                  : this->tileset.cropped({ 96, 32, 32, 32 })));
}
