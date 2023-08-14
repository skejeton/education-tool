#ifndef HPP_ENVIRONMENT_CATEDU
#define HPP_ENVIRONMENT_CATEDU

#include "player.hpp"
#include "scene.hpp"
#include "flashbacks.hpp"
#include "dialog.hpp"

enum PlayingMode {
    PLAYING_MODE_BUILD,
    PLAYING_MODE_PLAY,
    PLAYING_MODE_MENU,
};


struct Environment {
    DialogQueue dialogs;
    PlayerPool player_pool;
    Scene scene;
    Flashbacks flashbacks;
    PlayingMode playing_mode;
};


#endif
