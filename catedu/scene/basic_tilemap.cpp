#include "basic_tilemap.hpp"

static ChunkPositionToChunk &get_or_create_chunk(BasicTilemap *tilemap,
                                                 Vector2i chunk_position)
{
    for (auto [id, chunk] : iter(tilemap->chunks))
    {
        if (chunk.chunk_position == chunk_position)
        {
            return chunk;
        }
    }

    ChunkPositionToChunk p = {};
    p.chunk_position = chunk_position;
    TableId id = tilemap->chunks.allocate(p);

    return tilemap->chunks.get_assert(id);
}

static Vector2i chunk_tile_position_to_tile_position(Vector2i chunk_position,
                                                     int tile_id)
{
    Vector2i chunk_tile_position = {tile_id % BASIC_TILEMAP_CHUNK_DIM,
                                    tile_id / BASIC_TILEMAP_CHUNK_DIM};
    return chunk_position * BASIC_TILEMAP_CHUNK_DIM + chunk_tile_position;
}

static int *tile_address(BasicTilemap *tilemap, Vector2i tile_position)
{
    Vector2i chunk_position = tile_position / BASIC_TILEMAP_CHUNK_DIM;
    Vector2i chunk_tile_position = tile_position % BASIC_TILEMAP_CHUNK_DIM;

    // Negative modulo is not what we want
    if (chunk_tile_position.x < 0)
    {
        chunk_tile_position.x += BASIC_TILEMAP_CHUNK_DIM;
        chunk_position.x -= 1;
    }
    if (chunk_tile_position.y < 0)
    {
        chunk_tile_position.y += BASIC_TILEMAP_CHUNK_DIM;
        chunk_position.y -= 1;
    }

    ChunkPositionToChunk &chunk = get_or_create_chunk(tilemap, chunk_position);
    return &chunk.chunk.data[chunk_tile_position.y * BASIC_TILEMAP_CHUNK_DIM +
                             chunk_tile_position.x];
}

BasicTilemapSerial BasicTilemapSerial::init(BasicTilemap &tilemap)
{
    BasicTilemapSerial serial = {};
    serial.tilemap = &tilemap;
    return serial;
}

void BasicTilemapSerial::deinit()
{
}

TilePositionToTile BasicTilemapSerial::next()
{
    for (; chunk_id < tilemap->chunks.count; chunk_id++)
    {
        ChunkPositionToChunk &chunk =
            tilemap->chunks.get_assert({(size_t)chunk_id + 1, 0});
        for (; tile_id < BASIC_TILEMAP_CHUNK_SIZE; tile_id++)
        {
            if (chunk.chunk.data[tile_id] != 0)
            {
                TilePositionToTile tile = {};
                tile.position = chunk_tile_position_to_tile_position(
                    chunk.chunk_position, tile_id);
                tile.id = chunk.chunk.data[tile_id];
                tile_id++;
                return tile;
            }
        }
        tile_id = 0;
    }

    TilePositionToTile tile = {};
    tile.id = -1;
    return tile;
}

BasicTilemap BasicTilemap::init()
{
    return BasicTilemap();
}

void BasicTilemap::deinit()
{
    this->chunks.deinit();
}

void BasicTilemap::set_tile(Vector2i tile_position, int tile_id)
{
    *tile_address(this, tile_position) = tile_id;
}

int BasicTilemap::get_tile(Vector2i tile_position)
{
    return *tile_address(this, tile_position);
}
