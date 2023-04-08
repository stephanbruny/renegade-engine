#ifndef RENEGADE_ENGINE_TEXTURES_H
#define RENEGADE_ENGINE_TEXTURES_H

#include <raylib.h>
#include <map>
#include <string>

using namespace std;

class Textures {
private:
    map<string, Texture2D> texture_map;

public:
    Textures() = default;

    void add(string path, string name) {
        if (texture_map.count(name)) return;
        texture_map.insert(pair<string, Texture2D>( name, LoadTexture(path.c_str()) ));
    }

    shared_ptr<Texture2D> get(string name) {
        if (!texture_map.count(name)) {
            string error = "Could not find texture: " + name;
            throw runtime_error(error.c_str());
        }
        return make_shared<Texture2D>(texture_map[name]);
    }

    void remove(string name) {
        if (!texture_map.count(name)) return;
        auto texture = texture_map[name];
        UnloadTexture(texture);
        texture_map.erase(name);
    }

    ~Textures() {
        for (auto &pair : texture_map) {
            auto texture = pair.second;
            UnloadTexture(texture);
        }
        texture_map.clear();
    }
};

#endif //RENEGADE_ENGINE_TEXTURES_H
