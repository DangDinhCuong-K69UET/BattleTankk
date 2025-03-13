#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <vector>

// Định nghĩa struct cho một ô trên map (ví dụ: tường, đất trống, v.v.)
struct MapTile {
  int type; // 0: đất trống, 1: tường, ...
  SDL_Rect rect; // Vị trí và kích thước của ô
};

class GameMap {
public:
  GameMap(int width, int height);
  ~GameMap();

  int getWidth() const { return mapWidth; }
  int getHeight() const { return mapHeight; }

  // Lấy tile tại vị trí x, y (trả về nullptr nếu ra ngoài map)
  MapTile* getTile(int x, int y);

  // Vẽ map
  void render(SDL_Renderer* renderer);

  // Kiểm tra xem vị trí có thể đi qua được không
  bool isWalkable(float x, float y);

private:
  int mapWidth;
  int mapHeight;
  std::vector<MapTile> tiles; // Mảng chứa thông tin các ô trên map
};

#endif
