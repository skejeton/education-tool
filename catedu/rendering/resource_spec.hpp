#pragma once
#include "catedu/core/math/math.hpp"
#include "catedu/core/storage/table.hpp"
#include "catedu/rendering/3d/model.hpp"
#include "catedu/rendering/texture.hpp"

struct SpecModel
{
    const char *name;
    catedu::Model model;
    bool if_editor_only;
};

struct SpecTile
{
    const char *name;
    TableId model_id;
    bool if_obstacle;
};

struct ResourceSpec
{
    Texture tileset;
    Table<SpecModel> models;
    Table<SpecTile> tiles;

    TableId find_model_by_name(const char *name);
    void deinit();
};
