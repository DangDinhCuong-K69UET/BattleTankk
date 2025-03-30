#ifndef TANK_H
#define TANK_H

#include <SDL.h>
class GameMap;

struct Tank {
    float x;
    float y;
    float angle;
    float turretAngle; // Angle of the turret relative to the tank body
    int health;
    SDL_Texture* texture;


    Tank(float startX, float startY);
    void loadTexture(SDL_Renderer* renderer, const char* filePath);
    void move(int dx, int dy, GameMap& gameMap);  //Move dx, dy again!
    void setDirection(int direction);  // New method Tank(float startX, float startY);
    float getX() const { return x; }
    float getY() const { return y; }
    void render(SDL_Renderer* gRenderer);
    void heal(int amount);
};

extern const int TANK_WIDTH;
extern const int TANK_HEIGHT;
extern const int TANK_SPEED;
extern const int TURRET_ROTATION_SPEED;

#endif
