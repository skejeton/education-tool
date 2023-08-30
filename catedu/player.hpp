#ifndef HPP_PLAYER_CATEDU
#define HPP_PLAYER_CATEDU

#include "camera.hpp"
#include "table.hpp"

struct Player
{
    Camera camera;
    Vector3 camera_velocity;
};

struct PlayerPool
{
    Table<Player> players;
};

#endif
