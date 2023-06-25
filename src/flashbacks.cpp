#include <cassert>
#include "imgui/imgui.h"
#include "imgui_utilities.hpp"
#include "flashbacks.hpp"
#include <cstdio>
#include "util.hpp"

FlashbacksAllocatedDialog Flashbacks::alloc_dialog() {
  for (size_t i = 0; i < FLASHBACKS_DIALOGS_MAX; i++) {
    if (this->dialogs[i].taken == false) {
      this->dialogs[i] = {};
      this->dialogs[i].taken = true;
      return { i + 1, &this->dialogs[i] };
    }
  }

  return { 0 };
}


FlashbacksDialog *Flashbacks::get_from_id(FlashbacksDialogId id) {
  size_t index = id - 1;
  if (index >= FLASHBACKS_DIALOGS_MAX) {
    return nullptr;
  }
  
  return &this->dialogs[index];
}


void Flashbacks::touch(FlashbacksDialogId id, FlashbacksDialogChoice choice) {
  FlashbacksDialog *dialog = this->get_from_id(id);
  assert(dialog && "Invalid dialog ID");

  dialog->choice = choice;
  
  int index = -1, i = 0;

  for (auto item : this->backlog) {
    if (item == id) {
      index = i;
      break;
    }
    i++;
  }

  if (index == -1) {
    backlog.push_back(id);
  } else {
    backlog.erase(backlog.begin()+index);
    backlog.push_back(id);
  }
}

static FlashbacksDialogId allocate_dialog_sequential(Flashbacks *flashbacks, FlashbacksDialogPrototype proto, FlashbacksDialogId previous_id) {
  FlashbacksAllocatedDialog allocated = flashbacks->alloc_dialog();
  assert(allocated.id && "Couldn't allocate!");

  if (allocated.id) {
    allocated.pointer->text = string_duplicate(proto.text);
    allocated.pointer->answer = string_duplicate(proto.answer);
    allocated.pointer->numeric = proto.numeric;

    if (previous_id != 0) {
      FlashbacksDialog *previous = flashbacks->get_from_id(previous_id);

      previous->next = allocated.id;
      allocated.pointer->prev = previous_id;
    }
  }

  return allocated.id;
}

void FlashbacksDialogMaker::append_dialog(FlashbacksDialogPrototype proto) {
  FlashbacksDialogId id = allocate_dialog_sequential(flashbacks, proto, previous_id);

  if (starter_id == 0) {
    this->starter_id = id;
  }

  previous_id = id;
}


FlashbacksDialogMaker FlashbacksDialogMaker::from(Flashbacks *flashbacks) {
  FlashbacksDialogMaker result = {};
  result.flashbacks = flashbacks;

  return result;
}


void FlashbacksGui::begin_sequence(FlashbacksDialogId start) {
  // if ID is 0, resort to inactive state
  if (start == 0) {
    this->mode = this->prev_mode;
    this->sequence_current = 0;
  } else {
    this->mode = Mode::SEQUENCE;
    this->sequence_current = start;
    this->answer_mode = AnswerMode::UNKNOWN;
  }

  this->answer[0] = 0;
}

static float
convert_string_to_numeric_answer(const char *str)
{
  char *end_ptr;
  return strtof(str, &end_ptr);
}

static bool
compare_numeric_answer_equal(const char *a, const char *b)
{
  return convert_string_to_numeric_answer(a) == convert_string_to_numeric_answer(b);
}

