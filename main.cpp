#include <iostream>
#include <raylib.h>
#include <vector>
#include <cmath>
#include "config.hpp"

using namespace std;

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;

constexpr int MAP_WIDTH = 10;
constexpr int CELL_SIZE = 64;
constexpr float PI_DIV2 = M_PI / 2;
constexpr float PI_3 = 3*PI_DIV2;

constexpr float ONE_DEG = 0.0174533f;


constexpr float MAX_ROTATION = 2.0f * M_PI;

vector<int> tilemap = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

struct Player {
    double rotation;
    Vector2 position;
    Vector2 delta;

    void rotate(float amount = 0.1f, float deltaAmount = 5.0f) {
        this->rotation += amount;

        if (this->rotation < 0) this->rotation = MAX_ROTATION;
        if (this->rotation > MAX_ROTATION) this->rotation = 0;

        this->delta.x = cos(this->rotation) * deltaAmount;
        this->delta.y = sin(this->rotation) * deltaAmount;
    }

    void moveForward() {
        this->position.x += this->delta.x;
        this->position.y += this->delta.y;
    }

    void moveBackward() {
        this->position.x -= this->delta.x;
        this->position.y -= this->delta.y;
    }
};

float getDistance(Vector2 a, Vector2 b, float ang) {
    return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

void castRays(Player &player, vector<int> map, int maxDistance = 8, int mapWidth = 10, int fov = 60) {
    int rayX, rayY, offsetX, offsetY, mapX, mapY, mapIndex;
    int distance = 0;
    int rayDistance = 0;
    float rayAngle = player.rotation - (fov / 2) * ONE_DEG;
    int raysCount = fov;
    for (int i = 0; i < raysCount; i++) {
        float distHorizontal = 100000.0f;
        Vector2 horizontalPosition = { player.position.x, player.position.y };
        float aTan = -1 / tan(rayAngle);
        bool isUpwards = rayAngle > M_PI;

        if (isUpwards) {
            rayY = ((((int)player.position.y) >> 6) << 6) - 0.0001;
            rayX = (((int)player.position.y) - rayY) * aTan + (int)player.position.x;
            offsetY = -CELL_SIZE;
        } else { // looking downard
            rayY = ((((int)player.position.y) >> 6) << 6) + CELL_SIZE;
            rayX = (((int)player.position.y) - rayY) * aTan + (int)player.position.x;
            offsetY = CELL_SIZE;
        }
        if (rayAngle == 0 || rayAngle == M_PI) {
            rayX = (int)player.position.x;
            rayY = (int)player.position.y;
            distance = maxDistance;
        }
        offsetX = -offsetY * aTan;
        while (distance < maxDistance) {
            mapX = (int)(rayX) >> 6;
            mapY = (int)(rayY) >> 6;
            mapIndex = mapY * mapWidth + mapX;
            if (mapIndex > 0 && mapIndex < map.size() && map[mapIndex] > 0) {
                // hit wall
                horizontalPosition = { (float)rayX, (float)rayY };
                distHorizontal = getDistance(player.position, horizontalPosition, rayAngle);
                break;
            } else {
                rayX += offsetX;
                rayY += offsetY;
                distance++;
            }
        }

        // vertical

        float nTan = -tan(rayAngle);
        bool isLeft = rayAngle > PI_DIV2 && rayAngle < PI_3;
        float distVertical = 100000.0f;
        Vector2 verticalPosition = { player.position.x, player.position.y };

        if (isLeft) {
            rayX = ((((int)player.position.x) >> 6) << 6) - 0.0001;
            rayY = (((int)player.position.y) - rayX) * nTan + (int)player.position.y;
            offsetX = -CELL_SIZE;
        } else { // looking right
            rayX = ((((int)player.position.x) >> 6) << 6) + CELL_SIZE;
            rayY = (((int)player.position.y) - rayX) * nTan + (int)player.position.y;
            offsetX = CELL_SIZE;
        }
        if (rayAngle == 0 || rayAngle == M_PI) {
            rayX = (int)player.position.x;
            rayY = (int)player.position.y;
            distance = maxDistance;
        }
        offsetY = -offsetX * nTan;
        distance = 0;
        while (distance < maxDistance) {
            mapX = (int)(rayX) >> 6;
            mapY = (int)(rayY) >> 6;
            mapIndex = mapY * mapWidth + mapX;
            if (mapIndex > 0 && mapIndex < map.size() && map[mapIndex] > 0) {
                // hit wall
                verticalPosition = { (float)rayX, (float)rayY };
                distVertical = getDistance(player.position, verticalPosition, rayAngle);
                break;
            } else {
                rayX += offsetX;
                rayY += offsetY;
                distance++;
            }
        }

        if (distHorizontal < distVertical) {
            rayX = horizontalPosition.x;
            rayY = horizontalPosition.y;
            rayDistance = distHorizontal;
        } else {
            rayX = verticalPosition.x;
            rayY = verticalPosition.y;
            rayDistance = distVertical;
        }

        float lineHeight = (CELL_SIZE * Config::DISPLAY_HEIGHT) / rayDistance;
        if (lineHeight > Config::DISPLAY_HEIGHT) lineHeight = Config::DISPLAY_HEIGHT;
        int lineX = i * 8;
        int lineY = Config::DISPLAY_HEIGHT / 2 - lineHeight / 2;
        // DrawLine(lineX, lineY - lineHeight, lineX, lineY, WHITE);
        DrawRectangleLines(lineX, lineY, 8, lineHeight, WHITE);

        rayAngle += ONE_DEG;
    }
}

void update(double dt, Player& player) {
    if (IsKeyDown(KEY_LEFT)) {
        player.rotate(-0.1f);
    }
    if (IsKeyDown(KEY_RIGHT)) {
        player.rotate(0.1f);
    }
    if (IsKeyDown(KEY_UP)) {
        player.moveForward();
    }
    if (IsKeyDown(KEY_DOWN)) {
        player.moveBackward();
    }
}

void render(Player &player) {
    castRays(player, tilemap);
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, Config::WINDOW_TITLE.c_str());

    SetTargetFPS(60);

    double currentTime = 0.0;

    Player player;
    player.position = { 5 * 64, 5 * 64 };

    RenderTexture2D canvas = LoadRenderTexture(Config::DISPLAY_WIDTH, Config::DISPLAY_HEIGHT);
    Rectangle canvasSource = { 0, 0, Config::DISPLAY_WIDTH, Config::DISPLAY_HEIGHT };
    Rectangle canvasDest = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

    while (!WindowShouldClose())
    {
        double time = GetTime();
        update(time - currentTime, player);
        currentTime = time;

        BeginTextureMode(canvas);
            ClearBackground(BLACK);
            render(player);
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(canvas.texture, canvasSource, canvasDest, Vector2 { 0, 0 }, 0, WHITE );
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
