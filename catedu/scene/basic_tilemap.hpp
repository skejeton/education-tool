#pragma once
#include "catedu/core/alloc/free_list.hpp"
#include "catedu/core/math/math.hpp"

#define BASIC_TILEMAP_CHUNK_DIM 64
#define BASIC_TILEMAP_CHUNK_SIZE                                               \
    (BASIC_TILEMAP_CHUNK_DIM * BASIC_TILEMAP_CHUNK_DIM)

struct Chunk
{
    int data[BASIC_TILEMAP_CHUNK_SIZE];
};

struct ChunkPositionToChunk
{
    Vector2i chunk_position;
    Chunk chunk;
};

struct TilePositionToTile
{
    Vector2i position;
    int id;
};

struct BasicTilemap
{
    FreeList<ChunkPositionToChunk> chunks;

    static BasicTilemap init();
    void deinit();
    void set_tile(Vector2i tile_position, int tile_id);
    int get_tile(Vector2i tile_position);
};

struct BasicTilemapSerial
{
    BasicTilemap *tilemap;
    int chunk_id;
    int tile_id;

    static BasicTilemapSerial init(BasicTilemap &tilemap);
    void deinit();
    TilePositionToTile next();
};
