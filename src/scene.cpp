#include "scene.hpp"
#include "character.hpp"
#include <cassert>

Entity *scene_get_entity(Scene *scene, TableId id)
{
  return scene->entities.get(id);
}


TableId scene_summon_entity(Scene *scene, Entity ent)
{
  return scene->entities.allocate(ent);
}


void scene_remove_entity(Scene *scene, TableId id)
{
  assert(scene->entities.remove(id) && "Invalid entity ID");
}


SceneIterator scene_iterator_begin(Scene *scene)
{
  return TableIterator<Entity>::init(&scene->entities);
}


bool scene_iterator_going(SceneIterator* iterator)
{
  return iterator->going();
}

void scene_iterator_next(SceneIterator *iterator)
{
  iterator->next();
}


static Box3 create_box_from_base(float width, float depth, float height, Vector3 position)
{
  return box3_extrude_from_point_volume(position+Vector3{0, height/2, 0}, Vector3{width, height, depth});
}


static Box3 shape_boundaries(Shape shape, Vector3 position)
// TODO: we want to keep this data with each Shape 
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
  Box3 box = shape_boundaries(shape, position);

  float height = box.max.y-box.min.y;

  // TODO: refactor. Each shape should be defined in it's model/data file. 
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
  return shape_boundaries(ent->shape, ent->position);
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


// TODO: also move to entity model files
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
