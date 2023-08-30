#include "entity_editor.hpp"
#include "flashbacks.hpp"
#include "imgui/imgui.h"
#include "memory.h"

#define MAX_LENGTH 1024

FlashbacksDialogPrototype
allocate_prototype()
{
    return FlashbacksDialogPrototype{ (char*)calloc(MAX_LENGTH, 1),
                                      (char*)calloc(MAX_LENGTH, 1) };
}

void
deallocate_prototype(FlashbacksDialogPrototype* prototype)
{
    free(prototype->text);
    free(prototype->answer);
}

void
EntityEditor::show(EasyGui* gui)
{
    int i = 0;
    int delete_index = -1;
    int swap_index_first = -1;
    int swap_index_last = -1;
    gui->margin = 5;
    gui->padding = 10;

    gui->stretch = true;
    for (auto& dialog : this->prototypes) {
        gui->push_id(i);
        gui->input_text_multiline("Description", dialog.text, MAX_LENGTH);
        gui->input_text("Answer", dialog.answer, MAX_LENGTH);
        if (gui->button("Delete")) {
            delete_index = i;
        }
        gui->begin_layout(Layout::ROW);
        {
            gui->stretch = false;
            if (gui->button("^")) {
                swap_index_first = i;
                swap_index_last = i - 1;
            }
            if (gui->button("v")) {
                swap_index_first = i;
                swap_index_last = i + 1;
            }
            gui->checkbox("Numeric Guess", &dialog.numeric);
            gui->stretch = true;
        }
        gui->end_layout();
        gui->pop_id();
        i++;
    }

    if (swap_index_first >= 0 && swap_index_last >= 0 &&
        swap_index_first < prototypes.size() &&
        swap_index_last < prototypes.size()) {
        auto temp = prototypes[swap_index_first];
        prototypes[swap_index_first] = prototypes[swap_index_last];
        prototypes[swap_index_last] = temp;
    }

    if (delete_index >= 0) {
        deallocate_prototype(&this->prototypes[delete_index]);
        this->prototypes.erase(this->prototypes.begin() + delete_index);
    }

    if (gui->button("+")) {
        this->prototypes.push_back(allocate_prototype());
    }
}

void
copy_string_safe_n(char* dest, const char* src, size_t max)
{
    *dest = 0;
    if (!src)
        return;
    for (size_t i = 0; src[i] && i < max - 1; i++) {
        dest[i] = src[i];
    }
    dest[max - 1] = 0;
}

static FlashbacksDialogPrototype
derive_prototype_from(FlashbacksDialog dialog)
{
    FlashbacksDialogPrototype prototype = allocate_prototype();

    copy_string_safe_n(prototype.text, dialog.text, MAX_LENGTH);
    copy_string_safe_n(prototype.answer, dialog.answer, MAX_LENGTH);

    return prototype;
}

void
EntityEditor::derive_from(TableId entity_id, Entity* entity)
{
    prototypes = {};

    this->entity_id = entity_id;
    this->entity = entity;

    FlashbacksDialogId id = entity->dialog_id;
    while (id.id) {
        FlashbacksDialog* dialog = flashbacks->get_from_id(id);
        if (!dialog) {
            break;
        }
        prototypes.push_back(derive_prototype_from(*dialog));
        id = dialog->next;
    }
}

void
EntityEditor::emplace(Netcode* nc)
{
    if (entity == nullptr) {
        return;
    }

    FlashbacksDialogId* start = &entity->dialog_id;

    flashbacks->free_sequence(nc, *start);

    FlashbacksDialogMaker maker = FlashbacksDialogMaker::from(flashbacks);

    for (auto dialog : this->prototypes) {
        if (dialog.answer && *dialog.answer == 0) {
            dialog.answer = nullptr;
        }
        maker.append_dialog(nc, dialog);
    }

    *start = maker.starter_id;
    nc->set_entity(entity_id, *entity);
}
