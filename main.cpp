#include <iostream>
#include <raylib.h>
#include <vector>
#include <cmath>
#include "config.hpp"

using namespace std;

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;

constexpr int MAP_WIDTH = 30;
constexpr int CELL_SIZE = 8;
constexpr float PI_DIV2 = M_PI / 2;
constexpr float PI_3 = 3*PI_DIV2;
constexpr float PI_2 = 2*M_PI;

constexpr float ONE_DEG = 0.0174533f;


constexpr float MAX_ROTATION = 360.0f;

inline float degreeToRadians(float degree) {
    return degree * (float)M_PI / 180;
}

vector<int> tilemap_floor = {
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
};

vector<int> tilemap = {
    8, 8, 2, 3, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

struct Player {
    float rotation;
    Vector2 position;
    Vector2 delta;
    Vector2 direction;
    Vector2 plane;
    shared_ptr<vector<int>> map;

    Player(shared_ptr<vector<int>> map) {
        this->map = map;
        this->rotation = 0.0f;
        this->position = Vector2 { 0, 0 };
        this->direction = Vector2 { -1, 0 };
        this->plane = Vector2 { 0, 0.66 };
        this->delta = Vector2 { 0, 0 };
    }

    void rotate(float amount = 0.1f, float deltaAmount = 0.1f) {
        this->rotation += amount;

        if (this->rotation < 0) this->rotation = MAX_ROTATION;
        if (this->rotation > MAX_ROTATION) this->rotation = 0;

        this->delta.x = cos(degreeToRadians(this->rotation)) * deltaAmount;
        this->delta.y = sin(degreeToRadians(this->rotation)) * deltaAmount;

        auto oldDirX = this->direction.x;
        this->direction.x = this->direction.x * cos(-amount) - this->direction.y * sin(-amount);
        this->direction.y = oldDirX * sin(-amount) + this->direction.y * cos(-amount);
        auto oldPlaneX = this->plane.x;
        this->plane.x = this->plane.x * cos(-amount) - this->plane.y * sin(-amount);
        this->plane.y = oldPlaneX * sin(-amount) + this->plane.y * cos(-amount);
    }

    void moveForward(float amount = 0.1f) {
        int px = (this->position.x + this->direction.x * 1.0);
        int py = (this->position.y + this->direction.y * 1.0);
        int index = py * MAP_WIDTH + px;
        if ((*this->map)[index] > 0) return;

        this->position.x += this->direction.x * amount;
        this->position.y += this->direction.y * amount;
    }

    void moveBackward(float amount = 0.1f) {
        int px = (this->position.x - this->direction.x * 1.0);
        int py = (this->position.y - this->direction.y * 1.0);
        int index = py * MAP_WIDTH + px;
        if ((*this->map)[index] > 0) return;

        this->position.x -= this->direction.x * amount;
        this->position.y -= this->direction.y * amount;
    }
};

float getDistance(Vector2 a, Vector2 b, float ang) {
    return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

void renderRaycaster(Player &player, vector<int> &map, int mapWidth, Texture2D &textures) {
    for (int x = 0; x < Config::DISPLAY_WIDTH; x++) {
        double cameraX = 2 * x / double(Config::DISPLAY_WIDTH) - 1; //x-coordinate in camera space
        double rayDirX = player.direction.x + player.plane.x * cameraX;
        double rayDirY = player.direction.y + player.plane.y * cameraX;

        //which box of the map we're in
        int mapX = int(player.position.x);
        int mapY = int(player.position.y);

        //length of ray from current position to next x or y-side
        double sideDistX;
        double sideDistY;

        //length of ray from one x or y-side to next x or y-side
        double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
        double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);
        double perpWallDist;

        //what direction to step in x or y-direction (either +1 or -1)
        int stepX;
        int stepY;

        int hit = 0; //was there a wall hit?
        int side; //was a NS or a EW wall hit?

        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (player.position.x - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - player.position.x) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (player.position.y - mapY) * deltaDistY;
        } else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - player.position.y) * deltaDistY;
        }

        int rayDepth = 0;
        int wallTextureId = -1;
        while (rayDepth < 10000)
        {
            //jump to next map square, either in x-direction, or in y-direction
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            int mapIndex = mapY * mapWidth + mapX;
            //Check if ray has hit a wall
            if (mapIndex >= 0 && mapIndex < map.size() && map[mapIndex] > 0) {
                wallTextureId = map[mapIndex];
                break;
            }
            rayDepth++;
        }

        //calculate value of wallX
        double wallX; //where exactly the wall was hit
        if (side == 0) wallX = player.position.y + perpWallDist * rayDirY;
        else           wallX = player.position.x + perpWallDist * rayDirX;
        wallX -= floor((wallX));

        //x coordinate on the texture
        int texX = int(wallX * double(Config::TEXTURE_SIZE));
        if(side == 0 && rayDirX > 0) texX = Config::TEXTURE_SIZE - texX - 1;
        if(side == 1 && rayDirY < 0) texX = Config::TEXTURE_SIZE - texX - 1;

        if(side == 0)
            perpWallDist = (sideDistX - deltaDistX);
        else
            perpWallDist = (sideDistY - deltaDistY);

        //Calculate height of line to draw on screen
        int lineHeight = (int)(Config::DISPLAY_HEIGHT / perpWallDist);

        //calculate lowest and highest pixel to fill in current stripe
        double wallLightDist = perpWallDist;
        if (wallLightDist < 1) wallLightDist = 1;
        unsigned char wallDepth = (1 / wallLightDist) * ((side == 1) ? 128 : 255);
        int drawStart = -lineHeight / 2 + Config::DISPLAY_HEIGHT / 2;
        // if(drawStart < 0)drawStart = 0;
        int drawEnd = lineHeight / 2 + Config::DISPLAY_HEIGHT / 2;
        // if(drawEnd >= Config::DISPLAY_HEIGHT)drawEnd = Config::DISPLAY_HEIGHT - 1;

        Color color = { wallDepth, wallDepth, wallDepth, 255 };
        // if (side == 1) color = GRAY;

        // DrawLine(x, drawStart, x, drawEnd, color);
        int atlasWidth = textures.width / Config::TEXTURE_SIZE;
        int textureIdX = wallTextureId % atlasWidth;
        int textureIdY = wallTextureId / atlasWidth;
        float textureX = textureIdX * Config::TEXTURE_SIZE + texX;
        float textureY = textureIdY * Config::TEXTURE_SIZE;
        DrawTexturePro(
                textures,
                Rectangle { textureX, textureY, 1, Config::TEXTURE_SIZE },
                Rectangle { (float)x, (float)drawStart, 1, float(drawEnd - drawStart) },
                Vector2 { 0, 0 },
                0.0,
                color
        );
    }
}

