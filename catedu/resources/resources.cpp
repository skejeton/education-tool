#include "resources.hpp"

const SpecModel models[] = {
    {"invalid",
     {0, 0, 32, 32},
     box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.5}),
     false},
    {"bricks",
     {64, 32, 32, 32},
     box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.5}),
     true},
    {"shuttle",
     {96, 160, 32, 32},
     box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.9}),
     false},
    {"car",
     {96 - 32, 160, 32, 32},
     box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.6}),
     false},
    {"stairs_up",
     {0, 192, 32, 32},
     box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.5}),
     false},
    {"stairs_down",
     {32, 192, 32, 32},
     box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.5}),
     false},
    {"atm",
     {96, 128, 32, 32},
     box3_extrude_from_point({0.0, 0.0, 0.0}, {0.45, 0.5, 0.45}),
     false},
    {"taxi",
     {96 - 64, 160, 32, 32},
     box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.6}),
     false},
    {"ad",
     {96, 96, 32, 32},
     box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.5}),
     true},
    {"tile",
     {32, 32, 32, 32},
     box3_extrude_from_point({0.0, -0.45, 0.0}, {0.5, 0.05, 0.5}),
     false},
    {"sidewalk",
     {64, 96, 32, 32},
     box3_extrude_from_point({0.0, -0.45, 0.0}, {0.5, 0.05, 0.5}),
     false},
    {"roadr",
     {0, 128, 32, 32},
     box3_extrude_from_point({0.0, -0.45, 0.0}, {0.5, 0.05, 0.5}),
     false},
    {"roadl",
     {0, 160, 32, 32},
     box3_extrude_from_point({0.0, -0.45, 0.0}, {0.5, 0.05, 0.5}),
     false},
    {"darkness",
     {32, 128, 32, 32},
     box3_extrude_from_point({0.0, -0.45, 0.0}, {0.5, 0.05, 0.5}),
     false},
    {"debug",
     {64, 0, 32, 32},
     box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.5}),
     false},
    {"counter",
     {64, 64, 32, 32},
     box3_extrude_from_point({0.0, -0.15, 0.0}, {0.5, 0.35, 0.5}),
     false},
    {"glass",
     {96, 64, 32, 32},
     box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.5}),
     true,
     true},
    {"door",
     {32, 64, 32, 32},
     box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.1}),
     true,
     true},
    {"entity",
     {64, 0, 32, 32},
     box3_extrude_from_point({0.0, 0.0, 0.0}, {0.3, 0.5, 0.3}),
     false},
    {"player",
     {0, 64, 32, 32},
     box3_extrude_from_point({0.0, 0.0, 0.0}, {0.3, 0.5, 0.3}),
     false,
     false,
     true},
    {"selector",
     {96, 0, 32, 32},
     box3_extrude_from_point({0.0, 0.0, 0.0}, {0.51, 0.51, 0.51}),
     false,
     true,
     true},
    {"pointer",
     {96, 192, 32, 32},
     box3_extrude_from_point({0.0, 0.49, 0.0}, {0.5, 0.01, 0.5}),
     false,
     true,
     true},
    {"waypoint",
     {0, 96, 32, 32},
     box3_extrude_from_point({0.0, -0.40, 0.0}, {0.5, 0.1, 0.5}),
     false,
     false,
     true},
    {"blocker",
     {32, 96, 32, 32},
     box3_extrude_from_point({0.0, -0.35, 0.0}, {0.5, 0.15, 0.5}),
     false,
     true,
     true},
    {"checkpoint",
     {64, 128, 32, 32},
     box3_extrude_from_point({0.0, -0.35, 0.0}, {0.5, 0.15, 0.5}),
     false,
     true,
     true},
};

ResourceSpec load_resource_spec(const char *path)
{
    ResourceSpec result = {};

    // "bricks", "tile", "debug", "player", "counter", "glass", "door"

    result.tileset = Texture::init("./assets/test_spritesheet_01.png");
    for (auto &model : models)
    {
        result.models.allocate(model);
    }

    TableId bricks_id = result.find_model_by_name("bricks");
    result.tiles.allocate({"brick", bricks_id, true, false});
    TableId tile_id = result.find_model_by_name("tile");
    result.tiles.allocate({"tile", tile_id, false, true});
    TableId counter_id = result.find_model_by_name("counter");
    result.tiles.allocate({"counter", counter_id, true, false});
    TableId glass_id = result.find_model_by_name("glass");
    result.tiles.allocate({"glass", glass_id, true, false});
    TableId door_id = result.find_model_by_name("door");
    result.tiles.allocate({"door", door_id, true, false});
    TableId sidewalk_id = result.find_model_by_name("sidewalk");
    result.tiles.allocate({"sidewalk", sidewalk_id, false, true});
    TableId roadr_id = result.find_model_by_name("roadr");
    result.tiles.allocate({"roadr", roadr_id, false, true});
    TableId roadl_id = result.find_model_by_name("roadl");
    result.tiles.allocate({"roadl", roadl_id, false, true});
    TableId darkness_id = result.find_model_by_name("darkness");
    result.tiles.allocate({"darkness", darkness_id, false, true});
    TableId blocker_id = result.find_model_by_name("blocker");
    result.tiles.allocate({"blocker", blocker_id, true, false});
    TableId ad_id = result.find_model_by_name("ad");
    result.tiles.allocate({"ad", ad_id, true, false});

    return result;
}
