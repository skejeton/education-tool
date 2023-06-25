///
/// BRIEF: The game scene
///

#ifndef H_SCENE_CATEDU
#define H_SCENE_CATEDU
#include "math.hpp"
#include "placement_grid.hpp"
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
  char objective_complete[128];
  size_t dialog_stages_id[9];
};

/// NOTE: EntityId of 0 indicates a NULL entry
struct EntityId {
  size_t index;
};

struct Scene {
  Entity entities[SCENE_ENTITY_BUFFER_SIZE];
  bool entities_taken[SCENE_ENTITY_BUFFER_SIZE];
};

struct SceneIteratorItem {
  EntityId id;
  Entity *entity;
};

struct SceneIterator {
  Scene *scene;
  size_t index;

  SceneIteratorItem item;
};

Entity *scene_get_entity(Scene *scene, EntityId id);
EntityId scene_summon_entity(Scene *scene, Entity ent);
void scene_remove_entity(Scene *scene, EntityId id);
SceneIterator scene_iterator_begin(Scene *scene);
bool scene_iterator_going(SceneIterator *iterator);
void scene_iterator_next(SceneIterator *iterator);
PlacementRegion entity_placement_region(Entity* ent);
Box3 entity_get_box(Entity *ent);
void entity_render(BoxdrawRenderer *renderer, Entity *entity);
Rect entity_collision_rect(Entity *ent);

#endif
