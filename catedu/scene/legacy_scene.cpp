#include "legacy_scene.hpp"


LegacyScene LegacyScene::init(Buffer file)
{
    LegacyScene scene = {};

    scene.ground = ObjTilemap::init();
    scene.middle = ObjTilemap::init();
    scene.backdrop = ObjBackdrop::init({0, 32, 32, 32});

    // ground
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            scene.ground.set_tile({i, j}, ((int*)file.data)[i*64+j]);
        }
    }

    file.data = (uint8_t*)file.data + sizeof(int) * 64 * 64;

    // middle
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            scene.middle.set_tile({i, j}, ((int*)file.data)[i*64+j]);
        }
    }

    file.data = (uint8_t*)file.data + sizeof(int) * 64 * 64;

    // entities
    scene.entity_count = *((uint64_t*)file.data);
    file.data = (uint8_t*)file.data + sizeof(uint64_t);
    for (int i = 0; i < scene.entity_count; i++)
    {
        struct WorldEntity
        {
            char name[32];
            char model_name[32];
            char interact_script[32];
            char id[32];
            Vector2 pos;
        };

        scene.entities[i] = ObjEntity::init(
            ((WorldEntity*)file.data)->model_name,
            ((WorldEntity*)file.data)->pos+Vector2{32, 32});
        file.data = (uint8_t*)file.data + sizeof(WorldEntity);
    }


    return scene;
}

void LegacyScene::render(BoxdrawRenderer &renderer, ResourceSpec &resources)
{
    this->backdrop.render(renderer, resources);
    this->ground.render(renderer, resources);
    this->middle.render(renderer, resources);
    for (size_t i = 0; i < this->entity_count; i++)
    {
        this->entities[i].render(renderer, resources);
    }
}

void LegacyScene::deinit()
{
    this->backdrop.deinit();
    this->ground.deinit();
    this->middle.deinit();
    for (size_t i = 0; i < this->entity_count; i++)
    {
        this->entities[i].deinit();
    }
}
