#include "file_buffer.hpp"
#include "catedu/sys/oom.hpp"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

FileBuffer FileBuffer::read_whole_file(FILE *f)
{
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t *data = (uint8_t *)OOM_HANDLER(malloc(file_size));

    fread(data, 1, file_size, f);

    return {data, file_size};
}

void FileBuffer::write_whole_file(FILE *f)
{
    fwrite(data, 1, size, f);
}

void FileBuffer::deinit()
{
    free(data);
}
