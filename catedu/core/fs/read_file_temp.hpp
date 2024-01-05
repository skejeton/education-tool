// This is a utility module to quickly read a file and then remove it.

#pragma once

#include "catedu/file_buffer.hpp"

#define READ_FILE_TEMP(name, path, code) \
    {FILE *f___ = fopen(path, "rb"); \
     FileBuffer file___ = FileBuffer::read_whole_file(f___); \
     fclose(f___); \
     Buffer name = {file___.size, file___.data}; \
    code;                         \
    file___.deinit();\
    }
