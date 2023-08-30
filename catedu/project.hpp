///
/// BRIEF: Project data for saving and loading.
///

#ifndef H_CATEDU_PROJECT
#define H_CATEDU_PROJECT

#include "file_buffer.hpp"
#include "flashbacks.hpp"
#include "scene.hpp"
#include <array>
#include <cstdint>

struct ProjectFile
{
    FILE* f;

    static ProjectFile init_from_path(const char* path);

    FileBuffer read_project_data();
    void write_project_data(FileBuffer f);
    void deinit();
};

struct Project
{
    Scene scene;
    Flashbacks flashbacks;

    static Project load(ProjectFile f);
    void save(ProjectFile f);
};

#endif // H_CATEDU_PROJECT
