#include "scene.hpp"
#include <cassert>

static EntityId index_to_id(size_t idx) {
  return { idx + 1 };
}


static size_t id_to_index(EntityId id) {
  return id.index - 1;
}


EntityId scene_allocate_entity(Scene *scene) {
  for (size_t i = 0; i < SCENE_ENTITY_BUFFER_SIZE; i++) {
    if (scene->entities_taken[i] == false) {
      scene->entities_taken[i] = true;
      return index_to_id(i);
    }
  }

  return { 0 };
}


Entity *scene_get_entity(Scene *scene, EntityId id) {
  if (id.index == 0) {
    return nullptr;
  }

  assert(id_to_index(id) < SCENE_ENTITY_BUFFER_SIZE && "Out of bounds access");

  return &scene->entities[id_to_index(id)];
}


EntityId scene_summon_entity(Scene *scene, Entity ent) {
  EntityId id = scene_allocate_entity(scene);
  Entity *ent_allocated = scene_get_entity(scene, id);

  if (ent_allocated) {
    *ent_allocated = ent;
  }

  return id;
}


void scene_remove_entity(Scene *scene, EntityId id) {
  if (scene_get_entity(scene, id)) {
    scene->entities_taken[id_to_index(id)] = 0;
  }
}


static void scene_iterator_step_forth(SceneIterator *iterator) {
  while (iterator->scene->entities_taken[iterator->index] == false) {
    iterator->index += 1;
  }
}


SceneIterator scene_iterator_begin(Scene *scene) {
  SceneIterator scene_iterator = {};

  scene_iterator.scene = scene;
  scene_iterator.index = 0;
  
  return scene_iterator;
}


bool scene_iterator_going(SceneIterator *iterator) {
  scene_iterator_step_forth(iterator);
  bool going = iterator->index < SCENE_ENTITY_BUFFER_SIZE;

  if (going) {
    SceneIteratorItem item = {};
    item.id = index_to_id(iterator->index);
    item.entity = scene_get_entity(iterator->scene, item.id);

    iterator->item = item;
  }

  return going;
}


void scene_iterator_next(SceneIterator *iterator) {
  iterator->index++;
}

PlacementRegion entity_placement_region(Entity *ent) {
  return { (int)ent->position.x-1, (int)ent->position.z-1, 2, 2 };
}