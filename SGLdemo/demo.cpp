#include <SGL.h>
int main(int argc, char* argv[])
{
	SGL_Init();
	SGL_Window window = SGL_CreateWindow("SGLDemo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
	SGL_InitVulkan(&window);
	SGL_PrepareVulkan(&window.vkSwapChain);
	SGL_PrepareVertices(&window.vkSwapChain);
	SGL_PrepareUniformBuffers(&window.vkSwapChain);
	SGL_DataSelect("Demo_data");
	SDL_Surface* surf = SGL_DataLoadImage("chimppiVulkant0");
	window.vkSwapChain.tex.format = SGL_PixelFormatToVkFormat(surf->format->format);
	window.vkSwapChain.tex.w = surf->w;
	window.vkSwapChain.tex.h = surf->h;
	window.vkSwapChain.tex.mipLevels = 1;
	window.vkSwapChain.tex.surf = surf;
	SGL_PrepareTextureTest(&window.vkSwapChain, &window.vkSwapChain.tex);
	SGL_SetupDescriptorSetLayout(&window.vkSwapChain);
	SGL_PreparePipelines(&window.vkSwapChain);
	SGL_SetupDescriptorPool(&window.vkSwapChain);
	SGL_SetupDescriptorSet(&window.vkSwapChain);
	SGL_BuildCommandBuffers(&window.vkSwapChain);
	
	SGL_Vec4 v0 = { 1.1f,1.01f,2.001f,1.0f };
	SGL_Vec4 v1 = { 1.1f,1.01f,2.001f,1.0f };

	//SGL_RunGLTest(&window);
	SDL_Event e = SDL_Event();
	while (e.type != SDL_QUIT)
	{
		e = SDL_Event();
		for (; SDL_PollEvent(&e); e.type != SDL_QUIT && e.type != SDL_KEYDOWN && e.type != SDL_FINGERDOWN);
		{

		}
		SGL_Render(&window.vkSwapChain);
		//SGL_UpdateVulkanDemo(&window.vkContext);
	}
	//SGL_DestroyWindow(&window);
	SGL_Quit();
	return 0;
}