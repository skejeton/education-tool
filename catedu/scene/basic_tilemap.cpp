#include "basic_tilemap.hpp"

ChunkPositionToChunk& get_or_create_chunk(BasicTilemap *tilemap, Vector2i chunk_position)
{
    for (auto [id, chunk] : iter(tilemap->chunks)) {
        if (chunk.chunk_position == chunk_position) {
            return chunk;
        }
    }

    ChunkPositionToChunk p = {};
    p.chunk_position = chunk_position;
    TableId id = tilemap->chunks.allocate(p);

    return tilemap->chunks.get_assert(id);
}

int BasicTilemapSerial::next()
{
    if (this->tile_id > BASIC_TILEMAP_CHUNK_SIZE)
    {
        this->tile_id = 0;
        this->chunk_id++;
    }

    if (this->chunk_id >= this->tilemap->chunks.count)
    {
        return -1;
    }

    ChunkPositionToChunk *p = this->tilemap->chunks.get({(size_t)this->chunk_id});

    return p->chunk.data[this->tile_id];
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
    Vector2i chunk_position = tile_position / BASIC_TILEMAP_CHUNK_DIM;
    Vector2i chunk_tile_position = tile_position % BASIC_TILEMAP_CHUNK_DIM;

    ChunkPositionToChunk &p = get_or_create_chunk(this, chunk_position);

    p.chunk.data[chunk_tile_position.y * BASIC_TILEMAP_CHUNK_DIM + chunk_tile_position.x] = tile_id;
}

int BasicTilemap::get_tile(Vector2i tile_position)
{
    Vector2i chunk_position = tile_position / BASIC_TILEMAP_CHUNK_DIM;
    Vector2i chunk_tile_position = tile_position % BASIC_TILEMAP_CHUNK_DIM;

    ChunkPositionToChunk &p = get_or_create_chunk(this, chunk_position);

    return p.chunk.data[chunk_tile_position.y * BASIC_TILEMAP_CHUNK_DIM + chunk_tile_position.x];
}
