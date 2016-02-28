#include <SGL.h>
int main(int argc, char* argv[])
{
	SGL_Init();
	SGL_Window window = SGL_CreateWindow("SGLDemo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
	SGL_Vk_CreateContext(&window);
	SDL_Surface* img0 = IMG_Load("vulkant.tga");
	SDL_Surface* img1 = IMG_Load("vulkan.tga");
	SGL_DataCreate("Demo_data", 1488/*JK*/);
	SGL_DataSaveImage("chimppiVulkant0", img0);
	SGL_DataSaveImage("chimppiVulkan0", img1);
	//SGL_DataSelect("Demo_data");
	SGL_InitVulkanDemo(&window.vkContext);
	//SGL_Camera* cam = SGL_CreateCamera(&window.rContext, SGL_CAMERA_TYPE_ORTHO, SGL_CAMERA_GUI, 0.1f, 1.0f, 0.0f, 1.0f);
	//SGL_DataSaveImage("chimppiVulkan2", img);
	//SGL_DataSaveImage("chimppiVulkan3", img);
	
	SGL_Vec4 v0 = { 1.1f,1.01f,2.001f,1.0f };
	SGL_Vec4 v1 = { 1.1f,1.01f,2.001f,1.0f };
	//unsigned int time = SDL_GetTicks();
	//for (size_t i = 0; i < 100000000; i++)
	//{
	//	v1 = SM_Cross2(&v1, &v0);
	//}
	//SDL_Log("NONSIMD: %f s", ((float)(SDL_GetTicks() - time))*0.001f);
	//SDL_Log("[%f][%f][%f][%f]", v1.x, v1.y, v1.z, v1.w);
	//
	//time = SDL_GetTicks();
	//for (size_t i = 0; i < 100000000; i++)
	//{
	//	v0 = SM_Cross(&v0, &v1);
	//}
	//SDL_Log("SIMD: %f s", ((float)(SDL_GetTicks() - time))*0.001f);
	//SDL_Log("[%f][%f][%f][%f]", v0.x, v0.y, v0.z, v0.w);
	//SGL_Vec4 eye = { 8.0f,5.0f,6.0f,0.0f};
	//SGL_Vec4 center = { eye.x,eye.y,eye.z-1.0f,0.0f };
	//SGL_Vec4 up = { 0.0f,4.0f,0.0f,0.0f };
	//SGL_Mat4 m;
	//m = SM_LookAt(&eye, &center, &up);
	//SDL_Log("[%f][%f][%f][%f]", m.m00, m.m01, m.m02, m.m03);

	//SGL_RunGLTest(&window);
	SDL_Event e = SDL_Event();
	while (e.type != SDL_QUIT)
	{
		for (; SDL_PollEvent(&e); e.type != SDL_QUIT && e.type != SDL_KEYDOWN && e.type != SDL_FINGERDOWN);
		SGL_UpdateVulkanDemo(&window.vkContext);
	}
	//SGL_DestroyWindow(&window);
	SGL_Quit();
	return 0;
}