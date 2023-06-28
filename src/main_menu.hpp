///
/// BRIEF: Main menu.
///

#ifndef H_CATEDU_MAIN_MENU
#define H_CATEDU_MAIN_MENU

#include <vector>
#include <string>
#include <filesystem>

#define MENU_FILE_NAME_SIZE 512

struct OpenProject {
  std::string path;
  bool is_open;
};

struct MainMenu {
  std::vector<std::string> files;
  std::filesystem::path directory_path;
  int file_selected;
  char file_name[MENU_FILE_NAME_SIZE];

  static MainMenu init(std::filesystem::path directory_path);
  void show(OpenProject *project);
};

#endif
