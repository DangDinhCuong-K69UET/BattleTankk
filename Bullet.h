#ifndef BULLET_H
#define BULLET_H

#include <SDL.h>

struct Bullet {
    float x;
    float y;
    float angle; // Angle of the bullet (0, 90, 180, 270)
    bool isAlive;
    int owner; // 0 for player 1, 1 for player 2

    Bullet(float startX, float startY, float angle, int ownerID);
};

#endif
