//
// Created by Stephan Bruny on 01.04.23.
//

#ifndef RENEGADE_ENGINE_MAP_H
#define RENEGADE_ENGINE_MAP_H

#include <vector>

using namespace  std;

class Map {
private:
    vector<int> floorData;
    vector<int> ceilingData;
    vector<int> wallsData;
    vector<int> lightMap;
    int width;
    int height;
public:
    Map(int width, int height, unsigned char globalLight = 48) {
        this->width = width;
        this->height = height;

        size_t map_size = width * height;

        this->wallsData = vector<int>(map_size);
        this->floorData = vector<int>(map_size);
        this->ceilingData = vector<int>(map_size);
        this->lightMap = vector<int>(map_size);

        this->wallsData.reserve(width * height);
        std::fill(this->wallsData.begin(), this->wallsData.end(), 0);

        this->floorData.reserve(width * height);
        std::fill(this->floorData.begin(), this->floorData.end(), 0);

        this->ceilingData.reserve(width * height);
        std::fill(this->ceilingData.begin(), this->ceilingData.end(), 0);

        this->lightMap.reserve(width * height);
        std::fill(this->lightMap.begin(), this->lightMap.end(), globalLight);
    }

    shared_ptr<vector<int>> getWalls() {
        return make_shared<vector<int>>(this->wallsData);
    }

    shared_ptr<vector<int>> getLightmap() {
        return make_shared<vector<int>>(this->lightMap);
    }

    shared_ptr<vector<int>> getFloor() {
        return make_shared<vector<int>>(this->floorData);
    }

    shared_ptr<vector<int>> getCeiling() {
        return make_shared<vector<int>>(this->ceilingData);
    }

    void setWalls(vector<int> &data) {
        if (data.size() != this->wallsData.size()) {
            throw runtime_error("Invalid walls data");
        }
        for (int i = 0; i < data.size(); i++) {
            this->wallsData[i] = data[i];
        }
    }

    void setFloor(vector<int> &data) {
        if (data.size() != this->floorData.size()) {
            throw runtime_error("Invalid walls data");
        }
        for (int i = 0; i < data.size(); i++) {
            this->floorData[i] = data[i];
        }
    }

    void setCeiling(vector<int> &data) {
        if (data.size() != this->ceilingData.size()) {
            throw runtime_error("Invalid walls data");
        }
        for (int i = 0; i < data.size(); i++) {
            this->ceilingData[i] = data[i];
        }
    }

    void autoLightMap() {
        for (int i = 0; i < lightMap.size(); i++) {
            if (ceilingData[i] > 0 && wallsData[i] <= 0) {
                lightMap[i] = lightMap[i] / 3;
            }
        }
    }

    void setLightmap(vector<int> &data) {
        if (data.size() != this->lightMap.size()) {
            throw runtime_error("Invalid walls data");
        }
        for (int i = 0; i < data.size(); i++) {
            this->lightMap[i] = data[i];
        }
    }

    [[nodiscard]] int getWidth() const {
        return width;
    }

    [[nodiscard]] int getHeight() const {
        return height;
    }
};

#endif //RENEGADE_ENGINE_MAP_H
