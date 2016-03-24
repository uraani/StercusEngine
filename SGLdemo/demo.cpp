#include <SGL.h>
int main(int argc, char* argv[])
{
	SGL_Init();
	SGL_Window* window = SGL_CreateWindow("SGLDemo", 3, 3, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
	SGL_Mat4 matA =
	{
		1.0f,2.0f,3.0f,4.0f,
		5.0f,6.0f,7.0f,8.0f,
		9.0f,10.0f,11.0f,12.0f,
		13.0f,14.0f,15.0f,16.0f,
	};
	SGL_Mat4 matB =
	{
		17.0f,18.0f,19.0f,20.0f,
		21.0f,22.0f,23.0f,24.0f,
		25.0f,26.0f,27.0f,28.0f,
		29.0f,30.0f,31.0f,32.0f,
	};
	SDL_Log("[%f][%f][%f][%f]\n", matA.m00, matA.m01, matA.m02, matA.m03);
	SDL_Log("[%f][%f][%f][%f]\n", matA.m10, matA.m11, matA.m12, matA.m13);
	SDL_Log("[%f][%f][%f][%f]\n", matA.m20, matA.m21, matA.m22, matA.m23);
	SDL_Log("[%f][%f][%f][%f]\n", matA.m30, matA.m31, matA.m32, matA.m33);
	SDL_Log("started");
	matA = SM_Multiply(&matA, &matB);
	SDL_Log("[%f][%f][%f][%f]\n", matA.m00, matA.m01, matA.m02, matA.m03);
	SDL_Log("[%f][%f][%f][%f]\n", matA.m10, matA.m11, matA.m12, matA.m13);
	SDL_Log("[%f][%f][%f][%f]\n", matA.m20, matA.m21, matA.m22, matA.m23);
	SDL_Log("[%f][%f][%f][%f]\n", matA.m30, matA.m31, matA.m32, matA.m33);
	SDL_Event e = SDL_Event();
	for (; e.type != SDL_QUIT && e.type != SDL_KEYDOWN && e.type != SDL_FINGERDOWN; SDL_PollEvent(&e));
	SGL_DestroyWindow(window);
	SGL_Quit();
	return 0;
}