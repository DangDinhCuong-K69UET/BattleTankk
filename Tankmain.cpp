#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include "tank.h"
#include "bullet.h"
#include "map.h"

// Constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int BULLET_WIDTH = 10;
const int BULLET_HEIGHT = 10;
const int BULLET_SPEED = 10;
const int HEALTH_BAR_WIDTH = 100;
const int HEALTH_BAR_HEIGHT = 10;

// Global Variables
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

// Function Prototypes
bool init();
void close();
void renderTexture(SDL_Texture* texture, int x, int y, SDL_Renderer* renderer, SDL_Rect* clip = nullptr, double angle = 0.0, SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);
void handleInput(SDL_Event& e, Tank& player1, Tank& player2, std::vector<Bullet>& bullets, GameMap& gameMap);
void update(Tank& player1, Tank& player2, std::vector<Bullet>& bullets, GameMap& gameMap);
void render(SDL_Renderer* renderer, Tank& player1, Tank& player2, std::vector<Bullet>& bullets, GameMap& gameMap);
bool checkCollision(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2);
void drawHealthBar(SDL_Renderer* renderer, int x, int y, int health, int maxHealth, int barWidth, int barHeight);  // New health bar function
void renderBullet(SDL_Renderer* renderer, float x, float y);


bool init() {
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        success = false;
    } else {
        // Create window
        gWindow = SDL_CreateWindow("Tank Battle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == nullptr) {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            success = false;
        } else {
            // Create renderer
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == nullptr) {
                std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
                success = false;
            } else {
                // Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                // Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags)) {
                    std::cerr << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
                    success = false;
                }

            }
        }
    }

    return success;
}

void close() {
    // Destroy renderer
    SDL_DestroyRenderer(gRenderer);
    gRenderer = nullptr;

    // Destroy window
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

// Hàm tải texture từ file ảnh
SDL_Texture* loadTexture(const char* filename, SDL_Renderer* renderer) {
    SDL_Surface* tempSurface = IMG_Load(filename);
    if (!tempSurface) {
        std::cout << "Không thể load ảnh: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    return texture;
}

// Hiển thị màn hình chờ
void showStartScreen(SDL_Renderer* renderer) {
    SDL_Texture* startScreen = loadTexture("batdau.png", renderer); // Đổi đúng đường dẫn ảnh

    if (!startScreen) return; // Thoát nếu không load được ảnh

    bool waiting = true;
    SDL_Event event;

    while (waiting) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, startScreen, NULL, NULL); // Vẽ ảnh toàn màn hình
        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0); // Thoát game nếu bấm nút tắt cửa sổ
            } else if (event.type == SDL_KEYDOWN) {
                waiting = false; // Nhấn phím bất kỳ để vào game
            }
        }

        SDL_Delay(10);
    }

    SDL_DestroyTexture(startScreen);
}
void showVictoryScreen(SDL_Renderer* renderer, const char* imagePath) {
    SDL_Texture* victoryTexture = NULL;
    SDL_Surface* victorySurface = IMG_Load(imagePath);

    if (!victorySurface) {
        printf("Failed to load victory image: %s\n", IMG_GetError());
        return;
    }

    victoryTexture = SDL_CreateTextureFromSurface(renderer, victorySurface);
    SDL_FreeSurface(victorySurface);

    if (!victoryTexture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        return;
    }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, victoryTexture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Event e;
    bool running = true;

    // Chờ người chơi bấm phím bất kỳ để thoát
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN) {
                running = false;
                break;
            }
        }
    }

    SDL_DestroyTexture(victoryTexture);
}
bool checkTankCollision(float newX1, float newY1, float newX2, float newY2) {
    float distanceX = newX1 - newX2;
    float distanceY = newY1 - newY2;
    float combinedWidth = TANK_WIDTH * 2; //Total
    float combinedHeight = TANK_HEIGHT * 2;

        return (abs(distanceX) < combinedWidth/2.0f &&
        abs(distanceY) < combinedHeight/2.0f);

}

void renderTexture(SDL_Texture* texture, int x, int y, SDL_Renderer* renderer, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
    SDL_Rect renderQuad = {x, y, 0, 0};

    if (clip != nullptr) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    } else {
        SDL_QueryTexture(texture, NULL, NULL, &renderQuad.w, &renderQuad.h);
    }

    SDL_RenderCopyEx(renderer, texture, clip, &renderQuad, angle, center, flip);
}

