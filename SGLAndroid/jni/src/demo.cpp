//#include <jni.h>
#include <SGL.h>
int main(int argc, char* argv[])
{
	SGL_Init();
	SGL_Window* window = SGL_CreateWindow("SGLDemo", 1, 0, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
	SGL_RunGLTest(window);
	SDL_Event e = SDL_Event();
	for (; e.type != SDL_QUIT && e.type != SDL_KEYDOWN; SDL_PollEvent(&e));
	SGL_DestroyWindow(window);
	SGL_Quit();
	return 0;
}