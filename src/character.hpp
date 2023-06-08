///
/// BRIEF: Procedural character generator
///

#ifndef H_CATEDU_CHARACTER
#define H_CATEDU_CHARACTER

#include "math.hpp"
#include "boxdraw.hpp"

struct Character {
  Vector3 position;

  void draw(BoxdrawRenderer *renderer, Vector4 color_multiple);
};

#endif