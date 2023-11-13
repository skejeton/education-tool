#ifndef HPP_CATEDU_STAGING_WORLD_WORLD_DEFINED
#define HPP_CATEDU_STAGING_WORLD_WORLD_DEFINED

#include "catedu/boxdraw.hpp"
#include "catedu/staging/texture.hpp"

struct World
{
    Texture tileset;
    int data[64][64];
    Vector2 camera_pos;
    int camera_rot;
    bool is_editor;

    static World init(Texture tileset);
    void deinit();

    Vector2i map_index_to_pos(int i, int j);

    void render(BoxdrawRenderer& boxdraw);
};

#endif
