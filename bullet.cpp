#include "bullet.h"
#include"map.h"
const int BULLET_WIDTH = 10;
const int BULLET_HEIGHT = 10;
Bullet::Bullet(float startX, float startY, float angle, int ownerID) :
    x(startX), y(startY), angle(angle), isAlive(true), owner(ownerID) {}

void renderBullet(SDL_Renderer* renderer, float x, float y) {
    // Define the points of the triangle
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color

    // Draw the triangle (pointing upwards)
    float halfWidth = BULLET_WIDTH / 2.0f;
    float halfHeight = BULLET_HEIGHT / 2.0f;

    float x1 = x;
    float y1 = y - halfHeight; // Top point

    float x2 = x - halfWidth;
    float y2 = y + halfHeight; // Bottom-left point

    float x3 = x + halfWidth;
    float y3 = y + halfHeight; // Bottom-right point

    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
    SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
}
