#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include "networking.h"
#include "graphic.h"

#define COMMAND_SIZE 2

// Function to handle player movement input
void move(int sockfd) {
    SDL_Event event;
    char command[COMMAND_SIZE];
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return;
    }

    SDL_Window *window = SDL_CreateWindow("Player Movement", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    // Main loop to handle events
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0; // Exit the loop if the window is closed
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        command[0] = 'W';
                        command[1] = '\0'; // Null-terminate the string
                        break;
                    case SDLK_a:
                        command[0] = 'A';
                        command[1] = '\0'; // Null-terminate the string
                        break;
                    case SDLK_s:
                        command[0] = 'S';
                        command[1] = '\0'; // Null-terminate the string
                        break;
                    case SDLK_d:
                        command[0] = 'D';
                        command[1] = '\0'; // Null-terminate the string
                        break;
                    default:
                        continue; // Ignore other keys
                }
                send_data(sockfd, command, strlen(command)); // Send command to the server
            }
        }
    }

    // Cleanup
    SDL_DestroyWindow(window);
    SDL_Quit();
}
