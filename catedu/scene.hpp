///
/// BRIEF: The game scene
///

#ifndef H_SCENE_CATEDU
#define H_SCENE_CATEDU
#include "math.hpp"
#include "placement_grid.hpp"
#include "table.hpp"
#define SCENE_ENTITY_BUFFER_SIZE 128

enum class ShapeType {
    CHARACTER,
    BUILDING,
    TREE,
    PAVEMENT
};

struct Shape {
    ShapeType type;
    // FIXME: For testing
    Vector4 color;
    int height;
};

enum EntityInteractionType {
    STATIC,   // Buildings, Trees e.t.c.
    CHARACTER // Interactible with dialogues.
};

struct Entity {
    Vector3 position;
    Shape shape;
    EntityInteractionType interaction_type;
    TableId dialog_id;
};

struct Scene {
    Table<Entity> entities;
};

struct SceneIteratorItem {
    TableId id;
    Entity *entity;
};

typedef TableIterator<Entity> SceneIterator;

Entity *scene_get_entity(Scene *scene, TableId id);
TableId scene_summon_entity(Scene *scene, Entity ent);
void scene_remove_entity(Scene *scene, TableId id);
SceneIterator scene_iterator_begin(Scene *scene);
bool scene_iterator_going(SceneIterator *iterator);
void scene_iterator_next(SceneIterator *iterator);
PlacementRegion entity_placement_region(Entity* ent);
Box3 entity_get_box(Entity *ent);
void entity_render(BoxdrawRenderer *renderer, Entity *entity);
Rect entity_collision_rect(Entity *ent);

#endif
