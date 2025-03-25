#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
SDL_Texture* loadTexture(const std::string& filePath);
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

