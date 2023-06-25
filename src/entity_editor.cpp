#include "entity_editor.hpp"
#include "flashbacks.hpp"
#include "imgui/imgui.h"
#include "memory.h"

#define MAX_LENGTH 1024

FlashbacksDialogPrototype allocate_prototype() {
  return FlashbacksDialogPrototype{(char*)calloc(MAX_LENGTH, 1), (char*)calloc(MAX_LENGTH, 1)};
}

void deallocate_prototype(FlashbacksDialogPrototype *prototype) {
  free(prototype->text);
  free(prototype->answer);
}

void EntityEditor::show() {
  size_t old_stage = stage;
  int i = 0;
  int delete_index = -1;
  int swap_index_first = -1;
  int swap_index_last = -1;
  ImGui::SetNextWindowSize({640, 480});
  ImGui::Begin("Entity Editor");
    ImGui::InputText("Complete objective", entity->objective_complete, sizeof entity->objective_complete);

    ImGui::SeparatorText("Dialogues");
    for (auto &dialog : this->prototypes) {
      ImGui::PushID(i);
      ImGui::InputTextMultiline("Description", dialog.text, MAX_LENGTH);
      ImGui::InputText("Answer", dialog.answer, MAX_LENGTH);
      if (ImGui::Button("Delete")) {
        delete_index = i;
      }
      ImGui::SameLine();
      ImGui::Checkbox("Numeric Guess", &dialog.numeric);
      ImGui::SameLine();
      if (ImGui::Button("^")) {
        swap_index_first = i;
        swap_index_last = i-1;
      }
      ImGui::SameLine();
      if (ImGui::Button("v")) {
        swap_index_first = i;
        swap_index_last = i+1;
      }
      ImGui::Separator();
      ImGui::PopID();
      i++;
    }

    if (swap_index_first >= 0 && swap_index_last >= 0 && swap_index_first < prototypes.size() && swap_index_last < prototypes.size()) {
      auto temp = prototypes[swap_index_first];
      prototypes[swap_index_first] = prototypes[swap_index_last];
      prototypes[swap_index_last] = temp;
    }

    if (delete_index >= 0) {
      deallocate_prototype(&this->prototypes[delete_index]);
      this->prototypes.erase(this->prototypes.begin()+delete_index);
    }

    if (ImGui::Button("+")) {
      this->prototypes.push_back(allocate_prototype());
    }
  ImGui::End();

  if (stage != old_stage) {
    size_t new_stage = stage;
    stage = old_stage;
    emplace();
    stage = new_stage;
    derive_from(entity);
  }
}

void copy_string_safe_n(char *dest, const char *src, size_t max) {
  *dest = 0;
  if (!src) return;
  for (size_t i = 0; src[i] && i < max-1; i++) {
    dest[i] = src[i];
  }
  dest[max-1] = 0;
}

static FlashbacksDialogPrototype derive_prototype_from(FlashbacksDialog dialog) {
	FlashbacksDialogPrototype prototype = allocate_prototype();
  
  copy_string_safe_n(prototype.text, dialog.text, MAX_LENGTH);
  copy_string_safe_n(prototype.answer, dialog.answer, MAX_LENGTH);
 
  return prototype;
}

void EntityEditor::derive_from(Entity* entity) {
  prototypes = {};

  this->entity = entity;

  FlashbacksDialogId id = entity->dialog_stages_id[stage];
  while (id) {
    FlashbacksDialog *dialog = flashbacks->get_from_id(id);
    if (!dialog) {
      break;
    }
    prototypes.push_back(derive_prototype_from(*dialog));
    id = dialog->next;
  }
}

void EntityEditor::emplace() {
  if (entity == nullptr) {
    return;
  }
  
  FlashbacksDialogId* start = &entity->dialog_stages_id[stage];

  flashbacks->free_sequence(*start);
  
  FlashbacksDialogMaker maker = FlashbacksDialogMaker::from(flashbacks);
	
  for (auto dialog : this->prototypes) {
    if (dialog.answer && *dialog.answer == 0) {
      dialog.answer = nullptr;
    }
    maker.append_dialog(dialog);
  }

	*start = maker.starter_id;
}
