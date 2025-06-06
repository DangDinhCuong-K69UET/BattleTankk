#ifndef MAP_H
#define MAP_H
#include"Tank.h"
#include <SDL.h>
#include <vector>

struct MapTile {
    int type; // 0: empty, 1: wall
    int health; // Health of the wall
    SDL_Texture* wallTexture ;

    MapTile(int tileType = 0, int tileHealth = 100) : type(tileType), health(tileHealth) {}
    SDL_Rect rect; // Vị trí và kích thước của ô
};
struct Base {
    int x, y; // Vị trí của trụ
    int health; // Máu của trụ
    int width, height; // Kích thước của trụ


    SDL_Rect getRect() const {
        return { x, y, width, height };
    }
};
struct HealthPack {
    int x, y;
    bool active;

    SDL_Rect getRect() const {
        return { x, y, 32, 32 }; // Kích thước 32x32
    }
};

class GameMap {
public:
  GameMap(int width, int height); // Thêm tham số renderer
    ~GameMap();
     Base base1, base2;
    SDL_Texture* wallTexture;

    void setWallTexture(SDL_Texture* texture);
    int getWidth() const { return mapWidth; }
    int getHeight() const { return mapHeight; }
    MapTile* getTile(int x, int y); // Lấy tile tại vị trí x, y (trả về nullptr nếu ra ngoài map)
   void render(SDL_Renderer* renderer, SDL_Texture* baseTexture);
    bool isWalkable(float x, float y);  //Kiểm tra xem vị trí có thể đi qua được không
    void spawnHealthPack() ;
    void checkTankHealthCollision(Tank& tank) ;
    std::vector<HealthPack> healthPacks; // Danh sách các cục máu

private:
    int mapWidth;
    int mapHeight;
    std::vector<MapTile> tiles; // Mảng chứa thông tin các ô trên map


};

#endif