FlashbacksEvent show_sequence(FlashbacksGui *gui) {
  FlashbacksDialog *dialog = gui->flashbacks->get_from_id(gui->sequence_current);
  assert(dialog && "Invalid dialog ID");

  ImGui::SetNextWindowPos({ (1024-400)/2.0f, (786-300)/2.0f });
  ImGui::SetNextWindowSize({ 400, 300 });
  ImGui::Begin("Dialogue");

  {
    ImGui::TextWrapped("%s", dialog->text);
    
    if (gui->answer_mode != FlashbacksGui::AnswerMode::UNKNOWN) {
      if (dialog->choice == FlashbacksDialogChoice::CORRECT) {
        ImGui::PushStyleColor(ImGuiCol_Text, { 0, 1, 0, 1 });
      } else {
        ImGui::PushStyleColor(ImGuiCol_Text, { 1, 0, 0, 1 });
      }
      ImGui::TextWrapped("Answer: %s", dialog->answer);
      ImGui::PopStyleColor();
    }

    iu_bottom_align_button();

    if (dialog->answer) {
      switch (gui->answer_mode) {
      case FlashbacksGui::AnswerMode::UNKNOWN:
        if (dialog->numeric) {
          if (ImGui::Button("Guess")) {
            if (compare_numeric_answer_equal(gui->answer, dialog->answer))  {
              gui->flashbacks->touch(gui->sequence_current, FlashbacksDialogChoice::CORRECT);
              gui->answer_mode = FlashbacksGui::AnswerMode::CHOSE_ANSWER;
            } else {
              gui->flashbacks->touch(gui->sequence_current, FlashbacksDialogChoice::WRONG);
              gui->answer_mode = FlashbacksGui::AnswerMode::CHOSE_ANSWER;
            }
          }
          ImGui::SameLine();
          ImGui::InputText("Answer", gui->answer, sizeof gui->answer);
        } else {
          if (ImGui::Button("Show Answer")) {
            gui->answer_mode = FlashbacksGui::AnswerMode::SEEN_ANSWER;
          }
        }
        break;
      case FlashbacksGui::AnswerMode::SEEN_ANSWER:
        if (ImGui::Button("Guessed Wrong")) {
          gui->flashbacks->touch(gui->sequence_current, FlashbacksDialogChoice::WRONG);
          gui->answer_mode = FlashbacksGui::AnswerMode::CHOSE_ANSWER;
        }
        ImGui::SameLine();
        if (ImGui::Button("Guessed Correct")) {
          gui->flashbacks->touch(gui->sequence_current, FlashbacksDialogChoice::CORRECT);
          gui->answer_mode = FlashbacksGui::AnswerMode::CHOSE_ANSWER;
        }
        break;
      case FlashbacksGui::AnswerMode::CHOSE_ANSWER:
        if (ImGui::Button("Next")) {
          gui->begin_sequence(dialog->next);
        }
        break;
      }
    } else {
      if (ImGui::Button("Next")) {
        gui->flashbacks->touch(gui->sequence_current, FlashbacksDialogChoice::UNDEFINED);
        gui->begin_sequence(dialog->next);
      }
    }
  }
  ImGui::End();

  if (gui->sequence_current == 0) {
    return FlashbacksEvent::COMPLETED;
  }

  return FlashbacksEvent::NONE;
}

FlashbacksGui FlashbacksGui::create(Flashbacks *flashbacks) {
  FlashbacksGui result = {};
  result.flashbacks = flashbacks;

  return result;
}

void FlashbacksGui::toggle_backlog() {
  if (this->mode == Mode::BACKLOG) {
    this->mode = Mode::INACTIVE;
  } else {
    this->mode = Mode::BACKLOG;
  }
}

FlashbacksEvent FlashbacksGui::show() {
  FlashbacksEvent event = FlashbacksEvent::NONE;

  switch (mode) {
    case Mode::INACTIVE:
      this->prev_mode = Mode::INACTIVE;
      break;
    case Mode::SEQUENCE:
      event = show_sequence(this);
      break;
    case Mode::BACKLOG:
      this->prev_mode = Mode::BACKLOG;

      ImGui::SetNextWindowPos({ (1024-400)/2.0f, (786-500)/2.0f });
      ImGui::SetNextWindowSize({ 400, 500 });
      ImGui::Begin("Backlog");
      {
        if (ImGui::BeginTable("Backlog-Table", 2))
        {
          ImGui::TableSetupColumn("c1", ImGuiTableColumnFlags_WidthStretch, 1);
          ImGui::TableSetupColumn("c2", ImGuiTableColumnFlags_WidthFixed, 50);
          for (auto item : this->flashbacks->backlog) {
            FlashbacksDialog *dialog = this->flashbacks->get_from_id(item);
            assert(dialog && "Invalid dialog ID");

            ImVec4 color = { 1, 1, 1, 1 };
            if (dialog->choice == FlashbacksDialogChoice::WRONG) {
              color = { 1, 0, 0, 1 };
            } 
            if (dialog->choice == FlashbacksDialogChoice::CORRECT) {
              color = { 0, 1, 0, 1 };
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::Text("%s", dialog->text);
            ImGui::PopStyleColor();
            ImGui::TableSetColumnIndex(1);
            ImGui::PushID(item);
            if (ImGui::Button("Retry")) {
              this->begin_sequence(item);
            }
            ImGui::PopID();
          }
          ImGui::EndTable();
        }
      }
      ImGui::End();
      break;
  }

  return event;
}

void Flashbacks::free_sequence(FlashbacksDialogId id) {
    FlashbacksDialog *dialog = get_from_id(id);
    
    while (dialog) {
        FlashbacksDialog *next = get_from_id(dialog->next);
    free((void*)dialog->text);
    free((void*)dialog->answer);
        *dialog = { 0 };
        dialog = next;
    }
}
