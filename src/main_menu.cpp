#include "main_menu.hpp"
#include <filesystem>
#include <imgui/imgui.h>

MainMenu MainMenu::init(std::filesystem::path directory_path)
{
  MainMenu menu = {};

  for (const auto &entry : std::filesystem::directory_iterator(directory_path)) {
    menu.files.push_back(entry.path().filename().generic_string());
  }

  return menu;
}

void MainMenu::show()
{
  if (ImGui::Begin("Main Menu"))
  {
    
  }
  ImGui::End();
}
