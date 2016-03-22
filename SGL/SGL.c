#include "SGL.h"
int SGL_Init(void)
{
	int error = SDL_Init(SDL_INIT_EVERYTHING);
	if(error != 0)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
		return error;
	}
	error = IMG_Init(IMG_INIT_PNG);
	if (error == 0)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
		return error;
	}
	SGL_InitDataSystem();
	return 0;
}
SGL_Window SGL_CreateWindow(const char* title, int x, int y, int w, int h, Uint32 SDLflags)
{
#if defined(_WINDOWS)
	SDL_DisplayMode targetMode, closestMode;
	targetMode.w = w;
	targetMode.h = h;
	targetMode.format = 0;  // don't care
	targetMode.refresh_rate = 0; // don't care
	targetMode.driverdata = 0; // initialize to 0
	SDL_GetClosestDisplayMode(0, &targetMode, &closestMode);
	SGL_Window window;
	window.handle = SDL_CreateWindow
		(
			title,
			x, y,
			closestMode.w, closestMode.h,
			SDLflags
			);
#elif defined(ANDROID)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GLMajorVersion > 3 ? GLMajorVersion : 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SGL_Window* window = SDL_malloc(sizeof(SGL_Window));
	SDL_DisplayMode displayMode;
	if (SDL_GetCurrentDisplayMode(0, &displayMode))
	{
		return NULL;
	}
	window->window = SDL_CreateWindow
		(
			title,
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			displayMode.w, displayMode.h,
			SDLflags
			);
	window->context = SDL_GL_CreateContext(window->handle);
#endif
	SGL_SetWindowIcon(&window, NULL);
	SDL_GetWindowSize(window.handle, &window.windowSize.x, &window.windowSize.y);
	window.windowHalfSizef.x = (float)window.windowSize.x*0.5f;
	window.windowHalfSizef.y = (float)window.windowSize.y*0.5f;
	return window;
}

/*
SGL_Window SGL_CreateWindow(const char* title, int GLMajorVersion, int GLMinorVersion, int x, int y, int w, int h, Uint32 SDLflags)
{
#if defined(_WINDOWS)
	SDL_DisplayMode targetMode;
	if (SDL_GetCurrentDisplayMode(0, &targetMode))
	{
		SGL_Window window;
		window.glContext = NULL;
		window.handle = NULL;
		return window;
	}
	SDL_DisplayMode closestMode;
	SDL_GetClosestDisplayMode(0, &targetMode, &closestMode);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GLMajorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GLMinorVersion);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SGL_Window window;
	window.handle = SDL_CreateWindow
		(
			title,
			x, y,
			w, h,
			SDLflags
		);
	window.glContext = SDL_GL_CreateContext(window.handle);
	glewExperimental = GL_TRUE;
	glewInit();
#elif defined(ANDROID)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GLMajorVersion > 3 ? GLMajorVersion : 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SGL_Window* window = SDL_malloc(sizeof(SGL_Window));
	SDL_DisplayMode displayMode;
	if (SDL_GetCurrentDisplayMode(0, &displayMode))
	{
		return NULL;
	}
	window->window = SDL_CreateWindow
		(
			title,
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			displayMode.w, displayMode.h,
			SDLflags
			);
	window->context = SDL_GL_CreateContext(window->window);
#endif
	SDL_Log("----------------------------------------------------------------\n");
	SDL_Log("Graphics Successfully Initialized For Window: %s\n", title);
	SDL_Log("OpenGL Info\n");
	SDL_Log("  Version: %s\n", glGetString(GL_VERSION));
	SDL_Log("   Vendor: %s\n", glGetString(GL_VENDOR));
	SDL_Log(" Renderer: %s\n", glGetString(GL_RENDERER));
	SDL_Log("  Shading: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	SDL_Log("----------------------------------------------------------------\n");
	SGL_SetWindowIcon(&window, NULL);
	SDL_GetWindowSize(window.handle, &window.rContext.windowSize.x, &window.rContext.windowSize.y);
	window.rContext.windowHalfSizef.x = (float)window.rContext.windowSize.x*0.5f;
	window.rContext.windowHalfSizef.y = (float)window.rContext.windowSize.y*0.5f;
	for (size_t i = 0; i < SGL_CAMERA_COUNT; i++)
	{
		window.rContext.cameras[i].camType = 0;
	}
	return window;
}*/

