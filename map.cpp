#include "map.h"
#include <SDL.h>
#include <iostream>
#include<SDL_image.h>

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
    base1 = {1, 1, 50};      // Trụ của Player 1 (góc trên trái)
    base2 = {mapWidth - 3, mapHeight - 3, 50}; // Trụ của Player 2 (góc dưới phải)

}

GameMap::~GameMap() {}

MapTile* GameMap::getTile(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) {
        return nullptr;
    }
    return &tiles[y * mapWidth + x];
}

void GameMap::render(SDL_Renderer* renderer, SDL_Texture* baseTexture) {
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
    SDL_Rect baseRect = {base1.x * 32, base1.y * 32, 64, 64}; // Trụ 1 (Kích thước lớn hơn tường)
    SDL_RenderCopy(renderer, baseTexture, NULL, &baseRect);

    baseRect = {base2.x * 32, base2.y * 32, 64, 64}; // Trụ 2
    SDL_RenderCopy(renderer, baseTexture, NULL, &baseRect);

}


bool GameMap::isWalkable(float x, float y) {
    int tileX = static_cast<int>(x) / 32;
    int tileY = static_cast<int>(y) / 32;

    // Kiểm tra nếu ra ngoài bản đồ
    if (tileX < 0 || tileX >= mapWidth || tileY < 0 || tileY >= mapHeight) {
        return false;
    }
      for (int dx = 0; dx < 1; dx++) {
        for (int dy = 0; dy < 1; dy++) {
            int checkX = tileX + dx;
            int checkY = tileY + dy;

            if ((checkX >= base1.x && checkX < base1.x + 2 &&
                 checkY >= base1.y && checkY < base1.y + 2) ||
                (checkX >= base2.x && checkX < base2.x + 2 &&
                 checkY >= base2.y && checkY < base2.y + 2)) {
                return false;
            }
        }
    }


    MapTile* tile = getTile(tileX, tileY);
    return (tile != nullptr && tile->type == 0); // Chỉ đi được nếu type == 0 (không phải tường)
}

