///
/// BRIEF: Utilities for ImGui
///

#ifndef H_IMGUI_UTILITIES_CATEDU
#define H_IMGUI_UTILITIES_CATEDU

#include "imgui/imgui.h"

static inline void iu_bottom_align_button() {
  float bottom = ImGui::GetWindowSize().y;
  // subtract window padding
  bottom -= ImGui::GetStyle().WindowPadding.y;
  // subtract button padding
  bottom -= ImGui::GetStyle().FramePadding.y*2;
  // subtract font size
  bottom -= ImGui::GetFontSize();
  ImGui::SetCursorPosY(bottom);
}

#endif // H_IMGUI_UTILITIES_CATEDU

