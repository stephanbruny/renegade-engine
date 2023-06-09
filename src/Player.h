//
// Created by Stephan Bruny on 01.04.23.
//

#ifndef RENEGADE_ENGINE_PLAYER_H
#define RENEGADE_ENGINE_PLAYER_H

#include <vector>
#include <raylib.h>
#include <cmath>
#include "Map.h"

using namespace std;

constexpr float MAX_ROTATION = 360.0f;

class Player {
public:
    float rotation;
    Vector2 position{};
    Vector2 direction{};
    Vector2 plane{};
    int tile_x, tile_y;
    Map* map;
    shared_ptr<vector<int>> walls;
    shared_ptr<vector<int>> lightmap;
    float brightness = 0.0f;

    bool isMoving { false };
    bool isRunning { false };

    double movingTime = 0.0;

    int currentMapWidth, currentMapHeight;

    explicit Player(Map* map) {
        this->map = map;
        this->walls = map->getWalls();
        this->lightmap = map->getLightmap();
        this->currentMapWidth = map->getWidth();
        this->currentMapHeight = map->getHeight();
        this->rotation = 0.0f;
        this->position = Vector2 { 0, 0 };
        this->direction = Vector2 { -1, 0 };
        this->plane = Vector2 { 0, 0.66 };
    }

    void rotate(float amount = 0.1f, float deltaAmount = 0.1f) {
        this->rotation += amount;

        if (this->rotation < 0) this->rotation = MAX_ROTATION;
        if (this->rotation > MAX_ROTATION) this->rotation = 0;

        auto oldDirX = this->direction.x;
        this->direction.x = this->direction.x * cos(-amount) - this->direction.y * sin(-amount);
        this->direction.y = oldDirX * sin(-amount) + this->direction.y * cos(-amount);
        auto oldPlaneX = this->plane.x;
        this->plane.x = this->plane.x * cos(-amount) - this->plane.y * sin(-amount);
        this->plane.y = oldPlaneX * sin(-amount) + this->plane.y * cos(-amount);
    }

    void setIsMoving(bool value) {
        this->isMoving = value;
        if (!value) {
            movingTime = 0;
        }
    }

    void setIsRunning(bool value) {
        this->isRunning = value;
    }

    bool getIsMoving() {
        return this->isMoving;
    }

    void onMove() {
        int index = (int)this->position.y * this->map->getWidth() + (int)this->position.x;
        this->brightness = (float)this->map->getLightAt(index) / 255;
        this->isMoving = true;

        tile_x = (int)this->position.x;
        tile_y = (int)this->position.y;
    }

    void moveForward(float amount = 0.1f, float lookAhead = 0.6f) {
        int px = (int)(this->position.x + this->direction.x * lookAhead);
        int py = (int)(this->position.y + this->direction.y * lookAhead);
        int index = py * this->currentMapHeight + px;
        if ((*this->walls)[index] > 0) return;

        this->position.x += this->direction.x * amount;
        this->position.y += this->direction.y * amount;
        this->onMove();
    }

    void moveBackward(float amount = 0.1f, float lookAhead = 0.6f) {
        int px = (int)(this->position.x - this->direction.x * lookAhead);
        int py = (int)(this->position.y - this->direction.y * lookAhead);
        int index = py * this->currentMapWidth + px;
        if ((*this->walls)[index] > 0) return;

        this->position.x -= this->direction.x * amount;
        this->position.y -= this->direction.y * amount;
        this->onMove();
    }

    void onUpdate(double dt) {
        if (isMoving) {
            movingTime += dt;
        }
    }
};

#endif //RENEGADE_ENGINE_PLAYER_H
