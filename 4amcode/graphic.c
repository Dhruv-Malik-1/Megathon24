#include <SDL2/SDL.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <string.h>
#include "networking.h"

#define COMMAND_SIZE 2
#define GRID_ROWS 21  // Adjust based on your server grid size
#define GRID_COLS 41  // Adjust based on your server grid size
#define BUFFER_SIZE 1024  // Adjust based on your needs

// Function to update the grid display based on the received buffer
void update_grid_display(SDL_Renderer *renderer, SDL_Texture *bgTexture, SDL_Texture *playerTexture, const char *buffer) {
    SDL_Rect cellRect = {0, 0, 44, 44}; // Size of each cell in the grid
    const char *line_start = buffer; // Start of each line
    int row = 0;

    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Draw the background
    SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

    // Loop through each character in the buffer until a newline is found
    while (*buffer != '\0') {
        if (*buffer == '\n') {
            // Move to the next line and reset the column
            row++;
            cellRect.x = 0; // Reset x position for the new row
            cellRect.y += 44; // Move down for the next row
            buffer++; // Move to the next character
            continue;
        }

        // Check character and decide what to draw
        if (*buffer == '#') {
            // Render player
            SDL_Rect playerRect = {cellRect.x, cellRect.y, 44, 44}; // Position of the player
            SDL_RenderCopy(renderer, playerTexture, NULL, &playerRect);
        } else {
            // Render empty space, could add a texture for the empty space if needed
        }

        // Move to the next cell
        cellRect.x += 44; // Move to the right by one cell
        buffer++; // Move to the next character
    }

    SDL_RenderPresent(renderer); // Update the screen
}

// Function to handle player movement input
void move(int sockfd) {
    SDL_Event event;
    char command[COMMAND_SIZE];

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return;
    }
    IMG_Init(IMG_INIT_PNG);

    SDL_Window *window = SDL_CreateWindow("Player Movement", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1804, 924, SDL_WINDOW_SHOWN); // 44 per cell
    if (!window) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    // Play the music using mpg123
    system("mpg123 -q music.mp3 &"); // Replace with your MP3 file path

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load textures
    SDL_Surface *bgSurface = IMG_Load("field.png");
    SDL_Texture *bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);

    SDL_Surface *playerSurface = IMG_Load("player.png");
    SDL_Texture *playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_FreeSurface(playerSurface);

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
                        break;
                    case SDLK_a:
                        command[0] = 'A';
                        break;
                    case SDLK_s:
                        command[0] = 'S';
                        break;
                    case SDLK_d:
                        command[0] = 'D';
                        break;
                    default:
                        continue; // Ignore other keys
                }
                command[1] = '\0'; // Null-terminate the string
                send_data(sockfd, command, strlen(command)); // Send command to the server

                // Receive the updated grid from the server
                char buffer[BUFFER_SIZE]; // Ensure this is large enough for your data
                ssize_t bytes_received = receive_data(sockfd, buffer, sizeof(buffer) - 1);
                if (bytes_received > 0) {
                    buffer[bytes_received] = '\0'; // Null-terminate the string
                    update_grid_display(renderer, bgTexture, playerTexture, buffer); // Update the grid display
                }
            }
        }
    }

    // Clean up resources
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
