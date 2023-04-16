//
// Created by Stephan Bruny on 30.03.23.
//

#ifndef RENEGADE_ENGINE_CONFIG_HPP
#define RENEGADE_ENGINE_CONFIG_HPP

#include <string>
#include <map>
using namespace std;

namespace Config {
    constexpr int DISPLAY_WIDTH = 480;
    constexpr int DISPLAY_HEIGHT = 260;
    constexpr  int TEXTURE_SIZE = 32;
    const string WINDOW_TITLE = string("Renegade Engine");
    constexpr double UPDATE_DELAY = 0.016;

    constexpr double PLAYER_ROTATION_SPEED = 2.0;
    constexpr double PLAYER_MOVEMENT_SPEED = 2.0;
    constexpr double PLAYER_RUN_SPEED = 5.66;

    constexpr int WINDOW_WIDTH = 1280;
    constexpr int WINDOW_HEIGHT = 800;

    map<string, string> TEXTURE_MAP = {
            { "assets/hand.png", "hand" },
            { "assets/textures.png", "textures" },
            { "assets/backdrop.png", "background" },
            { "assets/sprites/mask.png",  "mask" },
            { "assets/sprites/spear-head.png", "spearhead" },
            { "assets/sprites/statue-1.png", "statue" },
            { "assets/sprites/candles.png", "candles" },
            { "assets/sprites/cage.png", "cage" },
            { "assets/sprites/skull-1.png", "skull" },
            { "assets/sprites/hook.png", "hook" },
            { "assets/sprites/barell.png", "barell" },
            { "assets/sprites/mask.png", "mask" },
            { "assets/sprites/tree-1.png", "tree-1" },
            { "assets/sprites/tree-2.png", "tree-2" },
    };
}

#endif //RENEGADE_ENGINE_CONFIG_HPP
