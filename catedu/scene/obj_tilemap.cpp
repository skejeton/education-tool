#include "obj_tilemap.hpp"
#include "catedu/rendering/render_model.hpp"

ObjTilemap ObjTilemap::init()
{
    ObjTilemap tilemap = {};
    tilemap.tilemap = BasicTilemap::init();
    return tilemap;
}

void ObjTilemap::deinit()
{
    this->tilemap.deinit();
}

void ObjTilemap::set_tile(Vector2i tile_position, int tile_id)
{
    this->tilemap.set_tile(tile_position, tile_id);
}

int ObjTilemap::get_tile(Vector2i tile_position)
{
    return this->tilemap.get_tile(tile_position);
}

void ObjTilemap::update(PhysicsWorld &world, ResourceSpec &resources)
{
    if (!physics_init)
    {
        BasicTilemapSerial serial = BasicTilemapSerial::init(this->tilemap);
        for (TilePositionToTile tile = serial.next(); tile.id != -1;
             tile = serial.next())
        {
            if (tile.id == 0)
            {
                continue;
            }

            SpecTile tile_spec = resources.tiles.get_assert({(size_t)tile.id});
            if (tile_spec.if_obstacle)
            {
                PhysicsBody body = {0};
                body.area = {(float)tile.position.x - 0.5f,
                             (float)tile.position.y - 0.5f, 1, 1};
                body.solid = true;
                world.bodies.allocate(body);
            }
        }
        physics_init = true;
    }
}

void ObjTilemap::render(BoxdrawRenderer &renderer, ResourceSpec &resources)
{
    BasicTilemapSerial serial = BasicTilemapSerial::init(this->tilemap);
    for (TilePositionToTile tile = serial.next(); tile.id != -1;
         tile = serial.next())
    {
        if (tile.id == 0)
        {
            continue;
        }

        Vector3 pos = {(float)tile.position.x, 0, (float)tile.position.y};

        // FIXME: Assumes that tile ID's in the tilemap are the same as in the
        // resource spec.
        SpecTile tile_spec = resources.tiles.get_assert({(size_t)tile.id});
        SpecModel model_spec = resources.models.get_assert(tile_spec.model_id);
        render_model_at(pos, resources, tile_spec.model_id, renderer, true);
    }
}