void update(double dt, Player& player) {
    if (IsKeyDown(KEY_LEFT)) {
        player.rotate(-0.10f);
    }
    if (IsKeyDown(KEY_RIGHT)) {
        player.rotate(0.10f);
    }
    if (IsKeyDown(KEY_UP)) {
        player.moveForward();
    }
    if (IsKeyDown(KEY_DOWN)) {
        player.moveBackward();
    }
}

void render(Player &player, Texture2D &textures) {
    ClearBackground(BLACK);
    renderRaycaster(player, tilemap, MAP_WIDTH, textures);
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
    std::cout << "Hello, World!" << std::endl;
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, Config::WINDOW_TITLE.c_str());

    SetTargetFPS(60);

    double currentTime = 0.0;
    auto shared_map = make_shared<vector<int>>(tilemap);

    Player player(shared_map);
    player.position = { 5, 5 };
    player.rotation = 180;

    RenderTexture2D canvas = LoadRenderTexture(Config::DISPLAY_WIDTH, Config::DISPLAY_HEIGHT);
    Rectangle canvasSource = { 0, 0, Config::DISPLAY_WIDTH, Config::DISPLAY_HEIGHT };
    Rectangle canvasDest = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

    Texture2D textures = LoadTexture("assets/textures.png");

    while (!WindowShouldClose())
    {
        double time = GetTime();
        update(time - currentTime, player);
        currentTime = time;

        BeginTextureMode(canvas);
            render(player, textures);
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(canvas.texture, canvasSource, canvasDest, Vector2 { 0, 0 }, 0, WHITE );
            renderMinimap(player, tilemap, MAP_WIDTH);
        EndDrawing();
    }

    UnloadRenderTexture(canvas);
    UnloadTexture(textures);

    CloseWindow();

    return 0;
}
