#include "map.h"
#include <SDL.h>
#include <iostream>
#include<SDL_image.h>

GameMap::GameMap(int width, int height) : mapWidth(width), mapHeight(height) {
    tiles.resize(width * height, MapTile());
    // Tạo tường xung quanh bản đồ
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            MapTile& tile = tiles[y * mapWidth + x];

            // Tạo tường xung quanh màn hình
            if (x == 0 || x == mapWidth - 1 || y == 0 || y == mapHeight - 1) {
                tile.type = 1; // Tường
            }

            // Tạo tường bảo vệ trụ góc trên trái
            if ((x >= 1 && x <= 3 && y >= 1&& y <= 3)) {
                tile.type = 1;
            }

            // Tạo tường bảo vệ trụ góc dưới phải
            if ((x >= mapWidth - 4 && x <= mapWidth - 2 && y >= mapHeight - 4 && y <= mapHeight - 2)) {
                tile.type = 1;
            }
        }
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
void GameMap::setWallTexture(SDL_Texture* texture) {
    if (!texture) {
       // std::cerr << "setWallTexture: Texture bị null!" << std::endl;
        return;
    }
    wallTexture = texture;
    for (auto& tile : tiles) {
        if (tile.type == 1) {
            tile.wallTexture = texture;
        }
    }
}

void GameMap::render(SDL_Renderer* renderer, SDL_Texture* baseTexture) {
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            MapTile& tile = tiles[y * mapWidth + x];
            SDL_Rect rect = { x * 32, y * 32, 32, 32 }; // Kích thước tile

            if (tile.type == 1 && tile.wallTexture) { // Nếu là tường có texture
                SDL_RenderCopy(renderer, tile.wallTexture, NULL, &rect);
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu đen cho nền
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    // Vẽ trụ của 2 người chơi
    SDL_Rect baseRect = {base1.x * 32, base1.y * 32, 64, 64};
    SDL_RenderCopy(renderer, baseTexture, NULL, &baseRect);

    baseRect = {base2.x * 32, base2.y * 32, 64, 64};
    SDL_RenderCopy(renderer, baseTexture, NULL, &baseRect);
}

bool GameMap::isWalkable(float x, float y) {
    int tileX = static_cast<int>(x) / 32;
    int tileY = static_cast<int>(y) / 32;

    // Chặn xe ra ngoài màn hình
    if (x < 0 || y < 0 || x >= (mapWidth * 32) || y >= (mapHeight * 32)) {
        return false;
    }

    // Chặn xe đi vào trụ
    if ((tileX >= base1.x && tileX < base1.x + 2 &&
         tileY >= base1.y && tileY < base1.y + 2) ||
        (tileX >= base2.x && tileX < base2.x + 2 &&
         tileY >= base2.y && tileY < base2.y + 2)) {
        return false;
    }

    // Kiểm tra tường
    MapTile* tile = getTile(tileX, tileY);
    return (tile && tile->type == 0); // Chỉ đi được nếu không phải tường
}
