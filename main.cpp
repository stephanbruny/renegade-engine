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
#include "src/Textures.h"
#include "src/Entities.h"

#include <cmath>

#include "lib/AStar/AStar.hpp"

#include "src/TestMap.h"

using namespace std;

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;

constexpr int MAP_WIDTH = 30;
constexpr int CELL_SIZE = 8;
constexpr float PI_DIV2 = M_PI / 2;

AStar::Generator pathGenerator;

class Mask : public Entity {
private:
    unique_ptr<Player>& player;
    vector<AStar::Vec2i> path;
    Vector2 currentTarget { 0, 0 };
    AStar::Vec2i lastPlayerPosition { 0, 0 };
    int pathIndex { 0 };
    double checkPositionTimer = 5.0;
    float speed = 1.0f;
    bool sleep = false;
public:
    explicit Mask(unique_ptr<Player> &player) : player(player) {}

    void reset() {
        int x = GetRandomValue(0, 40);
        int y = GetRandomValue(0, 40);
        this->position.x = (float)x;
        this->position.y = (float)y;
        this->calculatePath();
    }

    void calculatePath() {
        lastPlayerPosition = { (int)player->position.x, (int)player->position.y };
        AStar::Vec2i source = { (int)this->position.x, (int)this->position.y };
        path = pathGenerator.findPath(
            { (int)this->position.x, (int)this->position.y },
            lastPlayerPosition
        );
        if (path.size() <= 1) {
            this->reset();
            return;
        }
        reverse(path.begin(), path.end());
        pathIndex = 0;
        currentTarget = Vector2 {
                (float)path[pathIndex].x + 0.5f,
                (float)path[pathIndex].y + 0.5f
        };
    }

    void nextTarget(int index) {
        currentTarget = Vector2 {
                (float)path[index].x + 0.5f,
                (float)path[index].y + 0.5f
        };
    }

    float lerp(float a, float b, float f) {
        return a * (1.0 - f) + (b * f);
    }

    void update(double dt) override {
        if (sleep) return;
        checkPositionTimer -= dt;
        if (checkPositionTimer <= 0) {
            this->path.clear();
            checkPositionTimer = 10.0;
        }
        if (this->path.empty()) calculatePath();
        this->position = {
                lerp(this->position.x, (float)currentTarget.x, (float)dt * speed),
                lerp(this->position.y, (float)currentTarget.y, (float)dt * speed),
        };
        float px, py, tx, ty;
        px = position.x;
        py = position.y;
        tx = currentTarget.x;
        ty = currentTarget.y;
        if (std::abs(px-tx)<0.1 && std::abs(py-ty)<0.1) {
            this->pathIndex++;
            if (pathIndex > path.size()) return;
            nextTarget(pathIndex);
        }
    }
};

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

void renderHand(Player * player, Textures &textures) {
    auto handTexture = textures.get("hand");
    auto color = Color { 64, 64, 64, 255 };
    color = ColorBrightness(color, player->brightness);
    float mod = player->isRunning ? 16.0f : 8.0f;
    float offsetY = sin(player->movingTime * mod) * 8.0f;
    DrawTexture(*handTexture, Config::DISPLAY_WIDTH / 2 + handTexture->width / 2, Config::DISPLAY_HEIGHT - handTexture->height + offsetY + 16, color);
}

void render(Player *player, Raycaster& raycaster, Textures &textures) {
    // ClearBackground(BLACK);
    renderBackground(textures.get("background"));
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

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, Config::WINDOW_TITLE.c_str());
    InitAudioDevice();

    Textures textures;

    textures.add("assets/hand.png", "hand");
    textures.add("assets/textures.png", "textures");
    textures.add("assets/backdrop.png", "background");
    textures.add("assets/sprites/mask.png", "mask");

    SetTargetFPS(60);

    auto level = Level("assets/maps/dungeon/dungeon-1.json");
    auto level_size = level.getSize();

    pathGenerator.setWorldSize({ level_size.x, level_size.y });

    auto map = make_unique<Map>(level_size.x, level_size.y);
    auto wallsLayerData = level.getLayerData("walls");
    auto floorLayerData = level.getLayerData("floor");
    auto ceilingLayerData = level.getLayerData("ceiling");
    auto gameObjects = level.getObjects();
    map->setWalls(wallsLayerData);
    map->setFloor(floorLayerData);
    map->setCeiling(ceilingLayerData);
    map->autoLightMap();

    pathGenerator.setDiagonalMovement(true);
    for (int i = 0; i < wallsLayerData.size(); i++) {
        if (wallsLayerData[i] > 0) {
            int x = i % level_size.x;
            int y = i / level_size.x;
            pathGenerator.addCollision({ x, y });
        }
    }

    auto player = make_unique<Player>(map.get());
    player->position = { 20, 20 };
    player->rotation = 180;

    RenderTexture2D canvas = LoadRenderTexture(Config::DISPLAY_WIDTH, Config::DISPLAY_HEIGHT);
    Rectangle canvasSource = { 0, 0, Config::DISPLAY_WIDTH, -Config::DISPLAY_HEIGHT };
    Rectangle canvasDest = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

    Raycaster raycaster(map.get(), player.get(), textures.get("textures"));
    for (auto &obj : gameObjects) {
        raycaster.addObject(obj);
    }

    raycaster.assignLightMap();

    auto music = LoadMusicStream("assets/music/MyVeryOwnDeadShip.ogg");

    Entities entities;
    auto maskTexture = textures.get("mask");
    auto maskSpriteId = raycaster.addSprite(Sprite({0, 0}, *maskTexture));
    Mask mask(player);
    mask.setPosition({ 14.5f, 20.5f });
    mask.setSpriteId(maskSpriteId);
    entities.add(mask);

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
