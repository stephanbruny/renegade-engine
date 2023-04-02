//
// Created by Stephan Bruny on 01.04.23.
//

#ifndef RENEGADE_ENGINE_LEVEL_H
#define RENEGADE_ENGINE_LEVEL_H

#include <vector>
#include <string>
#include "../lib/Tileson.h"

using namespace std;

struct GameObject {
    string name;
    string type;
    Vector2 position;
};

class Level {
private:
    std::unique_ptr<tson::Map> map;
public:
    Level(const string &mapPath) {
        tson::Tileson t;
        this->map = t.parse(mapPath);

        if(map->getStatus() != tson::ParseStatus::OK) throw runtime_error("Could not parse map " + mapPath);
    }

    tson::Vector2<int> getSize() {
        return this->map->getSize();
    }

    vector<int> getLayerData(const string &layerName) {
        auto layer = map->getLayer(layerName);
        if (layer == nullptr) throw runtime_error("Could not find layer " + layerName);
        // Fix id offsets
        auto layerData = layer->getData();
        for (auto &id : layerData) {
            id = id -1;
        }

        return (const vector<int> &) layerData;
    }

    vector<GameObject> getObjects() {
        auto layer = map->getLayer("objects");
        if (layer == nullptr) return {};
        auto objects = layer->getObjects();
        vector<GameObject> result;
        result.reserve(objects.size());
        for (auto &obj : objects) {
            auto pos = obj.getPosition();
            result.push_back({
                obj.getName(),
                obj.getType(),
                Vector2 { (float)pos.x, (float)pos.y }
            });
        }
        return result;
    }
};

#endif //RENEGADE_ENGINE_LEVEL_H
