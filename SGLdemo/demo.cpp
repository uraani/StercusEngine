#include "SGL.h"
#include <string>
int main(int argc, char* argv[])
{
	SGL_Init();
	SGL_CreateWindow("SGLDemo", 3, 3, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
	SDL_Event e = SDL_Event();
	for (; e.type != SDL_QUIT && e.type != SDL_KEYDOWN; SDL_PollEvent(&e));
	SGL_Quit();
	return 0;
}