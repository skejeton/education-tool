#include "patch_explorer.hpp"

Buffer PatchExplorer::get_section(const char *section)
{
    size_t i = 0;
    while (i < this->patch->size) {
        const char *name = (const char *)((uint8_t*)this->patch->data+i);
        size_t name_size = strlen((const char *)((uint8_t*)this->patch->data+i))+1;
        i += name_size;
        size_t data_size = *(size_t*)((uint8_t*)((uint8_t*)this->patch->data+i));
        i += sizeof(size_t);
        uint8_t *data = (uint8_t*)this->patch->data+i;
        i += data_size;

        if (strcmp(name, section) == 0) {
            return {data_size, data};
        }
    }

    return {};
}
