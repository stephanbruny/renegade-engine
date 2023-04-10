//
// Created by Stephan Bruny on 10.04.23.
//

#ifndef RENEGADE_ENGINE_ENTITIES_H
#define RENEGADE_ENGINE_ENTITIES_H

#include <raylib.h>

#include <utility>
#include "Raycaster.h"

class Entity {
private:
    bool is_alive { true };
public:
    Entity() = default;

    int spriteId { -1 };

    Vector2 position { 0, 0 };

    virtual void update(double dt) {};

    void setPosition(Vector2 pos) {
        this->position = pos;
    }

    void setSpriteId(int id) {
        this->spriteId = id;
    }

    bool isAlive() { return this->is_alive; }
};

class Entities {
private:
    vector<Entity> entity_list;
public:
    Entities() = default;

    void add(Entity &e) {
        entity_list.emplace_back(e);
    }

    void update(double dt, Raycaster &raycaster) {
        for (auto &e : entity_list) {
            e.update(dt);
            if (e.spriteId >= 0) {
                raycaster.setSpritePosition(e.spriteId, e.position);
            }
        }
    }
};

#endif //RENEGADE_ENGINE_ENTITIES_H
