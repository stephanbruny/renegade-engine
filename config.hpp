//
// Created by Stephan Bruny on 30.03.23.
//

#ifndef RENEGADE_ENGINE_CONFIG_HPP
#define RENEGADE_ENGINE_CONFIG_HPP

#include <string>
#include <map>
using namespace std;

namespace Config {
    constexpr int DISPLAY_WIDTH = 320;
    constexpr int DISPLAY_HEIGHT = 200;
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
            { "assets/dungeon.png", "dungeon" },
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
            { "assets/sprites/bed.png", "bed" },
            { "assets/sprites/stool.png", "stool" },
            { "assets/sprites/infusion.png", "infusion" },
            { "assets/sprites/infusion2.png", "infusion-2" },
    };

    struct vec2i {
        int x;
        int y;
    };

    static vec2i getWindowSize() {
        return {
            WINDOW_WIDTH,
            WINDOW_HEIGHT
        };
    }
}

#endif //RENEGADE_ENGINE_CONFIG_HPP
