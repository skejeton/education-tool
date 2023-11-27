#ifndef HPP_CATEDU_STAGING_WORLD_WORLD_DEFINED
#define HPP_CATEDU_STAGING_WORLD_WORLD_DEFINED

#include "catedu/boxdraw.hpp"
#include "catedu/staging/texture.hpp"
#include "resource_spec.hpp"

struct WorldEntity
{
    char name[32];
    char model_name[32];
    char interact_script[32];
    char id[32];
    Vector2 pos;
};

struct World
{
    int ground[64][64];
    int middle[64][64];
    Table<WorldEntity> entities;
    Vector2 camera_pos;
    int camera_rot;
    bool is_editor;
    bool if_grass;

    static World init();
    void deinit();

    Vector2i map_index_to_pos(int i, int j);

    void render(ResourceSpec &res, BoxdrawRenderer &boxdraw);
};

#endif
