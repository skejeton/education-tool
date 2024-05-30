#include "world_file.hpp"

// TODO: Get rid of this and use handles instead
struct PtrToHandle
{
    void *ptr;
    uint32_t handle;
};

struct Mapper
{
    PtrToHandle mappings[64];
    size_t count;
};

void pushmapper(Mapper *mapper, void *ptr, uint32_t handle)
{
    assert(mapper->count < 64);
    mapper->mappings[mapper->count++] = {ptr, handle + 1};
}

uint32_t gethandle(Mapper *mapper, void *ptr)
{
    for (size_t i = 0; i < mapper->count; i++)
    {
        if (mapper->mappings[i].ptr == ptr)
        {
            return mapper->mappings[i].handle;
        }
    }

    return 0;
}

void *getptr(Mapper *mapper, uint32_t handle)
{
    for (size_t i = 0; i < mapper->count; i++)
    {
        if (mapper->mappings[i].handle == handle)
        {
            return mapper->mappings[i].ptr;
        }
    }

    return nullptr;
}

void WorldFile::save(const char *path, Dispatcher &dispatcher)
{
    Mapper mapper = {};

    FILE *file = fopen(path, "wb");
    if (!file)
    {
        return;
    }

    for (auto &place : iter(dispatcher.world.places))
    {
        pushmapper(&mapper, &place, mapper.count);
    }

    uint32_t place_count = mapper.count;
    fwrite(&place_count, sizeof(place_count), 1, file);

    for (auto &place : iter(dispatcher.world.places))
    {
        uint32_t count = 0;
        for (auto &_ : iter(place.objects))
        {
            count++;
        }

        fwrite(&count, sizeof(count), 1, file);
        fwrite(&place.interior, sizeof(place.interior), 1, file);

        for (auto obj : iter(place.objects))
        {
            // HACK: Reassigning a pointer with a handle to use in fwrite
            // FIX BY TOMORROW, THE WORLDS WILL NOT BE COMPATIBLE ON WEB
            // DUE TO DIFFERENCE IN POINTER SIZES.
            obj.place = (Place *)(size_t)gethandle(&mapper, obj.place);

            fwrite(&obj, sizeof(obj), 1, file);
        }
    }

    fclose(file);

    dispatcher.dirty = false;
}

Dispatcher WorldFile::load(const char *path)
{
    Mapper mapper = {};

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
        Place *place;
        if (i == 0)
        {
            place = dispatcher.world.first;
        }
        else
        {
            place = dispatcher.world.places.alloc();
            *place = Place::create();
        }

        pushmapper(&mapper, place, i);
    }

    for (size_t i = 1; i <= count; i++)
    {
        Place *place = (Place *)getptr(&mapper, i);

        uint32_t count = 0;
        fread(&count, sizeof(count), 1, file);
        fread(&place->interior, sizeof(place->interior), 1, file);

        for (size_t i = 0; i < count; i++)
        {
            Object obj = {};
            fread(&obj, sizeof(obj), 1, file);

            obj.place = (Place *)getptr(&mapper, (uint32_t)(size_t)obj.place);
            place->place_object(obj);
        }
    }

    fclose(file);

    return dispatcher;
}
