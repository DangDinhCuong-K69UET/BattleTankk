#ifndef TANK_H
#define TANK_H

#include <SDL.h>
class GameMap;  // Forward declaration

struct Tank {
    float x;
    float y;
    float angle; // Tank's facing angle in degrees
    float turretAngle; // Angle of the turret relative to the tank body
    int health;

    Tank(float startX, float startY);
    void move(int dx, int dy, GameMap& gameMap);  //Move dx, dy again!
    void setDirection(int direction);  // New method
};

extern const int TANK_WIDTH;
extern const int TANK_HEIGHT;
extern const int TANK_SPEED;
extern const int TURRET_ROTATION_SPEED;

#endif
