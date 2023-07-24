#ifndef HPP_PLAYER_CATEDU
#define HPP_PLAYER_CATEDU

#include "table.hpp"
#include "camera.hpp"

struct Player {
    Camera camera;
    Vector3 camera_velocity;
};


struct PlayerPool {
    Table<Player> players;
};


#endif
