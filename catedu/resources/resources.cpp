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
};

const ModelProto model_protos[] = {{"barrel", "./assets/models/barrel.gltf"},
                                   {"car", "./assets/models/car.gltf"},
                                   {"counter", "./assets/models/counter.gltf"},
                                   {"tile", "./assets/models/tile.gltf"},
                                   {"wall", "./assets/wall_edge.gltf"}};

const TileProto tile_protos[] = {{"barrel", "barrel", true},
                                 {"counter", "counter", true},
                                 {"tile", "tile", false},
                                 {"wall", "wall", true}};

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
        result.tiles.allocate({proto.name, model_id, proto.if_obstacle});
    }

    return result;
}
