#ifndef TANK_H
#define TANK_H

#include <SDL.h>

// Forward declaration của Bullet (để tránh include vòng)
struct Bullet;

struct Tank {
    float x;
    float y;
    float angle; // Tank's facing angle in degrees
   // SDL_Texture* texture;
    //SDL_Texture* turretTexture; // REMOVED
    float turretAngle; // Turret's angle relative to the tank's body
    int health;

    Tank(float startX, float startY); // Modified Constructor
};

#endif
