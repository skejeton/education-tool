///
/// BRIEF: Editor for entities and flashbacks.
///

#ifndef H_CATEDU_ENTITY_EDITOR
#define H_CATEDU_ENTITY_EDITOR

#include "easy_gui.hpp"
#include "flashbacks.hpp"
#include "netcode.hpp"
#include "scene.hpp"
#include "vector"

struct EntityEditor
{
    Flashbacks* flashbacks;
    TableId entity_id;
    Entity* entity;
    std::vector<FlashbacksDialogPrototype> prototypes;

    static EntityEditor init(Flashbacks* flashbacks)
    {
        EntityEditor editor = {};
        editor.flashbacks = flashbacks;
        return editor;
    }
    void show(EasyGui* gui);
    void derive_from(TableId entity_id, Entity* entity);
    void emplace(Netcode* nc);
};

#endif
