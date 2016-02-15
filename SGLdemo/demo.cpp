#include <SGL.h>
int main(int argc, char* argv[])
{
	SGL_Init();
	SGL_Window window = SGL_CreateWindow("SGLDemo", 3, 3, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
	SGL_Camera* cam = SGL_CreateCamera(&window.rContext, SGL_CAMERA_TYPE_ORTHO, SGL_CAMERA_GUI, 0.1f, 1.0f, 0.0f, 1.0f);
	SDL_Event e = SDL_Event();
	for (; e.type != SDL_QUIT && e.type != SDL_KEYDOWN && e.type != SDL_FINGERDOWN; SDL_PollEvent(&e));
	SGL_DestroyWindow(&window);
	SGL_Quit();
	return 0;
}