// This is a utility module to quickly read a file and then remove it.

#pragma once

#include "catedu/core/memory/buffer.hpp"
#include "file_buffer.hpp"
#include <assert.h>

#define READ_FILE_TEMP(name, path, code)                                       \
    do                                                                         \
    {                                                                          \
        FILE *f___ = fopen(path, "rb");                                        \
        assert(f___ && "Failed to open file");                                 \
        FileBuffer file___ = FileBuffer::read_whole_file(f___);                \
        fclose(f___);                                                          \
        Buffer name = {file___.data, file___.size};                            \
        code;                                                                  \
        file___.deinit();                                                      \
    } while (0)
