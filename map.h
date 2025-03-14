#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <vector>

struct MapTile {
    int type; // 0: empty, 1: wall
    int health; // Health of the wall

    MapTile(int tileType = 0, int tileHealth = 100) : type(tileType), health(tileHealth) {}
    SDL_Rect rect; // Vị trí và kích thước của ô
};

class GameMap {
public:
    GameMap(int width, int height);
    ~GameMap();

    int getWidth() const { return mapWidth; }
    int getHeight() const { return mapHeight; }
    MapTile* getTile(int x, int y); // Lấy tile tại vị trí x, y (trả về nullptr nếu ra ngoài map)
    void render(SDL_Renderer* renderer); // Vẽ map
    bool isWalkable(float x, float y);  //Kiểm tra xem vị trí có thể đi qua được không

private:
    int mapWidth;
    int mapHeight;
    std::vector<MapTile> tiles; // Mảng chứa thông tin các ô trên map
};

#endif