void handleInput(SDL_Event& e, Tank& player1, Tank& player2, std::vector<Bullet>& bullets, GameMap& gameMap) {
    // Player 1 controls: W, A, S, D for movement, Space to fire
    // Player 2 controls: Up, Left, Down, Right for movement,k to fire
    // Lấy trạng thái bàn phím
    const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);

    // Kiểm tra di chuyển của player1
    if (currentKeyStates[SDL_SCANCODE_W]) {
        float newX = player1.getX();
        float newY = player1.getY() - TANK_SPEED;
        if (!checkTankCollision(newX, newY, player2.getX(), player2.getY())) {
            player1.move(0, -TANK_SPEED, gameMap);
            player1.setDirection(270);
        }
    }
    if (currentKeyStates[SDL_SCANCODE_A]) {
        float newX = player1.getX() - TANK_SPEED;
        float newY = player1.getY();
        if (!checkTankCollision(newX, newY, player2.getX(), player2.getY())) {
            player1.move(-TANK_SPEED, 0, gameMap);
            player1.setDirection(180);
        }
    }
    if (currentKeyStates[SDL_SCANCODE_S]) {
        float newX = player1.getX();
        float newY = player1.getY() + TANK_SPEED;
        if (!checkTankCollision(newX, newY, player2.getX(), player2.getY())) {
            player1.move(0, TANK_SPEED, gameMap);
            player1.setDirection(90);
        }
    }
    if (currentKeyStates[SDL_SCANCODE_D]) {
        float newX = player1.getX() + TANK_SPEED;
        float newY = player1.getY();
        if (!checkTankCollision(newX, newY, player2.getX(), player2.getY())) {
            player1.move(TANK_SPEED, 0, gameMap);
            player1.setDirection(0);
        }
    }

    // Kiểm tra di chuyển của player2
    if (currentKeyStates[SDL_SCANCODE_UP]) {
        float newX = player2.getX();
        float newY = player2.getY() - TANK_SPEED;
        if (!checkTankCollision(newX, newY, player1.getX(), player1.getY())) {
            player2.move(0, -TANK_SPEED, gameMap);
            player2.setDirection(270);
        }
    }
    if (currentKeyStates[SDL_SCANCODE_LEFT]) {
        float newX = player2.getX() - TANK_SPEED;
        float newY = player2.getY();
        if (!checkTankCollision(newX, newY, player1.getX(), player1.getY())) {
            player2.move(-TANK_SPEED, 0, gameMap);
             player2.setDirection(180);
        }
    }
    if (currentKeyStates[SDL_SCANCODE_DOWN]) {
        float newX = player2.getX();
        float newY = player2.getY() + TANK_SPEED;
        if (!checkTankCollision(newX, newY, player1.getX(), player1.getY())) {
            player2.move(0, TANK_SPEED, gameMap);
             player2.setDirection(90);
        }
    }
    if (currentKeyStates[SDL_SCANCODE_RIGHT]) {
        float newX = player2.getX() + TANK_SPEED;
        float newY = player2.getY();
        if (!checkTankCollision(newX, newY, player1.getX(), player1.getY())) {
            player2.move(TANK_SPEED, 0, gameMap);
             player2.setDirection(0);
        }
    }

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            exit(0);
        } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_SPACE: {
                    // Player 1 fire
                    float bulletStartX = player1.x + (TANK_WIDTH / 2.0f);
                    float bulletStartY = player1.y + (TANK_HEIGHT / 2.0f);

                    float bulletAngle = player1.angle;

                    bullets.emplace_back(bulletStartX, bulletStartY, bulletAngle, 0);
                    break;
                }

                case SDLK_k : {
                    // Player 2 fire
                    float bulletStartX = player2.x + (TANK_WIDTH / 2.0f);
                    float bulletStartY = player2.y + (TANK_HEIGHT / 2.0f);

                       float bulletAngle = player2.angle;

                    bullets.emplace_back(bulletStartX, bulletStartY, bulletAngle, 1);
                    break;
                                  }
                default:
                    break;
            }
        }
    }
}


void update(Tank& player1, Tank& player2, std::vector<Bullet>& bullets, GameMap& gameMap) {
    // Update bullet positions
    for (auto& bullet : bullets) {
        if (bullet.isAlive) {
            bullet.x += BULLET_SPEED * cos(bullet.angle * M_PI / 180.0);
            bullet.y += BULLET_SPEED * sin(bullet.angle * M_PI / 180.0);

            // Check for bullet out of bounds
            if (bullet.x < 0 || bullet.x > SCREEN_WIDTH || bullet.y < 0 || bullet.y > SCREEN_HEIGHT) {
                bullet.isAlive = false;
            }

            // Check for collision with map (example: destroy bullet on wall)
             int tileX = static_cast<int>(bullet.x / 32); // Assuming tile size of 32
            int tileY = static_cast<int>(bullet.y / 32);
            MapTile* tile = gameMap.getTile(tileX, tileY);

            if (tile && tile->type == 1) {
                // Destroy the wall (set tile type to 0)
                 tile->type = 0;

              //Adjusts if it hits, bullet takes damange
               // tile->health -=10;
                bullet.isAlive = false;

            }

        }
    }

    // Check for bullet collisions with tanks
    for (auto& bullet : bullets) {
        if (bullet.isAlive) {
            if (bullet.owner == 0) { // Player 1's bullet
                if (checkCollision(bullet.x, bullet.y, BULLET_WIDTH, BULLET_HEIGHT, player2.x, player2.y, TANK_WIDTH, TANK_HEIGHT)) {
                    bullet.isAlive = false;
                    player2.health -= 20; // Adjust damage as needed
                    if (player2.health < 0) player2.health = 0;
                }
            } else { // Player 2's bullet
                if (checkCollision(bullet.x, bullet.y, BULLET_WIDTH, BULLET_HEIGHT, player1.x, player1.y, TANK_WIDTH, TANK_HEIGHT)) {
                    bullet.isAlive = false;
                    player1.health -= 20; // Adjust damage as needed
                    if (player1.health < 0) player1.health = 0;
                }
            }
        }
    }

    // Remove dead bullets
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.isAlive; }), bullets.end());
}

