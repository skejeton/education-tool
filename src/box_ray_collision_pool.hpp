///
/// BRIEF: A collision pool for boxes against rays.
/// 

#include "math.hpp"

#ifndef H_BOX_RAY_COLLISION_POOL_CATEDU
#define H_BOX_RAY_COLLISION_POOL_CATEDU

#define BOX_RAY_COLLISION_POOL_ITEMS_COUNT 32

struct BoxRayCollisionPool {
  struct Item {
    bool taken;
    Box3 box;
  } items[BOX_RAY_COLLISION_POOL_ITEMS_COUNT];

  int push_box(Box3 box);
  int check_ray(Ray3 ray);
};

#endif