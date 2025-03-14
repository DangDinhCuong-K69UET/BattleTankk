#include "map.h"
#include <iostream>

GameMap::GameMap(int width, int height) : mapWidth(width), mapHeight(height) {
    tiles.resize(width * height);

    // Initialize the map (example with some walls)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = x + y * width;
            tiles[index].type = 0; // Empty
            tiles[index].rect.x = x * 32;
            tiles[index].rect.y = y * 32;
            tiles[index].rect.w = 32;
            tiles[index].rect.h = 32;
            tiles[index].health = 0; //0 for now

            // Example: Create a border of walls
            if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                tiles[index].type = 1; // Wall
                tiles[index].health = 100; // set health if wall
            }
        }
    }
}

GameMap::~GameMap() {}

MapTile* GameMap::getTile(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) {
        return nullptr; // Out of bounds
    }
    return &tiles[x + y * mapWidth];
}

void GameMap::render(SDL_Renderer* renderer) {
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            MapTile* tile = getTile(x, y);
            if (tile) {
                if (tile->type == 0) { // Empty
                    SDL_SetRenderDrawColor(renderer, 0x00, 0x80, 0x00, 0xFF); // Green
                } else { // Wall
                    SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, 0xFF); // Gray
                }
                SDL_RenderFillRect(renderer, &tile->rect);
            }
        }
    }
}

bool GameMap::isWalkable(float x, float y) {
        int tileX = static_cast<int>(x / 32);
        int tileY = static_cast<int>(y / 32);
        MapTile* tile = getTile(tileX, tileY);
    return (tile != nullptr && tile->type == 0);
}
