#include "player.hpp"
#include "construct.hpp"

GeneratedObject genmesh_generate_player()
{
    GeneratedObject player = {};

    Box3 player_box = construct_box(Baseline::Bottom, {1, 1.8, 1});

    player.push_colored_box(player_box, Color::hex(0xFF7777FF), true);

    return player;
}
