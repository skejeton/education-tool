#include "resources.hpp"


ResourceSpec load_resource_spec(const char *path)
{
    ResourceSpec result = {};

    // "bricks", "tile", "debug", "player", "counter", "glass", "door"

    result.tileset = Texture::init("./assets/test_spritesheet_01.png");
    TableId invalid_id = result.models.allocate(
        {"invalid",
         {0, 0, 32, 32},
         box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.5}),
         false});
    TableId bricks_id = result.models.allocate(
        {"bricks",
         {64, 32, 32, 32},
         box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.5}),
         true});
    TableId shuttle_id = result.models.allocate(
        {"shuttle",
         {96, 160, 32, 32},
         box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.9}),
         false});
    TableId car_id = result.models.allocate(
        {"car",
         {96 - 32, 160, 32, 32},
         box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.6}),
         false});
    TableId atm_id = result.models.allocate(
        {"atm",
         {96, 128, 32, 32},
         box3_extrude_from_point({0.0, 0.0, 0.0}, {0.45, 0.5, 0.45}),
         false});
    TableId taxi_id = result.models.allocate(
        {"taxi",
         {96 - 64, 160, 32, 32},
         box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.6}),
         false});
    TableId ad_id = result.models.allocate(
        {"ad",
         {96, 96, 32, 32},
         box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.5}),
         true});
    TableId tile_id = result.models.allocate(
        {"tile",
         {32, 32, 32, 32},
         box3_extrude_from_point({0.0, -0.45, 0.0}, {0.5, 0.05, 0.5}),
         false});
    TableId sidewalk_id = result.models.allocate(
        {"sidewalk",
         {64, 96, 32, 32},
         box3_extrude_from_point({0.0, -0.45, 0.0}, {0.5, 0.05, 0.5}),
         false});
    TableId roadr_id = result.models.allocate(
        {"roadr",
         {0, 128, 32, 32},
         box3_extrude_from_point({0.0, -0.45, 0.0}, {0.5, 0.05, 0.5}),
         false});
    TableId roadl_id = result.models.allocate(
        {"roadl",
         {0, 160, 32, 32},
         box3_extrude_from_point({0.0, -0.45, 0.0}, {0.5, 0.05, 0.5}),
         false});
    TableId darkness_id = result.models.allocate(
        {"darkness",
         {32, 128, 32, 32},
         box3_extrude_from_point({0.0, -0.45, 0.0}, {0.5, 0.05, 0.5}),
         false});
    TableId debug_id = result.models.allocate(
        {"debug",
         {64, 0, 32, 32},
         box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.5}),
         false});
    TableId counter_id = result.models.allocate(
        {"counter",
         {64, 64, 32, 32},
         box3_extrude_from_point({0.0, -0.15, 0.0}, {0.5, 0.35, 0.5}),
         false});
    TableId glass_id = result.models.allocate(
        {"glass",
         {96, 64, 32, 32},
         box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.5}),
         true,
         true});
    TableId door_id = result.models.allocate(
        {"door",
         {32, 64, 32, 32},
         box3_extrude_from_point({0.0, 0.0, 0.0}, {0.5, 0.5, 0.1}),
         true,
         true});
    TableId entity_id = result.models.allocate(
        {"entity",
         {64, 0, 32, 32},
         box3_extrude_from_point({0.0, 0.0, 0.0}, {0.3, 0.5, 0.3}),
         false});
    TableId player_id = result.models.allocate(
        {"player",
         {0, 64, 32, 32},
         box3_extrude_from_point({0.0, 0.0, 0.0}, {0.3, 0.5, 0.3}),
         false,
         false,
         true});
    TableId selector_id = result.models.allocate(
        {"selector",
         {96, 0, 32, 32},
         box3_extrude_from_point({0.0, 0.0, 0.0}, {0.51, 0.51, 0.51}),
         false,
         true,
         true});
    TableId waypoint_id = result.models.allocate(
        {"waypoint",
         {0, 96, 32, 32},
         box3_extrude_from_point({0.0, -0.40, 0.0}, {0.5, 0.1, 0.5}),
         false,
         false,
         true});
    TableId blocker_id = result.models.allocate(
        {"blocker",
         {32, 96, 32, 32},
         box3_extrude_from_point({0.0, -0.35, 0.0}, {0.5, 0.15, 0.5}),
         false,
         true,
         true});
    TableId checkpoint_id = result.models.allocate(
        {"checkpoint",
         {64, 128, 32, 32},
         box3_extrude_from_point({0.0, -0.35, 0.0}, {0.5, 0.15, 0.5}),
         false,
         true,
         true});

    result.tiles.allocate({"brick", bricks_id, true, false});
    result.tiles.allocate({"tile", tile_id, false, true});
    result.tiles.allocate({"counter", counter_id, true, false});
    result.tiles.allocate({"glass", glass_id, true, false});
    result.tiles.allocate({"door", door_id, true, false});
    result.tiles.allocate({"sidewalk", sidewalk_id, false, true});
    result.tiles.allocate({"roadr", roadr_id, false, true});
    result.tiles.allocate({"roadl", roadl_id, false, true});
    result.tiles.allocate({"darkness", darkness_id, false, true});
    result.tiles.allocate({"blocker", blocker_id, true, false});
    result.tiles.allocate({"ad", ad_id, true, false});

    return result;
}
