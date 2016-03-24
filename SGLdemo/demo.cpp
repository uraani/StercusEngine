#include <SGL.h>
int main(int argc, char* argv[])
{
	SGL_Init();
	SGL_Window window = SGL_CreateWindow("SGLDemo", 4, 5, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
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