void drawHealthBar(SDL_Renderer* renderer, int x, int y, int health, int maxHealth, int barWidth, int barHeight) {
    // Calculate health percentage
    float healthPercentage = static_cast<float>(health) / maxHealth;

    // Draw the background of the health bar
    SDL_Rect backgroundRect = { x, y, barWidth, barHeight };
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // Black
    SDL_RenderFillRect(renderer, &backgroundRect);

    // Draw the health portion of the health bar
    SDL_Rect healthRect = { x, y, static_cast<int>(barWidth * healthPercentage), barHeight };
    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF); // Green
    SDL_RenderFillRect(renderer, &healthRect);

    //Draw a border
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // White
    SDL_RenderDrawRect(renderer, &backgroundRect);
}

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

void render(SDL_Renderer* renderer, Tank& player1, Tank& player2, std::vector<Bullet>& bullets, GameMap& gameMap) {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // Black background

        gameMap.render(renderer);
    //   SDL_Rect tankRect1 = { static_cast<int>(player1.x), static_cast<int>(player1.y), TANK_WIDTH, TANK_HEIGHT };
   // SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF); // Red
   // SDL_RenderFillRect(renderer, &tankRect1);
    player1.render(gRenderer) ;
     player2.render(gRenderer) ;
   // SDL_Rect tankRect2 = { static_cast<int>(player2.x), static_cast<int>(player2.y), TANK_WIDTH, TANK_HEIGHT };
    //SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF); // Blue
    //SDL_RenderFillRect(renderer, &tankRect2);


    // Render bullets - REPLACED WITH WHITE TRIANGLE
    for (const auto& bullet : bullets) {
        if (bullet.isAlive) {
            renderBullet(renderer, bullet.x, bullet.y);
        }
    }

    // Display health - REPLACED WITH HEALTH BARS
    drawHealthBar(renderer, 10, 10, player1.health, 100, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT);
    drawHealthBar(renderer, SCREEN_WIDTH - HEALTH_BAR_WIDTH - 10, 10, player2.health, 100, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT);

    // Update screen
    SDL_RenderPresent(renderer);
}

bool checkCollision(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2) {
    if (x1 < x2 + w2 &&
        x1 + w1 > x2 &&
        y1 < y2 + h2 &&
        y1 + h1 > y2) {
        return true;
    }
    return false;
}

int main(int argc, char* args[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("Tank Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);

    if (!init()) {
        std::cerr << "Initialization failed!" << std::endl;
        return 1;
    }
      showStartScreen(gRenderer);
    // Create the game map
    GameMap gameMap(25, 19); // Example: 25x19 tile map
    // Create tanks
    Tank player1(100, 100);
    player1.loadTexture(gRenderer, "tank1.png");
    Tank player2(600, 400);
    player2.loadTexture(gRenderer, "tank2.png");
    // Create bullets vector
    std::vector<Bullet> bullets;

    // Create the game map

    // Game loop
    SDL_Event e;
    bool quit = false;
    while (!quit) {

        // Handle events
        handleInput(e, player1, player2, bullets, gameMap); //Adjusted

        // Update game logic
        update(player1, player2, bullets, gameMap);

        // Render the scene
        render(gRenderer, player1, player2, bullets, gameMap);

        // Check for game over condition (e.g., one player's health reaches 0)
        if (player1.health <= 0 || player2.health <= 0) {
            quit = true;
            if (player1.health <= 0) {
                showVictoryScreen(gRenderer, "player1.png");
                }
                else if (player2.health <= 0) {
                showVictoryScreen(gRenderer, "player2.png");
                    }
            //std::cout << (player1.health <= 0 ? "Player 2 Wins!" : "Player 1 Wins!") << std::endl;
        }

        // Delay to cap frame rate (optional)
        SDL_Delay(10);  // Cap to approximately 100 FPS
    }
    // Clean up and quit
    close();

    return 0;
}
