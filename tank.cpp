#include "tank.h"
#include "map.h"
#include <cmath>
#include <iostream>
#include<SDL_image.h>

// Constants (defined in tank.cpp because they're only used here)
const int TANK_WIDTH = 50;
const int TANK_HEIGHT = 30;
const int TANK_SPEED = 3;
const int TURRET_ROTATION_SPEED = 3;
const int TANK_SIZE = 32;
const int TILE_SIZE = 32;


Tank::Tank(float startX, float startY)
    : x(startX), y(startY), angle(0), turretAngle(0), health(100) {}


void Tank::move(int dx, int dy, GameMap& gameMap) {
   float newX = x + dx;
    float newY = y + dy;
  if (dx > 0) turretAngle = 0;
    if (dx < 0) turretAngle = 180;
    if (dy > 0) turretAngle = 90;
    if (dy < 0) turretAngle = 270;

     bool canMove =
        gameMap.isWalkable(newX, newY) &&
        gameMap.isWalkable(newX + TANK_SIZE - 1, newY) &&
        gameMap.isWalkable(newX, newY + TANK_SIZE - 1) &&
        gameMap.isWalkable(newX + TANK_SIZE - 1, newY + TANK_SIZE - 1);

    if (canMove) {
        x = newX;
        y = newY;
    }
}
void Tank::loadTexture(SDL_Renderer* gRenderer, const char* filePath) {
    SDL_Surface* tempSurface = IMG_Load(filePath);
    texture = SDL_CreateTextureFromSurface(gRenderer, tempSurface);
    SDL_FreeSurface(tempSurface);
}
void Tank::render(SDL_Renderer* gRenderer) {
    SDL_Rect dst = { static_cast<int>(x), static_cast<int>(y), 32, 32 };
    SDL_RenderCopyEx(gRenderer, texture, nullptr, &dst, angle, nullptr, SDL_FLIP_NONE);
}

void Tank::setDirection(int direction) {

    direction = direction % 360;
    if (direction < 0) direction += 360;

    // Assign direction
    angle = direction;
}
