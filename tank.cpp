#include "tank.h"
#include "map.h"
#include <cmath>
#include <iostream>

// Constants (defined in tank.cpp because they're only used here)
const int TANK_WIDTH = 50;
const int TANK_HEIGHT = 30;
const int TANK_SPEED = 3;
const int TURRET_ROTATION_SPEED = 3;

Tank::Tank(float startX, float startY) :
    x(startX), y(startY), angle(0), turretAngle(0), health(100) {}

void Tank::move(int dx, int dy, GameMap& gameMap) {
   float newX = x + dx;
    float newY = y + dy;
    x += dx;
    y += dy;

    // Cập nhật turretAngle theo hướng di chuyển
    if (dx > 0)      turretAngle = 0;    // Đi sang phải
    else if (dx < 0) turretAngle = 180;  // Đi sang trái
    else if (dy > 0) turretAngle = 90;   // Đi xuống
    else if (dy < 0) turretAngle = -90;  // Đi lên

    // Check if the new position is walkable
    if (gameMap.isWalkable(newX, newY)) {
        x = newX;
        y = newY;
    }
}

void Tank::setDirection(int direction) {
    // Normalize the angle to be within 0-360 degrees
    direction = direction % 360;
    if (direction < 0) direction += 360;  // Ensure positive angle

    // Assign direction
    angle = direction;
}
