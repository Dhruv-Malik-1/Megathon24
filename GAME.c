#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	SDL_Window *win = SDL_CreateWindow("GAME",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1000, 1000, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	SDL_Surface *bgSurface = IMG_Load("field.png");
	SDL_Texture *bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);

	SDL_FreeSurface(bgSurface);

	SDL_Rect rect = { 800 / 2 - 25, 600 / 2 - 25, 50, 50 };
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
					rect.y -= 10;
				}else if(event.key.keysym.sym == SDLK_s){
					rect.y += 10;
				}else if(event.key.keysym.sym == SDLK_d){
					rect.x += 10;
				}else if(event.key.keysym.sym == SDLK_a){
					rect.x -= 10;
				}
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, bgTexture, NULL, NULL);
		
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
        SDL_RenderFillRect(renderer, &rect); // Draw the rectangle
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
