#include "quest.hpp"
#include <cassert>
#include "imgui/imgui.h"

Objective* ObjectiveList::get_objective_by_id(char* id) {
	for (auto &objective : objectives) {
		if (strcmp(objective.id, id) == 0) {
			return &objective;
		}
	}
	return nullptr;
}

void ObjectiveList::push_objective(char* id, char* name) {
	if (get_objective_by_id(id) == nullptr) {
		objectives.push_back({ false, id, name });
		return;
	}
	assert(false);
}

void ObjectiveList::complete_objective(char* id) {
	Objective* objective = get_objective_by_id(id);
	if (objective) {
		objective->completed = true;
	}
}

static void show_objective(Objective* obj) {
	bool completed = obj->completed;
	ImVec4 color = { 1.0, 1.0, 1.0, 1.0 };
	if (completed) {
		color.x = 0.4;
		color.z = 0.4;
	}

	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::Text("%s", obj->name);
	ImGui::PopStyleColor();

	color.w = 0.4;
	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::SetWindowFontScale(0.7);
	ImGui::Text("%s", obj->id);
	ImGui::SetWindowFontScale(1.0);
	ImGui::PopStyleColor();
}

void ObjectiveListUi::show() {
	if (ImGui::Begin("Objectives")) {
		for (auto& objective : list->objectives) {
			show_objective(&objective);
			ImGui::Separator();
		}
	}
	ImGui::End();
}