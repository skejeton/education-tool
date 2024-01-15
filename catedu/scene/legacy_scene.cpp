#include "legacy_scene.hpp"

Scene LegacyScene::load_data_to_scene(Buffer file)
{
    Scene result = Scene::init();

    auto backdrop = ObjBackdrop::init({0, 32, 32, 32});
    auto ground = ObjTilemap::init();
    auto middle = ObjTilemap::init();

    // ground
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            ground.set_tile({i, j}, ((int *)file.data)[i * 64 + j]);
        }
    }

    file.data = (uint8_t *)file.data + sizeof(int) * 64 * 64;

    // middle
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            middle.set_tile({i, j}, ((int *)file.data)[i * 64 + j]);
        }
    }

    file.data = (uint8_t *)file.data + sizeof(int) * 64 * 64;

    result.add_object(Scene::object(backdrop));
    result.add_object(Scene::object(ground));
    result.add_object(Scene::object(middle));

    // entities
    uint32_t entity_count = *((uint64_t *)file.data) - 1;
    file.data = (uint8_t *)file.data + sizeof(uint64_t);
    for (int i = 0; i < entity_count; i++)
    {
        struct WorldEntity
        {
            char name[32];
            char model_name[32];
            char interact_script[32];
            char id[32];
            Vector2 pos;
        };

        Object obj = Scene::object(
            ObjEntity::init(((WorldEntity *)file.data)->model_name,
                            ((WorldEntity *)file.data)->pos + Vector2{32, 32}));

        memcpy(obj.id, ((WorldEntity *)file.data)->id, 32);
        result.add_object(obj);

        file.data = (uint8_t *)file.data + sizeof(WorldEntity);
    }

    return result;
}
