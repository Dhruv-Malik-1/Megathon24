#include <SDL2/SDL.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <string.h>
#include "networking.h"

#define COMMAND_SIZE 2

void bufferToMap(char buffer[])
{
	struct node
	{
		SDL_Rect client;
		struct node *next;
	};

	struct node *prev = NULL;
	struct node *head = NULL;
	for (int i = 0; i < 21; i++)
	{
		for (int j = 0; j < 42; j++)
		{
			if (buffer[j + (42 * i)] == '#')
			{
				struct node *temp = (struct node *)malloc(sizeof(struct node));

				//temp->client = {44 * j, 44 * i, 44, 44};
				if (prev != NULL)
				{
					prev->next = temp;
					head = temp;
				}
				prev = temp;
			}
		}
		i++;
	}
	// return head;
}

// Function to handle player movement input
void move(int sockfd)
{
	SDL_Rect playerRect = {44, 44, 44, 44};

	SDL_Event event;
	char command[COMMAND_SIZE];

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
		return;
	}
	IMG_Init(IMG_INIT_PNG);

	SDL_Window *window = SDL_CreateWindow("Player Movement", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1804, 924, SDL_WINDOW_SHOWN); // 44 per cell
	if (!window)
	{
		fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
		SDL_Quit();
		return;
	}
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Surface *bgSurface = IMG_Load("field.png");
	SDL_Texture *bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);

	SDL_FreeSurface(bgSurface);
	SDL_Surface *playerSurface = IMG_Load("player.png");
	SDL_Texture *playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
	SDL_FreeSurface(playerSurface);

	// Main loop to handle events
	int running = 1;
	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = 0; // Exit the loop if the window is closed
			}
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_w:
					command[0] = 'W';
					command[1] = '\0'; // Null-terminate the string
					playerRect.y -= 44;
					break;
				case SDLK_a:
					command[0] = 'A';
					command[1] = '\0'; // Null-terminate the string
					playerRect.x -= 44;
					break;
				case SDLK_s:
					command[0] = 'S';
					command[1] = '\0'; // Null-terminate the string
					playerRect.y += 44;
					break;
				case SDLK_d:
					command[0] = 'D';
					command[1] = '\0'; // Null-terminate the string
					playerRect.x += 44;
					break;
				default:
					continue; // Ignore other keys
				}
				send_data(sockfd, command, strlen(command)); // Send command to the server
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

		SDL_RenderCopy(renderer, playerTexture, NULL, &playerRect); // render player
		SDL_RenderPresent(renderer);
		SDL_Delay(16);
	}
	SDL_DestroyTexture(bgTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
}
