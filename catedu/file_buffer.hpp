///
/// BRIEF: Binary file buffer
///

#ifndef H_CATEDU_FILE_BUFFER
#define H_CATEDU_FILE_BUFFER

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

#endif // H_CATEDU_FILE_BUFFER
