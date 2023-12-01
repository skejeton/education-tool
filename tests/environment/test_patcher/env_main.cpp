#include "catedu/core/patcher/user.hpp"
#include <stdio.h>

struct Player
{
    int position[3];
    char *name;
};

void patch_player_pos(Patcher *patcher, void *player_)
{
    Player *player = (Player *)player_;
    patcher->value("position", &player->position);
}

void patch_player(Patcher *patcher, void *player_)
{
    Player *player = (Player *)player_;

    patcher->value("position", &player->position);
    patcher->string("name", &player->name);
}

int main()
{
    Patcher patcher = {};

    Player example_player = {{1, 2, 3}, "Jeff"};

    Player example_player_2 = {{4, 5, 6}, "Bob"};

    printf("%d %d %d %s\n", example_player.position[0],
           example_player.position[1], example_player.position[2],
           example_player.name);
    printf("%d %d %d %s\n", example_player_2.position[0],
           example_player_2.position[1], example_player_2.position[2],
           example_player_2.name);

    Patch patch = patcher.create_patch(patch_player, &example_player);
    patcher.debug_patch(patch_player, &patch);
    patcher.apply_patch(patch_player, &example_player_2, &patch);

    printf("%d %d %d %s\n", example_player.position[0],
           example_player.position[1], example_player.position[2],
           example_player.name);
    printf("%d %d %d %s\n", example_player_2.position[0],
           example_player_2.position[1], example_player_2.position[2],
           example_player_2.name);
}
