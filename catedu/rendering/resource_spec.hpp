#pragma once
#include "catedu/core/storage/table.hpp"
#include "catedu/rendering/3d/model.hpp"
#include "catedu/rendering/texture.hpp"

struct SpecModel
{
    const char *name;
    Model model;
    bool if_editor_only;
};

struct SpecTile
{
    const char *name;
    TableId model_id;
    bool if_obstacle;
    int rotation;
};

struct ResourceSpec
{
    Texture tileset;
    Table<SpecModel> models;
    Table<SpecTile> tiles;

    TableId find_model_by_name(const char *name);
    TableId find_tile_by_name(const char *name);
    void deinit();
};
