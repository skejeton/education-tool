#include "box_ray_collision_pool.hpp"
#include "assert.h"

static int allocate(BoxRayCollisionPool *pool) {
  for (size_t i = 0; i < BOX_RAY_COLLISION_POOL_ITEMS_COUNT; i++) {
    if (!pool->items[i].taken) {
      pool->items[i].taken = true;
      return i; 
    }
  }


  return -1;
}

int BoxRayCollisionPool::push_box(Box3 box) {
  int id = allocate(this);
  if (id == -1) {
    assert(false && "failed to allocate (internal error)");
    return -1;
  }

  this->items[id].box = box;
  return id;
}

int BoxRayCollisionPool::check_ray(Ray3 ray) {
  float min_distance = 1e9;
  int min_item = -1;
  for (size_t i = 0; i < BOX_RAY_COLLISION_POOL_ITEMS_COUNT; i++) {
    float distance;
    if (this->items[i].taken) {
      if (ray3_vs_box3(ray, this->items[i].box, min_distance, &distance)) {
        min_distance = distance;
        min_item = i;
      }
    }
  }

  return min_distance;
}
