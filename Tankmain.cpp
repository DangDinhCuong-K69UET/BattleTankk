#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include<SDL_mixer.h>
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
// Hàm khởi tạo và phát nhạc nền
void playBackgroundMusic(const char* musicPath) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "Error initializing SDL_mixer: " << Mix_GetError() << std::endl;
        return;
    }

    Mix_Music* bgMusic = Mix_LoadMUS(musicPath);
    if (!bgMusic) {
        std::cout << "Failed to load background music: " << Mix_GetError() << std::endl;
        return;
    }

    Mix_PlayMusic(bgMusic, -1); // Phát nhạc lặp vô hạn
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
// Trả về: -1 (lỗi), 1 (chơi lại), 2 (thoát)
int showVictoryScreen(SDL_Renderer* renderer, const char* imagePath) {
    SDL_Texture* victoryTexture = NULL;
    SDL_Surface* victorySurface = IMG_Load(imagePath);

    // Kiểm tra xem có tải được ảnh không
    if (!victorySurface) {
        printf("Không thể tải ảnh chiến thắng: %s\n", IMG_GetError());
        return -1;  // Trường hợp lỗi
    }

    // Tạo texture từ surface
    victoryTexture = SDL_CreateTextureFromSurface(renderer, victorySurface);
    SDL_FreeSurface(victorySurface);  // Giải phóng surface sau khi dùng xong

    // Kiểm tra xem có tạo được texture không
    if (!victoryTexture) {
        printf("Không thể tạo texture: %s\n", SDL_GetError());
        return -1;  // Trường hợp lỗi
    }

    // Xóa renderer và vẽ texture lên màn hình
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, victoryTexture, NULL, NULL);
    SDL_RenderPresent(renderer);  // Hiển thị lên màn hình

    SDL_Event e;
    bool running = true;
    int result = 0;  // 0: tiếp tục chờ, 1: chơi lại, 2: thoát

    // Vòng lặp sự kiện
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {  // Khi đóng cửa sổ
                result = 2;  // Thoát
                running = false;
            }
            else if (e.type == SDL_KEYDOWN) {  // Khi nhấn phím
                switch (e.key.keysym.sym) {
                    case SDLK_r:    // Nhấn R để chơi lại
                        result = 1;
                        running = false;
                        break;
                    case SDLK_q:    // Nhấn Q để thoát
                        result = 2;
                        running = false;
                        break;
                }
            }
        }
    }

    // Giải phóng texture trước khi thoát
    SDL_DestroyTexture(victoryTexture);
    return result;  // Trả về lựa chọn của người chơi
}
void showHowToPlayScreen(SDL_Renderer* renderer) {
    if (!renderer) return; // Kiểm tra renderer hợp lệ

    SDL_Texture* howToPlayScreen = loadTexture("huongdan.png", renderer); // Load ảnh hướng dẫn
    if (!howToPlayScreen) {
        std::cerr << "Failed to load how-to-play screen texture!" << std::endl;
        return;
    }

    SDL_Event event;
    bool waiting = true;

    while (waiting) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, howToPlayScreen, NULL, NULL); // Hiển thị toàn màn hình
        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) exit(0); // Thoát game nếu bấm nút tắt
            if (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) waiting = false; // Bấm phím hoặc chuột để tiếp tục
        }

        SDL_Delay(5); // Tối ưu CPU
    }

    SDL_DestroyTexture(howToPlayScreen);
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
             if (checkCollision(bullet.x, bullet.y, bullet.x / 32, bullet.y / 32,
                               gameMap.base1.x * 32, gameMap.base1.y * 32, 64, 64)) {
                gameMap.base1.health -= 10; // Trụ mất 10 máu mỗi lần trúng đạn
                bullet.isAlive = false; // Hủy đạn

                if (gameMap.base1.health <= 0) {
                    showVictoryScreen(gRenderer, "player1.png");
                      close();
                    // Có thể thêm logic kết thúc game ở đây
                }
            }

            // Kiểm tra nếu đạn bắn vào trụ 2
            if (checkCollision(bullet.x, bullet.y,bullet.x / 32, bullet.y / 32,
                               gameMap.base2.x * 32, gameMap.base2.y * 32, 64, 64)) {
                gameMap.base2.health -= 10;
                bullet.isAlive = false;

                if (gameMap.base2.health <= 0) {
                   showVictoryScreen(gRenderer, "player1.png");
                     close();
                    // Có thể thêm logic kết thúc game ở đây
                }
            }
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

    gameMap.checkTankHealthCollision(player1);
    gameMap.checkTankHealthCollision(player2);
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



