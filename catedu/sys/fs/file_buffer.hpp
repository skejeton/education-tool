///
/// BRIEF: Binary file buffer
///

#pragma once
#include <stdint.h>
#include <stdio.h>

struct FileBuffer
{
    uint8_t *data;
    size_t size;

    static FileBuffer read_whole_file(FILE *f);
    void write_whole_file(FILE *f);
    void deinit();
};