void SGL_SetWindowIcon(const SGL_Window* window, void * pixels)
{
	SDL_Surface* surface;
	if (pixels == NULL)
	{
		unsigned char defaultPixels[] = { 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 52, 75, 75, 75, 127, 127, 127, 194, 194, 194, 127, 127, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 255, 255, 0, 255, 255, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 75, 75, 75, 131, 131, 131, 75, 75, 75, 127, 127, 127, 75, 75, 75, 0, 0, 0, 0, 0, 0, 255, 0, 255, 255, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 75, 75, 75, 193, 193, 193, 25, 25, 25, 0, 0, 0, 0, 0, 0, 255, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 27, 27, 0, 0, 0, 52, 52, 52, 131, 131, 131, 0, 0, 0, 52, 52, 52, 0, 0, 0, 77, 77, 77, 38, 38, 38, 17, 17, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 72, 72, 72, 0, 0, 0, 52, 52, 52, 168, 168, 168, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 38, 77, 77, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40, 40, 40, 21, 21, 21, 0, 0, 0, 0, 0, 0, 99, 99, 99, 75, 75, 75, 194, 194, 194, 112, 112, 112, 75, 75, 75, 0, 0, 0, 0, 0, 0, 54, 54, 54, 131, 131, 131, 0, 0, 0, 0, 0, 0, 57, 57, 57, 0, 0, 0, 87, 87, 87, 87, 87, 87, 97, 97, 97, 50, 50, 50, 147, 147, 147, 75, 75, 75, 212, 212, 212, 75, 75, 75, 129, 129, 129, 0, 0, 0, 54, 54, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 110, 110, 110, 14, 14, 14, 0, 0, 0, 44, 44, 44, 65, 65, 65, 50, 50, 50, 84, 84, 84, 161, 161, 161, 210, 210, 210, 166, 166, 166, 75, 75, 75, 38, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 255, 0, 0, 0, 75, 75, 75, 21, 21, 21, 0, 0, 0, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 255, 255, 0, 255, 0, 0, 0, 40, 40, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 39, 39, 39, 39, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 77, 77, 77, 131, 131, 131, 131, 131, 131, 131, 131, 131, 77, 77, 77, 0, 0, 0, 0, 0, 0, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255 };
		surface = SDL_CreateRGBSurfaceFrom(defaultPixels, 16, 16, 24, 3 * 16, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	}
	else
	{
		surface = SDL_CreateRGBSurfaceFrom(pixels, 16, 16, 24, 3 * 16, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	}
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255));
	SDL_SetWindowIcon(window->handle, surface);
	SDL_FreeSurface(surface);
}

void SGL_DestroyWindow(const SGL_Window* window)
{
	//SDL_GL_DeleteContext(window->glContext);
	SDL_DestroyWindow(window->handle);
}

void SGL_ConvertPNGToIconArray(const char * imagePath, const char * fileName)
{
	SDL_RWops* rwop = SDL_RWFromFile(imagePath, "rb");
	if (rwop)
	{
		SDL_Surface* surf = IMG_LoadPNG_RW(rwop);
		SDL_RWclose(rwop);
		SDL_RWops *rw = SDL_RWFromFile(fileName, "w");
		if (rw)
		{
			size_t len = surf->h*surf->w*surf->format->BytesPerPixel;
			const char start[] = "unsigned char icon[] = { ";
			SDL_RWwrite(rw, start, 1, SDL_strlen(start));
			size_t i;
			for (i = 0; i < len; i++)
			{
				char buffer[5];
				SDL_snprintf(buffer, 5, "%u", *(((unsigned char*)surf->pixels) + i));
				//sprintf(buffer, "%u", *(((unsigned char*)surf->pixels) + i));
				size_t l;
				for (l = 1; l < 4; l++)
				{
					if (buffer[l] == 0 && i + 1 < len)
					{
						buffer[l] = ',';
						buffer[l + 1] = ' ';
						SDL_RWwrite(rw, buffer, 1, l + 2);
						break;
					}
				}
			}
			char buffer[7];
			SDL_snprintf(buffer, 7,"%u", *(((unsigned char*)surf->pixels) + (len - 1)));
			size_t l;
			for (l = 1; l < 4; l++)
			{
				if (buffer[l] == 0)
				{
					buffer[l] = ' ';
					buffer[l + 1] = '}';
					buffer[l + 2] = ';';
					SDL_RWwrite(rw, buffer, 1, l + 3);
					break;
				}
			}
			SDL_RWclose(rw);
		}
	}
}
/*
void SGL_RunGLTest(const SGL_Window* window)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
}*/

void SGL_Quit(void)
{
	IMG_Quit();
	SDL_Quit();
}
