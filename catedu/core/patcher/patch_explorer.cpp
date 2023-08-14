#include "patch_explorer.hpp"
#include <assert.h>
#include "catedu/core/memory/serial_mem_ext.hpp"

static PatchSectionData parse_data_section(SerialMem *serial, PatchExplorer *explorer)
{
    PatchSectionData data;
    // serial->read_value(&data.buffer);
    return data;
}

static PatchSectionScope parse_scope_section(SerialMem *serial, PatchExplorer *explorer)
{
    PatchSectionScope scope;
    serial->read_value(&scope.subsection_count);
    return scope;
}

static PatchSection parse_section(SerialMem *serial, PatchExplorer *explorer)
{
    namespace ext = serial_mem_ext;

    PatchSection section = {};
    size_t section_name_length;

    assert(serial->read_value(&section.type) == 0);
    assert(ext::read_string(serial, &section.name, &section_name_length) == 0);

    switch (section.type) {
        case PatchSection::Type::Data:
            parse_data_section(serial, explorer);
        case PatchSection::Type::Scope:
            parse_scope_section(serial, explorer);
        default:
            assert(false);
    }
}


PatchExplorer PatchExplorer::from_patch(Patch *patch)
{
    return {patch};
}

PatchSection PatchExplorer::get_section(const char *section)
{

    auto serial = SerialMem::from_buffer({this->patch->size, this->patch->data});
    /*


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
    */

    return {};
}
