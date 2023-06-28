#include "main_menu.hpp"
#include <filesystem>
#include <imgui/imgui.h>
#include <sokol/sokol_app.h>
#include <stdio.h>

void show_file_list(MainMenu *main_menu, ImVec2 size)
{
  if (ImGui::BeginListBox("##File List", size)) {

    int i = 0;
    for (auto &file : main_menu->files) {
      if (ImGui::Selectable(file.c_str(), main_menu->file_selected == i)) {
        main_menu->file_selected = i;
      }
      i++;
    }

    ImGui::EndListBox();
  }
}


void synchronize_file_list(MainMenu *main_menu)
{
  main_menu->file_selected = 0;
  main_menu->files = {};

  for (const auto &entry : std::filesystem::directory_iterator(main_menu->directory_path)) {
    main_menu->files.push_back(entry.path().filename().generic_string());
  }
}


void center_next_window_window(ImVec2 size)
{
  ImGui::SetNextWindowPos({(sapp_widthf()-size.x)/2.0f, (sapp_heightf()-size.y)/2.0f});
  ImGui::SetNextWindowSize(size);
}


std::string get_selected_file_filename(MainMenu *main_menu)
{
  return main_menu->files[main_menu->file_selected];
}


void remove_selected_file(MainMenu *main_menu)
{
  if (main_menu->files.size() > 0) {
    std::filesystem::remove(main_menu->directory_path / get_selected_file_filename(main_menu));
    synchronize_file_list(main_menu);
  }
}


void create_file(MainMenu *main_menu, const char *filename)
{
  if (filename[0] == 0) {
    return;
  }

  auto path = (main_menu->directory_path / filename).string();
  FILE *f = fopen(path.c_str(), "wb");
  fclose(f);
  synchronize_file_list(main_menu);
}


void open_selected_file(MainMenu *main_menu, OpenProject *project)
{
  project->path = (main_menu->directory_path / get_selected_file_filename(main_menu)).string();
  project->is_open = true;
}


MainMenu MainMenu::init(std::filesystem::path directory_path)
{
  MainMenu menu = {};

  menu.directory_path = directory_path;
  synchronize_file_list(&menu);

  return menu;
}

void MainMenu::show(OpenProject *project)
{
  center_next_window_window({400, 300});
  if (ImGui::Begin("Main Menu")) {
    show_file_list(this, {200, 240});
    
    float y = 45;

    ImGui::SetCursorPos({230, y});
    ImGui::SetNextItemWidth(150);
    ImGui::InputText("##File Name", this->file_name, MENU_FILE_NAME_SIZE);

    y += 40;

    ImGui::SetCursorPos({230, y});
    if (ImGui::Button("Create", {150, 30})) {
      create_file(this, this->file_name);
      this->file_name[0] = 0;
    }

    y += 40;

    ImGui::SetCursorPos({230, y});
    if (ImGui::Button("Open", {150, 30})) {
      open_selected_file(this, project);
    }

    y += 40;

    ImGui::SetCursorPos({230, y});
    if (ImGui::Button("Remove", {150, 30})) {
      remove_selected_file(this);
    }
  }
  
  ImGui::End();
}
