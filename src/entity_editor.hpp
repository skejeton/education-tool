///
/// BRIEF: Editor for entities and flashbacks.
///

#ifndef H_CATEDU_ENTITY_EDITOR
#define H_CATEDU_ENTITY_EDITOR

#include "flashbacks.hpp"
#include "scene.hpp"
#include "vector"

struct EntityEditor {
    Flashbacks *flashbacks;
    Entity *entity;
    std::vector<FlashbacksDialogPrototype> prototypes;


    static EntityEditor init(Flashbacks* flashbacks) {
        EntityEditor editor = {};
        editor.flashbacks = flashbacks;
        return editor;
    }
    void show();
    void derive_from(Entity *entity);
    void emplace();
};

#endif
