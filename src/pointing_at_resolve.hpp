///
/// BRIEF: A collision pool for boxes against rays.
/// 

#include "math.hpp"

#ifndef H_POINTING_AT_RESOLVE_CATEDU
#define H_POINTING_AT_RESOLVE_CATEDU

struct PointingAtResolve {
    Ray3 ray;
    double dist;
    int id;

    static PointingAtResolve init(Ray3 ray);

    void push_box(Box3 box, int binding_id);
    int closest_id();
};

#endif