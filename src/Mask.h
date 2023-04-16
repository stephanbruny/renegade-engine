//
// Created by Stephan Bruny on 16.04.23.
//

#ifndef RENEGADE_ENGINE_MASK_H
#define RENEGADE_ENGINE_MASK_H

#include "Entities.h"
#include "../lib/AStar/AStar.hpp"

class Mask : public Entity {
private:
    unique_ptr<Player>& player;
    unique_ptr<AStar::Generator>& pathGenerator;
    vector<AStar::Vec2i> path;
    Vector2 currentTarget { 0, 0 };
    AStar::Vec2i lastPlayerPosition { 0, 0 };
    int pathIndex { 0 };
    double checkPositionTimer = 5.0;
    float speed = 1.0f;
    bool sleep = false;
public:
    explicit Mask(unique_ptr<Player> &player, unique_ptr<AStar::Generator>& generator)
        : player(player), pathGenerator(generator) {}

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
        path = pathGenerator->findPath(
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

#endif //RENEGADE_ENGINE_MASK_H
