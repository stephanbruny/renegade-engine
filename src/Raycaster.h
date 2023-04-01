//
// Created by Stephan Bruny on 01.04.23.
//

#ifndef RENEGADE_ENGINE_RAYCASTER_H
#define RENEGADE_ENGINE_RAYCASTER_H

#include <raylib.h>
#include <cmath>
#include <utility>
#include "../config.hpp"
#include "Player.h"
#include "Map.h"

class Raycaster {
private:
    vector<int> floor;
    vector<int> walls;
    vector<int> ceiling;

    Player* player;
    Map* map;
    shared_ptr<Texture2D> textures;

public:

    Raycaster(Map *map, Player *player, shared_ptr<Texture2D> textures) {
        this->player = player;
        this->textures = textures;
        this->map = map;
        this->floor    = *(map->getFloor());
        this->walls    = *(map->getWalls());
        this->ceiling  = *(map->getCeiling());
    }

    void renderFloor() {
        int startY = Config::DISPLAY_HEIGHT / 2;
        int mapWidth = this->map->getWidth();
        for(int y = startY; y < Config::DISPLAY_HEIGHT; y++)
        {
            // rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
            float rayDirX0 = this->player->direction.x - this->player->plane.x;
            float rayDirY0 = this->player->direction.y - this->player->plane.y;
            float rayDirX1 = this->player->direction.x + this->player->plane.x;
            float rayDirY1 = this->player->direction.y + this->player->plane.y;

            // Current y position compared to the center of the screen (the horizon)
            int p = y - Config::DISPLAY_HEIGHT / 2;

            if (p == 0) continue;

            // Vertical position of the camera.
            float posZ = 0.5 * Config::DISPLAY_HEIGHT;

            // Horizontal distance from the camera to the floor for the current row.
            // 0.5 is the z position exactly in the middle between floor and ceiling.
            float rowDistance = posZ / p;

            // calculate the real world step vector we have to add for each x (parallel to camera plane)
            // adding step by step avoids multiplications with a weight in the inner loop
            float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / Config::DISPLAY_WIDTH;
            float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / Config::DISPLAY_WIDTH;

            // real world coordinates of the leftmost column. This will be updated as we step to the right.
            float floorX = this->player->position.x + rowDistance * rayDirX0;
            float floorY = this->player->position.y + rowDistance * rayDirY0;

            for(int x = 0; x < Config::DISPLAY_WIDTH; ++x)
            {
                // the cell coord is simply got from the integer parts of floorX and floorY
                int cellX = (int)(floorX);
                int cellY = (int)(floorY);

                // get the texture coordinate from the fractional part
                int tx = (int)(Config::TEXTURE_SIZE * (floorX - cellX)) & (Config::TEXTURE_SIZE - 1);
                int ty = (int)(Config::TEXTURE_SIZE * (floorY - cellY)) & (Config::TEXTURE_SIZE - 1);

                floorX += floorStepX;
                floorY += floorStepY;

                int floorIndex = cellY * mapWidth + cellX;
                int textureId = this->floor[floorIndex];
                int ceilingTextureId = this->ceiling[floorIndex];
                if (textureId <= 0) continue;

                int atlasWidth = textures->width / Config::TEXTURE_SIZE;
                int textureIdX = textureId % atlasWidth;
                int textureIdY = textureId / atlasWidth;
                float textureX = textureIdX * Config::TEXTURE_SIZE + tx;
                float textureY = textureIdY * Config::TEXTURE_SIZE + ty;

                Vector2 ceilingTexturePos = {
                        static_cast<float>((ceilingTextureId % atlasWidth) * Config::TEXTURE_SIZE + tx),
                        static_cast<float>((ceilingTextureId / atlasWidth) * Config::TEXTURE_SIZE + ty)
                };

                unsigned char depth = y - Config::DISPLAY_HEIGHT / 2;
                Color color { depth, depth, depth, 255 };

                DrawTexturePro(
                        *textures,
                        Rectangle { textureX, textureY, 1, 1 },
                        Rectangle { (float)x, (float)y, 1, 1 },
                        Vector2 { 0, 0 },
                        0.0,
                        color
                );

                if (ceilingTextureId <= 0) continue;
                DrawTexturePro(
                        *textures,
                        Rectangle { ceilingTexturePos.x, ceilingTexturePos.y, 1, 1 },
                        Rectangle { (float)x, (float)Config::DISPLAY_HEIGHT - y, 1, 1 },
                        Vector2 { 0, 0 },
                        0.0,
                        color
                );

                // choose texture and draw the pixel
                // int floorTexture = 3;
                // int ceilingTexture = 6;
                // Uint32 color;


                // floor
                /*color = texture[floorTexture][texWidth * ty + tx];
                color = (color >> 1) & 8355711; // make a bit darker
                buffer[y][x] = color;*/


                //ceiling (symmetrical, at Config::DISPLAY_HEIGHT - y - 1 instead of y)
                /*color = texture[ceilingTexture][texWidth * ty + tx];
                color = (color >> 1) & 8355711; // make a bit darker
                buffer[screenHeight - y - 1][x] = color;*/
            }
        }
    }

    void renderRaycaster() {
        int mapWidth = this->map->getWidth();
        for (int x = 0; x < Config::DISPLAY_WIDTH; x++) {
            double cameraX = 2 * x / double(Config::DISPLAY_WIDTH) - 1; //x-coordinate in camera space
            double rayDirX = this->player->direction.x + this->player->plane.x * cameraX;
            double rayDirY = this->player->direction.y + this->player->plane.y * cameraX;

            //which box of the map we're in
            int mapX = int(this->player->position.x);
            int mapY = int(this->player->position.y);

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
                sideDistX = (this->player->position.x - mapX) * deltaDistX;
            } else {
                stepX = 1;
                sideDistX = (mapX + 1.0 - this->player->position.x) * deltaDistX;
            }
            if (rayDirY < 0) {
                stepY = -1;
                sideDistY = (this->player->position.y - mapY) * deltaDistY;
            } else
            {
                stepY = 1;
                sideDistY = (mapY + 1.0 - this->player->position.y) * deltaDistY;
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
                if (mapIndex >= 0 && mapIndex < walls.size() && walls[mapIndex] > 0) {
                    wallTextureId = walls[mapIndex];
                    break;
                }
                rayDepth++;
            }

            //calculate value of wallX
            double wallX; //where exactly the wall was hit
            if (side == 0) wallX = this->player->position.y + perpWallDist * rayDirY;
            else           wallX = this->player->position.x + perpWallDist * rayDirX;
            wallX -= (double)::floor((wallX));

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
            int atlasWidth = textures->width / Config::TEXTURE_SIZE;
            int textureIdX = wallTextureId % atlasWidth;
            int textureIdY = wallTextureId / atlasWidth;
            float textureX = textureIdX * Config::TEXTURE_SIZE + texX;
            float textureY = textureIdY * Config::TEXTURE_SIZE;
            DrawTexturePro(
                    *textures,
                    Rectangle { textureX, textureY, 1, Config::TEXTURE_SIZE },
                    Rectangle { (float)x, (float)drawStart, 1, float(drawEnd - drawStart) },
                    Vector2 { 0, 0 },
                    0.0,
                    color
            );
        }
    }


};

#endif //RENEGADE_ENGINE_RAYCASTER_H
