#include <SDL2/SDL.h>
#include "wheel.h"
#include <iostream>
#include <chrono>

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 900;
const char* WINDOW_TITLE = "Casino Money Wheel";

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Create the wheel
    MoneyWheel wheel(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Main loop
    bool running = true;
    SDL_Event event;
    auto lastTime = std::chrono::high_resolution_clock::now();

    std::cout << "Casino Money Wheel" << std::endl;
    std::cout << "Click anywhere on the screen to spin the wheel!" << std::endl;
    std::cout << "Segments:" << std::endl;
    std::cout << "  - 20 Red segments worth $1" << std::endl;
    std::cout << "  - 10 Blue segments worth $2" << std::endl;
    std::cout << "  - 5 Green segments worth $5" << std::endl;
    std::cout << "  - 3 Gold segments worth BONUS" << std::endl;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        wheel.spin();
                    }
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                    break;
            }
        }

        // Calculate delta time
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // Cap delta time to prevent large jumps
        if (deltaTime > 0.016f) {
            deltaTime = 0.016f;
        }

        // Update
        wheel.update(deltaTime);

        // Render
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderClear(renderer);

        // Draw background
        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &bgRect);

        // Draw wheel
        wheel.render(renderer);

        // Draw UI text
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        // Draw status
        std::string statusText = wheel.isSpinning() ? "SPINNING..." : "Click to spin";
        
        SDL_Color textColor = {0, 0, 0, 255};
        
        // Simple text rendering (note: SDL2 doesn't have built-in text, this is pseudo-code)
        // In a real application, you'd use SDL_ttf for proper text rendering
        
        if (!wheel.isSpinning()) {
            int winningValue = wheel.getWinningSegment();
            if (winningValue > 0) {
                // Display winning value
            }
        }

        // Present
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
