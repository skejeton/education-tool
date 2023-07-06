#include "pointing_at_resolve.hpp"
#include "assert.h"
#include "math.hpp"

PointingAtResolve PointingAtResolve::init(Ray3 ray) {
    PointingAtResolve result = {};

    result.ray = ray;
    result.dist = 1e10;
    result.id = -1;

    return result;
}

void PointingAtResolve::push_box(Box3 box, int binding_id) {
    float box_distance;
    if (ray3_vs_box3(this->ray, box, 1e10, &box_distance)) {
        if (box_distance < this->dist || this->id == -1) {
            this->dist = box_distance;
            this->id = binding_id;
        }
    }
}

int PointingAtResolve::closest_id() {
    return this->id;
}