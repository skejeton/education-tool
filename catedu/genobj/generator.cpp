#include "generator.hpp"
#include <cassert>

void GeneratedObject::push(GeneratedObjectComponent component)
{
    assert(num_components < COMPONENT_COUNT);
    components[num_components++] = component;
}

void GeneratedObject::push_colored_box(Box3 box, Color color)
{
    GeneratedObjectComponent component = {};
    component.box = box;
    component.color = color;

    push(component);
}
