#include "obj_tilemap.hpp"

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

void ObjTilemap::render(BoxdrawRenderer &renderer, ResourceSpec &resources)
{
    BasicTilemapSerial serial = BasicTilemapSerial::init(this->tilemap);
    for (TilePositionToTile tile = serial.next(); tile.id != 0; tile = serial.next())
    {
        Vector3 pos = {(float)tile.position.x, 0, (float)tile.position.y};
        // TODO make link between tile_id and tileset
    }
}
