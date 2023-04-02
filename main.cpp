#include <iostream>
#include <raylib.h>
#include <utility>
#include <vector>
#include <cmath>
#include <thread>
#include "config.hpp"
#include "src/Map.h"
#include "src/Player.h"
#include "src/Raycaster.h"
#include "src/Level.h"

#include "src/TestMap.h"

using namespace std;

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;

constexpr int MAP_WIDTH = 30;
constexpr int CELL_SIZE = 8;
constexpr float PI_DIV2 = M_PI / 2;

Texture2D playerHandTexture;

void update(double dt, Player* player) {
    bool isRunning = IsKeyDown(KEY_LEFT_SHIFT);
    double playerSpeed = isRunning ? Config::PLAYER_RUN_SPEED : Config::PLAYER_MOVEMENT_SPEED;

    if (IsKeyDown(KEY_LEFT)) {
        player->rotate(-dt * Config::PLAYER_ROTATION_SPEED);
    }
    if (IsKeyDown(KEY_RIGHT)) {
        player->rotate(dt * Config::PLAYER_ROTATION_SPEED);
    }
    if (IsKeyDown(KEY_UP)) {
        player->moveForward(dt * playerSpeed);
    }
    if (IsKeyDown(KEY_DOWN)) {
        player->moveBackward(dt * playerSpeed);
    }
}

void renderBackground(Texture2D& background) {
    DrawTexturePro(
            background,
            Rectangle { 0, 0, (float)background.width, (float)background.height },
            Rectangle { 0, 0, Config::DISPLAY_WIDTH, Config::DISPLAY_HEIGHT },
            Vector2 { 0, 0, },
            0,
            WHITE
    );
}

void render(Player *player, Raycaster& raycaster, Texture2D& background) {
    ClearBackground(BLACK);
    renderBackground(background);
    raycaster.renderFloor();
    raycaster.renderRaycaster();
    raycaster.drawSprites();

    auto color = Color { 64, 64, 64, 255 };
    color = ColorBrightness(color, player->brightness);
    DrawTexture(playerHandTexture, Config::DISPLAY_WIDTH / 2 + playerHandTexture.width / 2, Config::DISPLAY_HEIGHT - playerHandTexture.height, color);
}

void renderMinimap(Player &player, vector<int> &tilemap, int mapWidth, Vector2 offset = { WINDOW_WIDTH / 2, 0 }) {
    int tileSize = CELL_SIZE;
    Color tileColor = { 0, 255, 0, 128 };
    Color playerColor = { 255, 255, 0, 128 };

    for (int i = 0; i < tilemap.size(); i++) {
        int mx = i % mapWidth;
        int my = i / mapWidth;
        int tx = mx * tileSize + offset.x;
        int ty = my * tileSize + offset.y;
        if (tilemap[i] > 0) {
            DrawRectangle(tx, ty, tileSize, tileSize, tileColor);
        }
    }

    DrawCircle(player.position.x * CELL_SIZE + offset.x, player.position.y * CELL_SIZE + offset.y, 8, playerColor);
}

int main() {
    auto isGameRunning = std::atomic<bool>(true);;
    auto deltaTime = std::atomic<double>(0);
    auto currentTime = std::atomic<double>(0.0);
    auto isUpdateFinished = std::atomic<bool>(false);

    std::cout << "Hello, World!" << std::endl;
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, Config::WINDOW_TITLE.c_str());
    InitAudioDevice();

    SetTargetFPS(60);

    playerHandTexture = LoadTexture("assets/hand.png");

    auto level = Level("assets/maps/dungeon/dungeon-1.json");
    auto level_size = level.getSize();

    auto map = make_unique<Map>(level_size.x, level_size.y);
    auto wallsLayerData = level.getLayerData("walls");
    auto floorLayerData = level.getLayerData("floor");
    auto ceilingLayerData = level.getLayerData("ceiling");
    auto gameObjects = level.getObjects();
    map->setWalls(wallsLayerData);
    map->setFloor(floorLayerData);
    map->setCeiling(ceilingLayerData);
    map->autoLightMap();
    // map->setLightmap(TestData::TILEMAP_LIGHT);

    auto player = make_unique<Player>(map.get());
    player->position = { 20, 20 };
    player->rotation = 180;

    RenderTexture2D canvas = LoadRenderTexture(Config::DISPLAY_WIDTH, Config::DISPLAY_HEIGHT);
    Rectangle canvasSource = { 0, 0, Config::DISPLAY_WIDTH, -Config::DISPLAY_HEIGHT };
    Rectangle canvasDest = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

    Texture2D textures = LoadTexture("assets/textures.png");
    Texture2D background = LoadTexture("assets/backdrop.png");

    Raycaster raycaster(map.get(), player.get(), make_shared<Texture2D>(textures));
    for (auto &obj : gameObjects) {
        raycaster.addObject(obj);
    }

    raycaster.assignLightMap();

    auto music = LoadMusicStream("assets/music/MyVeryOwnDeadShip.ogg");

    auto onUpdate = [&](){
        while (isGameRunning) {
            double now = GetTime();
            deltaTime = now - currentTime;
            if (deltaTime < Config::UPDATE_DELAY) {
                auto delayDelta = (Config::UPDATE_DELAY - deltaTime) * 1000;
                auto delay = std::chrono::milliseconds ((int) delayDelta);
                std::this_thread::sleep_for(delay);
            }
            currentTime = now;
            update(deltaTime, player.get());
        }
        std::cout << "Update thread finished\n";
        isUpdateFinished = true;
    };

    std::thread updateThread(onUpdate);
    updateThread.detach();

    music.looping = true;
    PlayMusicStream(music);

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);
        BeginTextureMode(canvas);
            render(player.get(), raycaster, background);
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(canvas.texture, canvasSource, canvasDest, Vector2 { 0, 0 }, 0, WHITE );
            // renderMinimap(player, tilemap, MAP_WIDTH);
        EndDrawing();
    }

    isGameRunning = false;

    while(!isUpdateFinished) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    UnloadRenderTexture(canvas);
    UnloadTexture(textures);
    UnloadTexture(background);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
