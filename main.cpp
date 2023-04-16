#include <iostream>
#include <raylib.h>
#include <vector>
#include <cmath>
#include <thread>
#include "config.hpp"
#include "src/Map.h"
#include "src/Player.h"
#include "src/Raycaster.h"
#include "src/Level.h"
#include "src/Textures.h"
#include "src/Entities.h"
#include "src/Mask.h"

#include "lib/AStar/AStar.hpp"

using namespace std;

void update(double dt, Player* player) {
    bool isRunning = IsKeyDown(KEY_LEFT_SHIFT);
    double playerSpeed = isRunning ? Config::PLAYER_RUN_SPEED : Config::PLAYER_MOVEMENT_SPEED;

    bool isMoving = false;
    if (IsKeyDown(KEY_LEFT)) {
        player->rotate(-dt * Config::PLAYER_ROTATION_SPEED);
    }
    if (IsKeyDown(KEY_RIGHT)) {
        player->rotate(dt * Config::PLAYER_ROTATION_SPEED);
    }
    if (IsKeyDown(KEY_UP)) {
        player->moveForward(dt * playerSpeed);
        isMoving = true;
    }
    if (IsKeyDown(KEY_DOWN)) {
        player->moveBackward(dt * playerSpeed);
        isMoving = true;
    }
    player->setIsMoving(isMoving);
    player->setIsRunning(isRunning);

    player->onUpdate(dt);
}

void renderBackground(shared_ptr<Texture2D> background) {
    DrawTexturePro(
            *background,
            Rectangle { 0, 0, (float)background->width, (float)background->height },
            Rectangle { 0, 0, Config::DISPLAY_WIDTH, Config::DISPLAY_HEIGHT },
            Vector2 { 0, 0, },
            0,
            WHITE
    );
}

void renderHand(Player * player, unique_ptr<Textures> &textures) {
    auto handTexture = textures->get("hand");
    auto color = Color { 64, 64, 64, 255 };
    color = ColorBrightness(color, player->brightness);
    float mod = player->isRunning ? 16.0f : 8.0f;
    float offsetY = sin(player->movingTime * mod) * 8.0f;
    DrawTexture(*handTexture, Config::DISPLAY_WIDTH / 2 + handTexture->width / 2, Config::DISPLAY_HEIGHT - handTexture->height + offsetY + 16, color);
}

void render(Player *player, Raycaster& raycaster, unique_ptr<Textures> &textures) {
    // ClearBackground(BLACK);
    renderBackground(textures->get("background"));
    raycaster.renderFloor();
    raycaster.renderRaycaster();
    raycaster.drawSprites();

    renderHand(player, textures);
}

int main() {
    auto isGameRunning = std::atomic<bool>(true);;
    auto deltaTime = std::atomic<double>(0);
    auto currentTime = std::atomic<double>(0.0);
    auto isUpdateFinished = std::atomic<bool>(false);

    InitWindow(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, Config::WINDOW_TITLE.c_str());
    InitAudioDevice();

    auto textures = make_unique<Textures>();

    for (auto &tex : Config::TEXTURE_MAP) {
        textures->add(tex.first, tex.second);
    }

    SetTargetFPS(60);

    auto level = Level("assets/maps/dungeon/dungeon-1.json");
    auto level_size = level.getSize();

    auto pathGenerator = make_unique<AStar::Generator>();

    pathGenerator->setWorldSize({ level_size.x, level_size.y });

    auto map = make_unique<Map>(level_size.x, level_size.y, 16);
    auto wallsLayerData = level.getLayerData("walls");
    auto floorLayerData = level.getLayerData("floor");
    auto ceilingLayerData = level.getLayerData("ceiling");
    auto gameObjects = level.getObjects();
    map->setWalls(wallsLayerData);
    map->setFloor(floorLayerData);
    map->setCeiling(ceilingLayerData);
    map->autoLightMap();

    pathGenerator->setDiagonalMovement(true);
    for (int i = 0; i < wallsLayerData.size(); i++) {
        if (wallsLayerData[i] > 0) {
            int x = i % level_size.x;
            int y = i / level_size.x;
            pathGenerator->addCollision({ x, y });
        }
    }

    auto player = make_unique<Player>(map.get());
    player->position = { 20, 20 };
    player->rotation = 180;

    RenderTexture2D canvas = LoadRenderTexture(Config::DISPLAY_WIDTH, Config::DISPLAY_HEIGHT);
    Rectangle canvasSource = { 0, 0, Config::DISPLAY_WIDTH, -Config::DISPLAY_HEIGHT };
    Rectangle canvasDest = { 0, 0, Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT };

    Raycaster raycaster(map.get(), player.get(), textures);
    raycaster.setAtlas("textures");

    for (auto &obj : gameObjects) {
        raycaster.addObject(obj);
    }

    raycaster.assignLightMap();

    auto music = LoadMusicStream("assets/music/MyVeryOwnDeadShip.ogg");

    Entities entities;
    auto maskTexture = textures->get("mask");
    auto maskSpriteId = raycaster.addSprite(Sprite({0, 0}, *maskTexture));
    Mask mask(player, pathGenerator);
    mask.setSpriteId(maskSpriteId);
    entities.add(mask);
    mask.reset();

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
            entities.update(deltaTime, raycaster);
            update(deltaTime, player.get());
            mask.update(deltaTime);
            raycaster.setSpritePosition(mask.spriteId, mask.position);
        }
        std::cout << "Update thread finished\n";
        isUpdateFinished = true;
    };

    std::thread updateThread(onUpdate);
    updateThread.detach();

    music.looping = true;
    PlayMusicStream(music);

    char* txt;

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);
        BeginTextureMode(canvas);
            render(player.get(), raycaster, textures);
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(canvas.texture, canvasSource, canvasDest, Vector2 { 0, 0 }, 0, WHITE );
            DrawFPS(4, 4);
            DrawText(TextFormat("P: %i:%i", player->tile_x, player->tile_y), 4, 24, 12, WHITE);
        DrawText(TextFormat("M: %f:%f", mask.position.x, mask.position.y), 4, 38, 12, WHITE);
        EndDrawing();
    }

    isGameRunning = false;

    while(!isUpdateFinished) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    UnloadRenderTexture(canvas);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
