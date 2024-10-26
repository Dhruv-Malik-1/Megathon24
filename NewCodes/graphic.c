#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include "graphic.h"

char key;

int move()
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	SDL_Window *win = SDL_CreateWindow("GAME",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1000, 1000, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	SDL_Surface *bgSurface = IMG_Load("field.png");
	SDL_Texture *bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);

	SDL_FreeSurface(bgSurface);
	
	SDL_Surface *playerSurface = IMG_Load("player.png");
    SDL_Texture *playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_FreeSurface(playerSurface);

    SDL_Rect playerRect = { 1000 / 2 - 25, 1000 / 2 - 25, 100, 100 };

	SDL_Event event;

	int go = 1;
	while(go){
		while(SDL_PollEvent(&event) != 0){
			if(event.type == SDL_QUIT){
				go = 0;
				break;
			}
			if(event.type == SDL_KEYDOWN){
				if(event.key.keysym.sym == SDLK_w){
					playerRect.y -= 20;
					key = 'W';
				}else if(event.key.keysym.sym == SDLK_s){
					playerRect.y += 20;
					key = 'S';
				}else if(event.key.keysym.sym == SDLK_d){
					playerRect.x += 20;
					key = 'D';
				}else if(event.key.keysym.sym == SDLK_a){
					playerRect.x -= 20;
					key = 'A';
				}
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, bgTexture, NULL, NULL);
		
		SDL_RenderCopy(renderer, playerTexture, NULL, &playerRect);
        SDL_RenderPresent(renderer);
		SDL_Delay(16);
	}
	
	SDL_DestroyTexture(bgTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
	return 0;
}
