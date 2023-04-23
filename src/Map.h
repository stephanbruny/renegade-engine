//
// Created by Stephan Bruny on 01.04.23.
//

#ifndef RENEGADE_ENGINE_MAP_H
#define RENEGADE_ENGINE_MAP_H

#include <vector>
#include "Math.h"

using namespace  std;

constexpr double PI_MUL_2 = M_PI * 2;

class Map {
private:
    vector<int> floorData;
    vector<int> ceilingData;
    vector<int> wallsData;
    vector<int> lightMap;
    int width;
    int height;
    int globalLight;
public:
    Map(int width, int height, unsigned char globalLight = 0) {
        this->width = width;
        this->height = height;
        this->globalLight = globalLight;

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

    int getLightAt(int index) {
        return this->lightMap[index];
    }

    void setLight(int index, int value) {
        if (index < 0 || index > this->lightMap.size()) {
            cout << "WARNING: light index invalid - " << to_string(index) << endl;
            return;
        }
        this->lightMap[index] = value;
        calculateLight(index % this->width, index / this->width, value / 128);
    }

    void calculateLight(int x, int y, float value) {
        int i = y * this->width + x;
        float light = value;
        float step = PI_MUL_2 / 16;
        float rad = 0;
        float targetValue = 128 / this->globalLight;
        while(rad < PI_MUL_2) {
            int currentLight = light * 128;
            int distance = 1;
            while (currentLight > this->globalLight) {
                int lx = x + (int)(cos(rad) * distance);
                int ly = y + (int)(sin(rad) * distance);
                int index = ly * this->width + lx;
                if (index < 0 || index > this->lightMap.size()) break;
                if (this->lightMap[index] != this->globalLight && this->lightMap[index] != currentLight) {
                    this->lightMap[index] += currentLight;
                    light = Math::lerp(light, targetValue, 0.2);
                    distance++;
                    continue;
                }
                if (this->wallsData[index] > 0) {
                    this->lightMap[index] = currentLight / 2;
                    break;
                };
                this->lightMap[index] = currentLight;
                light = Math::lerp(light, targetValue, 0.2);
                distance++;
            }
            rad += step;
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
