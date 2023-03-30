#include <iostream>
#include <raylib.h>
#include <vector>
#include "config.hpp"

using namespace std;

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;

struct Wall {
    Vector2 pointA;
    Vector2 pointB;
    int textureID;
};

struct Sector {
    float bottom;
    float top;
    int bottomTextureID;
    int topTextureID;
    vector<Wall> walls;
};

int main() {
    std::cout << "Hello, World!" << std::endl;
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, Config::WINDOW_TITLE.c_str());

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
