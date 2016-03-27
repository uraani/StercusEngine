#include <SGL.h>
int main(int argc, char* argv[])
{
	SGL_Init();
	SGL_Window window = SGL_CreateWindow("SGLDemo", 4, 5, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SGL_Camera* camera0 = SGL_CreateCamera(&window.rContext, SGL_CAMERA_TYPE_PERSPECTIVE, SGL_CAMERA_PERSPECTIVE, 0.1f, 2.0f, degreesToRadians(45.0f), 1.0f);
	SGL_Mat3 mat = { 45.0f, 10.0f, 22.0f, 0.0f, 4.0f, 5.0f, 6.0f, 0.0f, 8.0f, 9.0f, 10.0f, 0.0f };
	SGL_Vec2 vec[] = 
	{
		{ 1.0f,2.0f },
		{ 2.0f,3.0f },
		{ 3.0f,4.0f },
		{ 4.0f,5.0f },
	};
	SGL_Vec4 vx = { 1.0f,2.0f,3.0f,4.0f };
	SGL_Vec4 vy = { 2.0f,3.0f,4.0f,5.0f };
	SGL_Vec2 res[4];
	U32 ticks = SDL_GetTicks();
	//test0
	SGL_Vec4 texSize = { 256.0f, 256.0f,  256.0f, 256.0f};
	SGL_TexRegion reg = { 64.0f,64.0f,128.0f,128.0f };
	SGL_Vec2 uv0 = { reg.offset.x / texSize.x,reg.offset.y / texSize.y };
	SGL_Vec2 uv1 = { uv0.x + reg.size.x / texSize.x, uv0.y + reg.size.y / texSize.y };
	SM_CalculateUVs(&reg, &texSize, 0, 0);
	SDL_Log("%i ticks", SDL_GetTicks() - ticks);
	ticks = SDL_GetTicks();
	//test1

	SDL_Log("%i ticks", SDL_GetTicks() - ticks);
	SGL_DataSelect("Demo_data");
	SGL_RunGLTest(&window);
	//SDL_Surface* surf = SGL_DataLoadImage("chimppiVulkant0");
	//window.vkSwapChain.tex.format = SGL_PixelFormatToVkFormat(surf->format->format);
	//window.vkSwapChain.tex.w = surf->w;
	//window.vkSwapChain.tex.h = surf->h;
	//window.vkSwapChain.tex.mipLevels = 1;
	//window.vkSwapChain.tex.surf = surf;
	SDL_Event e = SDL_Event();
	while (e.type != SDL_QUIT)
	{
		e = SDL_Event();
		for (; SDL_PollEvent(&e); e.type != SDL_QUIT && e.type != SDL_KEYDOWN && e.type != SDL_FINGERDOWN);
		{
		}
		//SDL_SysWMinfo info;
		//SDL_VERSION(&info.version);
		//SDL_GetWindowWMInfo(window.handle, &info);
		//RedrawWindow(info.info.win.window, NULL, NULL, RDW_INTERNALPAINT);
		//SGL_UpdateVulkanDemo(&window.vkContext);
	}
	SGL_DestroyWindow(&window);
	SGL_Quit();
	return EXIT_SUCCESS;
}