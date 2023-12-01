#include "patch_applier.hpp"
#include <assert.h>

void PatchApplier::value(const char *section, Buffer data)
{
    /*
    Buffer buf = this->explorer.get_section(section);

    assert(data.size == buf.size);

    memcpy(data.data, buf.data, data.size);
    */
}

void PatchApplier::string(const char *section, char **value)
{
    /*
    Buffer buf = this->explorer.get_section(section);

    *value = (char*)malloc(buf.size);
    memcpy(*value, buf.data, buf.size);
    */
}