void render(SDL_Renderer* renderer, Tank& player1, Tank& player2, std::vector<Bullet>& bullets, GameMap& gameMap) {
    SDL_Texture* wallTex = loadTexture("stone.png", renderer);
    gameMap.setWallTexture(wallTex);
    SDL_Texture* baseTexture = loadTexture("base.png", renderer);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // Black background
    gameMap.render(gRenderer, baseTexture);
    player1.render(gRenderer) ;
    player2.render(gRenderer) ;
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


int main(int argc, char* args[]) {
    // Khởi tạo SDL video
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cout << "Không thể khởi tạo SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Khởi tạo SDL_image cho PNG
    IMG_Init(IMG_INIT_PNG);

    // Tạo renderer (chuyển lên trước để dùng trong các hàm khác)
    gWindow = SDL_CreateWindow("Tank Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!gRenderer || !gWindow) {
        std::cerr << "Khởi tạo renderer hoặc window thất bại!" << std::endl;
        return 1;
    }

    // Gọi hàm phát nhạc nền
    playBackgroundMusic("nhacnen.mp3");
    Mix_VolumeMusic(7); // Đặt âm lượng nhạc nền

    // Hiển thị màn hình bắt đầu và hướng dẫn
    showStartScreen(gRenderer);
    showHowToPlayScreen(gRenderer);

    // Tạo bản đồ game
    GameMap gameMap(25, 19); // Bản đồ 25x19 ô
    // Tạo tank cho người chơi
    Tank player1(100, 140);
    player1.loadTexture(gRenderer, "tank1.png");
    Tank player2(700, 445);
    player2.loadTexture(gRenderer, "tank2.png");
    // Tạo vector chứa đạn
    std::vector<Bullet> bullets;

    // Thời gian sinh vật phẩm
    Uint32 lastSpawnTime = SDL_GetTicks();
    const Uint32 spawnInterval = 5000; // Sinh vật phẩm mỗi 5 giây

    // Vòng lặp game chính
    SDL_Event e;
    bool quit = false;
    bool gameRunning = true;
    while (!quit) {
        Uint32 currentTime = SDL_GetTicks();

        if (gameRunning) {
            // Sinh vật phẩm định kỳ
            if (currentTime - lastSpawnTime >= spawnInterval) {
                gameMap.spawnHealthPack();
                lastSpawnTime = currentTime;
            }

            // Xử lý input
            handleInput(e, player1, player2, bullets, gameMap);

            // Cập nhật logic game
            update(player1, player2, bullets, gameMap);

            // Vẽ cảnh game
            render(gRenderer, player1, player2, bullets, gameMap);

            // Kiểm tra điều kiện kết thúc game
            if (player1.health <= 0 || player2.health <= 0) {
                gameRunning = false;
                int victoryResult;
                if (player1.health <= 0) {
                    victoryResult = showVictoryScreen(gRenderer, "player2.png"); // Player 2 thắng
                } else {
                    victoryResult = showVictoryScreen(gRenderer, "player1.png"); // Player 1 thắng
                }

                // Xử lý lựa chọn sau màn hình chiến thắng
                if (victoryResult == 1) { // Nhấn R để chơi lại
                    // Reset game
                    player1 = Tank(100, 140);
                    player1.loadTexture(gRenderer, "tank1.png");
                    player2 = Tank(700, 445);
                    player2.loadTexture(gRenderer, "tank2.png");
                    bullets.clear();
                    gameMap = GameMap(25, 19);
                    gameRunning = true;
                } else if (victoryResult == 2) { // Nhấn Q để thoát
                    quit = true;
                }
            }

            // Giới hạn FPS
            SDL_Delay(10); // Khoảng 100 FPS
        }
    }

    // Dọn dẹp và thoát
    close();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
