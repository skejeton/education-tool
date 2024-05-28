#include "world_file.hpp"

void WorldFile::save(const char *path, Dispatcher &dispatcher)
{
    FILE *file = fopen(path, "wb");
    if (!file)
    {
        return;
    }

    uint32_t count = 0;
    for (auto &_ : iter(dispatcher.world.places))
    {
        count++;
    }

    fwrite(&count, sizeof(count), 1, file);

    for (auto &place : iter(dispatcher.world.places))
    {
        uint32_t count = 0;
        for (auto &_ : iter(place.objects))
        {
            count++;
        }

        fwrite(&count, sizeof(count), 1, file);

        for (auto &obj : iter(place.objects))
        {
            fwrite(&obj, sizeof(obj), 1, file);
        }
    }

    fclose(file);

    dispatcher.dirty = false;
}

Dispatcher WorldFile::load(const char *path)
{
    FILE *file = fopen(path, "rb");

    Dispatcher dispatcher = Dispatcher::create();

    if (!file)
    {
        return dispatcher;
    }

    uint32_t count = 0;
    fread(&count, sizeof(count), 1, file);

    for (size_t i = 0; i < count; i++)
    {
        Place place = Place::create();

        uint32_t count = 0;
        fread(&count, sizeof(count), 1, file);

        for (size_t i = 0; i < count; i++)
        {
            Object obj = {};
            fread(&obj, sizeof(obj), 1, file);

            *place.objects.alloc() = obj;
        }

        *dispatcher.world.places.alloc() = place;
    }

    fclose(file);

    return dispatcher;
}
