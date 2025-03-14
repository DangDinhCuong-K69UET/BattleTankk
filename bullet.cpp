#include "bullet.h"

Bullet::Bullet(float startX, float startY, float angle, int ownerID) :
    x(startX), y(startY), angle(angle), isAlive(true), owner(ownerID) {}
