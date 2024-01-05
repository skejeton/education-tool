#include "legacy_scene.hpp"


LegacyScene LegacyScene::init(Buffer file)
{
    LegacyScene scene = {};

    scene.ground = ObjTilemap::init();
    scene.middle = ObjTilemap::init();

    // ground
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            scene.ground.set_tile({i, j}, ((int*)file.data)[i*64+j]);
        }
    }

    // middle
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            scene.middle.set_tile({i, j}, ((int*)file.data)[(64*64)+i*64+j]);
        }
    }

    return scene;
}

void LegacyScene::render(BoxdrawRenderer &renderer, ResourceSpec &resources)
{
    this->ground.render(renderer, resources);
    this->middle.render(renderer, resources);
}

void LegacyScene::deinit()
{
    this->ground.deinit();
    this->middle.deinit();
}
