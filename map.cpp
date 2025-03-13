#include "map.h"
#include <iostream>
GameMap::GameMap(int width, int height) : mapWidth(width), mapHeight(height) {
    tiles.resize(width * height); // Allocate memory for the entire map

    // Initialize the map with some default values (e.g., empty tiles)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = x + y * width;
            tiles[index].type = 0; // Empty tile
            tiles[index].rect.x = x * 32; // Example tile size of 32x32
            tiles[index].rect.y = y * 32;
            tiles[index].rect.w = 32;
            tiles[index].rect.h = 32;
        }
    }

    // Example: Create some walls along the edges
    for (int x = 0; x < width; ++x) {
        tiles[x].type = 1; // Top row wall
        tiles[x + (height - 1) * width].type = 1; // Bottom row wall
    }
    for (int y = 0; y < height; ++y) {
        tiles[y * width].type = 1; // Left column wall
        tiles[width - 1 + y * width].type = 1; // Right column wall
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
                // Set color based on tile type (example)
                if (tile->type == 0) { // Empty
                    SDL_SetRenderDrawColor(renderer, 0x00, 0x80, 0x00, 0xFF); // Green
                } else if (tile->type == 1) { // Wall
                    SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, 0xFF); // Gray
                } else {
                    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0xFF); // Pink (error)
                }
                SDL_RenderFillRect(renderer, &tile->rect);
            }
        }
    }
}

bool GameMap::isWalkable(float x, float y) {
  int tileX = static_cast<int>(x / 32); // Convert world coords to tile coords (assuming 32x32 tiles)
  int tileY = static_cast<int>(y / 32);

  MapTile* tile = getTile(tileX, tileY);
  if (!tile) return false; // Out of bounds is not walkable
  return (tile->type == 0); // Walkable if tile type is 0 (empty)
}
