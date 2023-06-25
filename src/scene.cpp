#include "scene.hpp"
#include "character.hpp"
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

static Box3 create_box_from_base(float width, float depth, float height, Vector3 position)
{
  return box3_extrude_from_point_volume(position+Vector3{0, height/2, 0}, Vector3{width, height, depth});
}

static Box3 shape_get_box(Shape shape, Vector3 position)
{
  float width, depth, height;

  switch (shape.type) {
    case ShapeType::BUILDING:
    case ShapeType::TREE:
      width = 8;
      depth = 8;
      height = shape.height * 2 + 2;
      break;
    case ShapeType::PAVEMENT:
      width = 4;
      depth = 4;
      height = 0.2;
      break;
    case ShapeType::CHARACTER:
    default:
      width = 2;
      depth = 2;
      height = 3;
      break;
  }

  return create_box_from_base(width, depth, height, position);
}

static void render_shape_at(Shape shape, Vector3 position, BoxdrawRenderer *renderer)
{
  Box3 box = shape_get_box(shape, position);

  float height = box.max.y-box.min.y;

  switch (shape.type) {
  case ShapeType::BUILDING:
    boxdraw_push(renderer, boxdraw_cmdgradient(box, { 0.5, 0.5, 0.5, 1.0 }, { 0.4, 0.3, 0.3, 1.0 }));
    for (int t = 0; t < shape.height; t++) {
      for (int x = -1; x <= 1; x++) {
        float y = t * 2 + 2;
        Box3 window = box3_extrude_from_point({ position.x+x*2.0f, y, position.z }, { 0.5, 0.75, 4.05 });
        boxdraw_push(renderer, boxdraw_cmdgradient(window, { 0.0, 0.0, 1.0, 1.0 }, { 1.0, 0.0, 0.1, 1.0 }));
      }
      for (int x = -1; x <= 1; x++) {
        float y = t * 2 + 2;
        Box3 window = box3_extrude_from_point({ position.x, y, position.z+x*2.0f }, { 4.05, 0.75, 0.5 });
        boxdraw_push(renderer, boxdraw_cmdgradient(window, { 0.0, 0.0, 1.0, 1.0 }, { 1.0, 0.0, 0.1, 1.0 }));
      }
    }
    break;
  case ShapeType::TREE:
    boxdraw_push(renderer, boxdraw_cmdgradient(box3_extrude_from_point({position.x, height/2, position.z}, {0.5, height/2, 0.5}), { 0.5, 0.2, 0.0, 1.0 }, { 0.6, 0.1, 0.0, 1.0 }));
    boxdraw_push(renderer, boxdraw_cmdgradient(box3_extrude_from_point({position.x, height+1, position.z}, {4, 2, 4}), { 0.0, 0.5, 0.2, 1.0 }, { 0.0, 0.6, 0.1, 1.0 }));
    break;
  case ShapeType::PAVEMENT:
    boxdraw_push(renderer, boxdraw_cmdgradient(box, { 0.5, 0.5, 0.5, 1.0 }, { 0.5, 0.5, 0.5, 1.0 }));
    break;
  case ShapeType::CHARACTER:
    Character{position}.draw(renderer, shape.color);
  }
}

void entity_render(BoxdrawRenderer *renderer, Entity *entity)
{
  render_shape_at(entity->shape, entity->position, renderer);
}

Box3 entity_get_box(Entity *ent)
{
  return shape_get_box(ent->shape, ent->position);
}

PlacementRegion entity_placement_region(Entity *ent)
{
  int x = ent->position.x;
  int z = ent->position.z;

  switch (ent->shape.type) {
    case ShapeType::BUILDING:
    case ShapeType::TREE:
      return {x-4, z-4, 8, 8};
    case ShapeType::PAVEMENT:
      return {x-2, z-2, 4, 4};
    case ShapeType::CHARACTER:
    default:
      return { x-1, z-1, 2, 2 };
  }
}

Rect entity_collision_rect(Entity *ent)
{
  float x = ent->position.x;
  float z = ent->position.z;

  switch (ent->shape.type) {
    case ShapeType::BUILDING:
      return {x-4, z-4, 8, 8};
    case ShapeType::TREE:
      return {x-1, z-1, 2, 2};
    case ShapeType::PAVEMENT:
      return {};
    case ShapeType::CHARACTER:
    default:
      return { x-1, z-1, 2, 2 };
  }
}