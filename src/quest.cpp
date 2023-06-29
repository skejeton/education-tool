#include "quest.hpp"
#include <cassert>
#include <execution>
#include <urlmon.h>
#include "imgui/imgui.h"


static void show_objective(Objective* obj, int posx)
{
	bool completed = obj->completed;
	ImVec4 color = { 1.0, 1.0, 1.0, 1.0 };
	if (completed) {
		color.x = 0.4;
		color.z = 0.4;
	}

	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::SetCursorPosX(posx);
	ImGui::Text("%s", obj->name);
	ImGui::PopStyleColor();

	color.w = 0.4;
	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::SetWindowFontScale(0.7);
	ImGui::SetCursorPosX(posx);
	ImGui::Text("%s", obj->id);
	ImGui::SetWindowFontScale(1.0);
	ImGui::PopStyleColor();
}


static void show_objective_children(ObjectivesUi *ui, ObjectiveId id, int depth = 0) 
{
	while (id != NULL_ID) {
		Objective *objective = ui->system->objectives.get(id);
		if (objective == nullptr) {
			break;
		}

		show_objective(objective, depth*50+30);
		ImGui::Separator();
		show_objective_children(ui, objective->child, depth+1);

		id = objective->sibling;
	}
}


static void attach_to_parent(ObjectiveSystem *system, ObjectiveId parent_id, ObjectiveId child_id)
{
	Objective *child = system->objectives.get(child_id);

	assert(child);

	if (parent_id == NULL_ID) {
		ObjectiveId last_child_id = system->first_element;

		while (last_child_id != NULL_ID) {
			ObjectiveId sibling = system->objectives.get(last_child_id)->sibling;
			if (sibling == NULL_ID) {
				break;
			}
			last_child_id = sibling;
		}

		if (last_child_id == NULL_ID) {
			// - If no child in the parent
			system->first_element = child_id;
		} else {
			// - If cphildren in the parent
			Objective *last_child = system->objectives.get(last_child_id);
			if (last_child) {
				last_child->sibling = child_id;
			}
		}
		return;

	} 

	Objective *parent = system->objectives.get(parent_id);

	if (parent) {
		child->parent = parent_id;

		ObjectiveId last_child_id = parent->child;

		while (last_child_id != NULL_ID) {
			ObjectiveId sibling = system->objectives.get(last_child_id)->sibling;
			if (sibling == NULL_ID) {
				break;
			}
			last_child_id = sibling;
		}

		if (last_child_id == NULL_ID) {
			// - If no child in the parent
			parent->child = child_id;
		} else {
			// - If cphildren in the parent
			Objective *last_child = system->objectives.get(last_child_id);
			if (last_child) {
				last_child->sibling = child_id;
			}
		}
	}
}


ObjectiveId ObjectiveSystem::push_objective(ObjectiveId parent_id, const char *id, const char *name)
{
	Objective to_push = {};

	to_push.id = id;
	to_push.name = name;

	ObjectiveId to_push_id = this->objectives.allocate(to_push);

	attach_to_parent(this, parent_id, to_push_id);

	return to_push_id;
}


void ObjectivesUi::show()
{
	if (ImGui::Begin("Objectives")) {
		show_objective_children(this, this->system->first_element);
	}
	ImGui::End();
}
