///
/// BRIEF: Main menu.
///

#ifndef H_CATEDU_MAIN_MENU
#define H_CATEDU_MAIN_MENU

#include <vector>
#include <string>
#include <filesystem>

struct MainMenu {
  std::vector<std::string> files;

  static MainMenu init(std::filesystem::path directory_path);
  void show();
};

#endif