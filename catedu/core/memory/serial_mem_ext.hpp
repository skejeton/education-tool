#pragma once
#include "serial_mem.hpp"
#include <string.h>

namespace serial_mem_ext
{

inline size_t write_string(SerialMem *serial_mem, const char *string)
{
    size_t remaining = 0;

    size_t size = strlen(string) + 1;
    remaining += serial_mem->write_value(size);
    remaining += serial_mem->write_buffer({(void *)string, size});

    return remaining;
}

inline size_t read_string(SerialMem *serial_mem, char **string, size_t *size)
{
    size_t remaining = 0;

    remaining += serial_mem->read_value(size);
    *string = (char *)serial_mem->pointer();
    remaining += serial_mem->read_buffer({string, *size});

    return remaining;
}

} // namespace serial_mem_ext
