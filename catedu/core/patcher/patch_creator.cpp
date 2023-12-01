#include "patch_creator.hpp"
#include <stdlib.h>
#include <string.h>

static void write(PatchCreator *patch_creator, uint8_t byte)
{
    if (patch_creator->offs >= patch_creator->cap)
    {
        patch_creator->cap = patch_creator->cap * 2 + 1;
        patch_creator->patch->data =
            realloc(patch_creator->patch->data, patch_creator->cap);
    }

    ((uint8_t *)patch_creator->patch->data)[patch_creator->offs++] = byte;
    patch_creator->patch->size = patch_creator->offs;
}

static void writen(PatchCreator *patch_creator, void *data, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        write(patch_creator, ((uint8_t *)data)[i]);
    }
}

void PatchCreator::value(const char *section, Buffer data)
{
    for (size_t i = 0; section[i]; i++)
    {
        write(this, section[i]);
    }
    write(this, 0);
    writen(this, &data.size, sizeof(data.size));
    writen(this, data.data, data.size);
}

void PatchCreator::string(const char *section, const char *value)
{
    this->value(section, {strlen(value) + 1, (void *)value});
}
