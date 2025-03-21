#include "map.h"
#include <SDL.h>
#include <iostream>

GameMap::GameMap(int width, int height) : mapWidth(width), mapHeight(height) {
    tiles.resize(width * height, MapTile());

    // Tạo tường xung quanh bản đồ
    for (int x = 0; x < width; ++x) {
        tiles[x].type = 1; // Hàng trên
        tiles[(height - 1) * width + x].type = 1; // Hàng dưới
    }
    for (int y = 0; y < height; ++y) {
        tiles[y * width].type = 1; // Cột trái
        tiles[y * width + (width - 1)].type = 1; // Cột phải
    }

    // Định nghĩa chữ "IT" ở giữa bản đồ
    int startX = width / 2 - 2;
    int startY = height / 2 - 2;

    // Vẽ chữ "I"
    for (int y = startY; y < startY + 5; ++y) {
        tiles[y * width + startX].type = 1;
    }

    // Vẽ chữ "T"
    for (int x = startX + 2; x < startX + 6; ++x) {
        tiles[startY * width + x].type = 1;
    }
    for (int y = startY; y < startY + 5; ++y) {
        tiles[y * width + startX + 3].type = 1;
    }
}

GameMap::~GameMap() {}

MapTile* GameMap::getTile(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) {
        return nullptr;
    }
    return &tiles[y * mapWidth + x];
}

void GameMap::render(SDL_Renderer* renderer) {
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            MapTile& tile = tiles[y * mapWidth + x];
            SDL_Rect rect = { x * 32, y * 32, 32, 32 }; // Kích thước mỗi tile là 32x32

            if (tile.type == 1) {
                SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Màu nâu cho tường
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu đen cho nền
            }
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

bool GameMap::isWalkable(float x, float y) {
    int tileX = static_cast<int>(x) / 32;
    int tileY = static_cast<int>(y) / 32;

    // Kiểm tra nếu ra ngoài bản đồ
    if (tileX < 0 || tileX >= mapWidth || tileY < 0 || tileY >= mapHeight) {
        return false;
    }

    MapTile* tile = getTile(tileX, tileY);
    return (tile != nullptr && tile->type == 0); // Chỉ đi được nếu type == 0 (không phải tường)
}

