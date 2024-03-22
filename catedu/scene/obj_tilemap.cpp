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
        physics_init = true;

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
    }
}

void ObjTilemap::render(catedu::pbr::Renderer &renderer,
                        ResourceSpec &resources)
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
        render_model_at(pos, resources, tile_spec.model_id, renderer, true,
                        false, tile_spec.rotation);
    }
}

ObjTilemap ObjTilemap::copy()
{
    ObjTilemap result = {};
    result.tilemap = BasicTilemap::init();

    BasicTilemapSerial serial = BasicTilemapSerial::init(this->tilemap);
    for (TilePositionToTile tile = serial.next(); tile.id != -1;
         tile = serial.next())
    {
        result.tilemap.set_tile(tile.position, tile.id);
    }

    return result;
}

ObjTilemap ObjTilemap::load(void **data_)
{
    ObjTilemap result = {};

    uint8_t *data = (uint8_t *)*data_;

    uint32_t count = *(uint32_t *)data;
    data += sizeof(uint32_t);

    for (uint32_t i = 0; i < count; i++)
    {
        Vector2i pos = *(Vector2i *)data;
        data += sizeof(Vector2i);
        int32_t id = *(int32_t *)data;
        data += sizeof(int32_t);

        result.tilemap.set_tile(pos, id);
    }

    *data_ = data;

    return result;
}

void ObjTilemap::save(BumpAllocator &alloc)
{
    BasicTilemapSerial serial = BasicTilemapSerial::init(this->tilemap);
    uint32_t count = 0;
    for (TilePositionToTile tile = serial.next(); tile.id != -1;
         tile = serial.next())
    {
        count++;
    }

    *(uint32_t *)alloc.alloc(sizeof(uint32_t)) = count;

    serial = BasicTilemapSerial::init(this->tilemap);
    for (TilePositionToTile tile = serial.next(); tile.id != -1;
         tile = serial.next())
    {
        *(Vector2i *)alloc.alloc(sizeof(Vector2i)) = tile.position;
        *(int32_t *)alloc.alloc(sizeof(int32_t)) = tile.id;
    }
}
