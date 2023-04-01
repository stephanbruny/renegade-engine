#include <iostream>
#include <raylib.h>
#include <vector>
#include <cmath>
#include "config.hpp"

using namespace std;

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;

constexpr int MAP_WIDTH = 10;
constexpr int CELL_SIZE = 32;
constexpr float PI_DIV2 = M_PI / 2;
constexpr float PI_3 = 3*PI_DIV2;
constexpr float PI_2 = 2*M_PI;

constexpr float ONE_DEG = 0.0174533f;


constexpr float MAX_ROTATION = 360.0f;

inline float degreeToRadians(float degree) {
    return degree * (float)M_PI / 180;
}

vector<int> tilemap = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 0, 1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

struct Player {
    float rotation;
    Vector2 position;
    Vector2 delta;

    Player(float x = 0, float y = 0) {
        this->rotation = 0.0f;
        this->position = Vector2 { x, y };
        this->delta = Vector2 { 0, 0 };
    }

    void rotate(float amount = 0.1f, float deltaAmount = 1.0f) {
        this->rotation += amount;

        if (this->rotation < 0) this->rotation = MAX_ROTATION;
        if (this->rotation > MAX_ROTATION) this->rotation = 0;

        this->delta.x = cos(degreeToRadians(this->rotation)) * deltaAmount;
        this->delta.y = sin(degreeToRadians(this->rotation)) * deltaAmount;
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

void castRays(Player &player, vector<int> map, int maxDistance = 1000, int mapWidth = 10, int fov = 60) {
    int halfScreenWidth = Config::DISPLAY_WIDTH / 2;
    int halfScreenHeight = Config::DISPLAY_HEIGHT / 2;
    int halfFov = fov / 2;
    float incrementAngle = (float)fov / Config::DISPLAY_WIDTH;
    float rayAngle = (float)player.rotation - (float)halfFov;
    for(int rayCount = 0; rayCount < Config::DISPLAY_WIDTH; rayCount++) {
        if (rayAngle < 0) rayAngle = 360.0f + rayAngle;
        if (rayAngle > 360.0f) rayAngle = rayAngle - 360.0f;
        float rad = degreeToRadians(rayAngle);
        Vector2 ray { player.position.x, player.position.y };
        float rayCos = cos(rad) / CELL_SIZE;
        float raySin = sin(rad) / CELL_SIZE;
        int wallDistance = 0;

        while (wallDistance < maxDistance) {
            ray.x += rayCos;
            ray.y += raySin;
            int x = (int)ray.x / CELL_SIZE;
            int y = (int)ray.y / CELL_SIZE;
            int wallIndex = y * mapWidth + x;
            if (wallIndex >= 0 && wallIndex < map.size() && map[wallIndex] > 0) {
                break;
            }
            wallDistance++;
        }

        /*
        float px = player.position.x - ray.x;
        float py = player.position.y - ray.y;
        auto distance = (float)::sqrt(
                (float)::pow(px, 2) + (float)::pow(py, 2)
        );*/

        float distance = getDistance(player.position, ray, rad);
        unsigned char depth = 255;
        depth = (1 / distance) * 255;
        if (depth < 0) depth = 0;
        if (depth > 255) depth = 255;
        distance = distance * (float)cos(degreeToRadians(rayAngle - player.rotation));

        int wallHeight = (int)::floor((float)halfScreenHeight / distance);
        if (wallHeight < 0) wallHeight = 0;
        if (wallHeight > Config::DISPLAY_HEIGHT) wallHeight = Config::DISPLAY_HEIGHT;
        Color color = { depth, depth, depth, 255 };
        if (distance > 0)
            DrawLine(
                rayCount,
                halfScreenHeight - wallHeight,
                rayCount,
                halfScreenHeight + wallHeight,
                color
            );
        rayAngle += incrementAngle;
    }
}

void update(double dt, Player& player) {
    if (IsKeyDown(KEY_LEFT)) {
        player.rotate(-1.0f);
    }
    if (IsKeyDown(KEY_RIGHT)) {
        player.rotate(1.0f);
    }
    if (IsKeyDown(KEY_UP)) {
        player.moveForward();
    }
    if (IsKeyDown(KEY_DOWN)) {
        player.moveBackward();
    }
}

void render(Player &player) {
    ClearBackground(BLACK);
    castRays(player, tilemap);
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

    DrawCircle(player.position.x + offset.x, player.position.y + offset.y, 8, playerColor);
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, Config::WINDOW_TITLE.c_str());

    SetTargetFPS(60);

    double currentTime = 0.0;

    Player player;
    player.position = { 5 * CELL_SIZE, 5 * CELL_SIZE };
    player.rotation = 180;

    RenderTexture2D canvas = LoadRenderTexture(Config::DISPLAY_WIDTH, Config::DISPLAY_HEIGHT);
    Rectangle canvasSource = { 0, 0, Config::DISPLAY_WIDTH, Config::DISPLAY_HEIGHT };
    Rectangle canvasDest = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

    while (!WindowShouldClose())
    {
        double time = GetTime();
        update(time - currentTime, player);
        currentTime = time;

        BeginTextureMode(canvas);
            render(player);
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(canvas.texture, canvasSource, canvasDest, Vector2 { 0, 0 }, 0, WHITE );
            renderMinimap(player, tilemap, 10);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
