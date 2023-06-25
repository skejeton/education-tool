#include "character.hpp"

#define PART_COUNT 4

void Character::draw(BoxdrawRenderer *renderer, Vector4 color_multiple) {
  Vector4 COLOR_TOP = Vector4{ 0, 0, 1, 1 } * color_multiple;
  Vector4 COLOR_BOTTOM = Vector4{ 1, 0, 1, 1 } * color_multiple;

  // Use the position parameter as base
  Vector3 position = this->position + Vector3{0, 1, 0};

  Box3 parts[PART_COUNT] = {
    box3_extrude_from_point(position, {0.5, 1, 0.5}),
    box3_extrude_from_point(position+Vector3{0, 1.5, 0}, {0.7, 0.7, 0.7}),
    box3_extrude_from_point(position+Vector3{0, 0.4, 0.4}, {0.4, 0.6, 0.4}),
    box3_extrude_from_point(position+Vector3{0, 0.4, -0.4}, {0.4, 0.6, 0.4}),
  };


  for (size_t i = 0; i < PART_COUNT; i++) {
    boxdraw_push(renderer, boxdraw_cmdgradient(parts[i], COLOR_TOP, COLOR_BOTTOM));
  }
}
