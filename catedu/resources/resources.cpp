#include "resources.hpp"

struct ModelProto
{
    const char *name;
    const char *path;
};

struct TileProto
{
    const char *name;
    const char *model_name;
    bool if_obstacle;
    int rotation;
};

const ModelProto model_protos[] = {{"invalid", "./assets/models/barrel.gltf"},
                                   {"selector", "./assets/models/counter.gltf"},
                                   {"barrel", "./assets/models/barrel.gltf"},
                                   {"car", "./assets/models/car.gltf"},
                                   {"counter", "./assets/models/counter.gltf"},
                                   {"tile", "./assets/models/tile.gltf"},
                                   {"wall", "./assets/models/wall_edge.gltf"}};

const TileProto tile_protos[] = {
    {"barrel", "barrel", true},      {"counter", "counter", true},
    {"tile", "tile", false},         {"wall_west", "wall", true, 0},
    {"wall_north", "wall", true, 1}, {"wall_east", "wall", true, 2},
    {"wall_south", "wall", true, 3}};

ResourceSpec load_resource_spec(const char *path)
{
    ResourceSpec result = {};

    result.tileset = Texture::init("./assets/test_spritesheet_01.png");
    for (auto &proto : model_protos)
    {
        catedu::RawModel raw_model;
        bool ok = catedu::RawModel::load_gltf(proto.path, raw_model);
        assert(ok && "Failed to load model");
        catedu::Model model;
        ok = catedu::Model::load_from_raw(raw_model, model);
        assert(ok && "Failed to create model");
        result.models.allocate({proto.name, model, false});
    }

    for (auto &proto : tile_protos)
    {
        TableId model_id = result.find_model_by_name(proto.model_name);
        result.tiles.allocate(
            {proto.name, model_id, proto.if_obstacle, proto.rotation});
    }

    return result;
}
