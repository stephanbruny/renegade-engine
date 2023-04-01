//
// Created by Stephan Bruny on 30.03.23.
//

#ifndef RENEGADE_ENGINE_CONFIG_HPP
#define RENEGADE_ENGINE_CONFIG_HPP

#include <string>
using namespace std;

namespace Config {
    constexpr int DISPLAY_WIDTH = 480;
    constexpr int DISPLAY_HEIGHT = 260;
    constexpr  int TEXTURE_SIZE = 32;
    const string WINDOW_TITLE = string("Renegade Engine");
    constexpr double UPDATE_DELAY = 0.016;
}

#endif //RENEGADE_ENGINE_CONFIG_HPP
