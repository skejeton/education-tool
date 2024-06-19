#include "script.hpp"

Script Script::clone()
{
    Script result = {};

    for (auto &e : iter(nodes))
    {
        result.nodes.push(e);
    }

    return result;
}

void Script::destroy()
{
    nodes.deinit();
}
