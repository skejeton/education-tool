///
/// BRIEF: Procedural character generator
///

#ifndef H_CATEDU_CHARACTER
#define H_CATEDU_CHARACTER

#include "boxdraw.hpp"
#include "math.hpp"

struct Character
{
    Vector3 position;

    void draw(BoxdrawRenderer* renderer, Vector4 color_multiple);
};

#endif
