#include "SGL.h"
int main(int argc, char* argv[])
{
	SGL_Init();
	IMG_Init(IMG_INIT_PNG);
	SDL_RWops* rwop = SDL_RWFromFile("ULOSTECH_16.h", "rb");
	if (rwop)
	{
		SDL_Surface* surf = IMG_LoadPNG_RW(rwop);
		SDL_RWclose(rwop);
		SDL_RWops *rw = SDL_RWFromFile("hello.txt", "w");
		if (rw != NULL)
		{
			size_t len = surf->h*surf->w*surf->format->BytesPerPixel;
			if (SDL_RWwrite(rw, surf->pixels, 1, len) != len) {
				int fail = 0;
			}
			else {
				int success = 0;
			}
			SDL_RWclose(rw);
		}
	}
	IMG_Quit();
	SGL_Quit();
	return 0;
}