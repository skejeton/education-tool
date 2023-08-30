///
/// BRIEF: Main menu.
///

#ifndef H_CATEDU_MAIN_MENU
#define H_CATEDU_MAIN_MENU

#include <filesystem>
#include <string>
#include <vector>

#define MENU_STRING_SIZE 512

enum class HostingType
{
    SINGLEPLAYER,
    HOST,
    JOIN
};

struct OpenProject
{
    std::string path;
    std::string host_name;
    HostingType hosting_type;
    bool is_open;
};

struct MainMenu
{
    std::vector<std::string> files;
    std::filesystem::path directory_path;
    int file_selected;
    char file_name[MENU_STRING_SIZE];
    char host_name[MENU_STRING_SIZE];

    static MainMenu init(std::filesystem::path directory_path);
    bool show(OpenProject* project);
};

#endif
