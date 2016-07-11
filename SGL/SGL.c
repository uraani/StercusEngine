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
inline void SGL_InitShadowMapper(SGL_RenderContext* rContext)
{
	glGenVertexArrays(1, &rContext->shadowVAO.handle);
	glGenBuffers(1, &rContext->shadowVAO.VBO);
	rContext->shaders[SGL_SHADER_SHADOWMAP].bindFunction(rContext->shadowVAO.handle, rContext->shadowVAO.VBO, rContext->shaders[SGL_SHADER_SHADOWMAP].handle);
}
inline void SGL_InitLightVAO(SGL_RenderContext* rContext)
{
	glGenVertexArrays(1, &rContext->lightVAO.handle);
	glGenBuffers(1, &rContext->lightVAO.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, rContext->lightVAO.VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	rContext->shaders[SGL_SHADER_LIGHT].bindFunction(rContext->lightVAO.handle, rContext->lightVAO.VBO, rContext->shaders[SGL_SHADER_LIGHT].handle);
}
SGL_Window SGL_CreateWindow(const char* title, I32 GLMajorVersion, I32 GLMinorVersion, U32 bufferCount, I32 x, I32 y, I32 w, I32 h, Uint32 SDLflags)
{
#if defined(_WINDOWS)
	SGL_Window window;
	SDL_memset(&window, 0, sizeof(SGL_Window));
	SDL_DisplayMode targetMode;
	if (SDL_GetCurrentDisplayMode(0, &targetMode))
	{
		return window;
	}
	SDL_DisplayMode closestMode;
	SDL_GetClosestDisplayMode(0, &targetMode, &closestMode);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GLMajorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GLMinorVersion);
	
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2); //2-8

	window.handle = SDL_CreateWindow
		(
			title,
			x, y,
			w, h,
			SDLflags
		);
	window.rContext.glHandle = SDL_GL_CreateContext(window.handle);
	glewExperimental = GL_TRUE;
	glewInit();
	glGetError();
#elif defined(ANDROID)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GLMajorVersion > 3 ? GLMajorVersion : 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SGL_Window window;
	SDL_DisplayMode displayMode;
	if (SDL_GetCurrentDisplayMode(0, &displayMode))
	{
		return NULL;
	}
	window.window = SDL_CreateWindow
		(
			title,
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			displayMode.w, displayMode.h,
			SDLflags
			);
	window.context = SDL_GL_CreateContext(window->window);
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
	window.rContext.windowHalfSizef.x = (F32)window.rContext.windowSize.x*0.5f;
	window.rContext.windowHalfSizef.y = (F32)window.rContext.windowSize.y*0.5f;
	SDL_assert(bufferCount <= SGL_BUFFER_COUNT_MAX);
	window.rContext.bufferCount = bufferCount;
	//glEnable(GL_MULTISAMPLE);
	SGL_CHECK_GL_ERROR;
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(UINT_MAX);
	glEnable(GL_BLEND);
	SGL_CHECK_GL_ERROR;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//SGL_CHECK_GL_ERROR;
	//glEnable(GL_POINT_SPRITE);
	SGL_CHECK_GL_ERROR;
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	SGL_CHECK_GL_ERROR;
	glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
	SGL_CHECK_GL_ERROR;
	SDL_GL_SetSwapInterval(0);
	for (size_t i = 0; i < SGL_CAMERA_COUNT; i++)
	{
		window.rContext.cameras[i].camType = 0;
	}
	SGL_LoadShaders(&window.rContext);
	SGL_InitShadowMapper(&window.rContext);
	SGL_InitLightVAO(&window.rContext);
	return window;
}

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
void SGL_RunGLTest(const SGL_Window* window)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(500);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(500);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(500);
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(500);
	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(500);
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(500);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(500);
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(500);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
}
inline SGL_UpdateCameras(SGL_RenderContext* rContext)
{
	for (U32 i = 0; i < SGL_CAMERA_COUNT; i++)
	{
		switch (rContext->cameras[i].camType)
		{
		case SGL_CAMERA_TYPE_ORTHO:
		{
			const float left = rContext->windowHalfSizef.x * -rContext->cameras[i].scale;
			const float right = rContext->windowHalfSizef.x * rContext->cameras[i].scale;
			const float bottom = rContext->windowHalfSizef.y * -rContext->cameras[i].scale;
			const float top = rContext->windowHalfSizef.y * rContext->cameras[i].scale;
			//rContext->cameras[i].vPMatrix.m00 = 2.0f / (right - left);
			//rContext->cameras[i].vPMatrix.m11 = 2.0f / (top - bottom);
			//rContext->cameras[i].vPMatrix.m22 = -1.0f;
			////rContext->cameras[i].vPMatrix.m22 = -2.0f / (rContext->cameras[i].farPlane - rContext->cameras[i].nearPlane);
			//rContext->cameras[i].vPMatrix.m30 = -(right + left) / (right - left);
			//rContext->cameras[i].vPMatrix.m31 = -(top + bottom) / (top - bottom);
			//rContext->cameras[i].vPMatrix.m32 = 0.0f;

			rContext->cameras[i].vPMatrix.m00 = 2.0f / (right - left);
			rContext->cameras[i].vPMatrix.m01 = 0.0f;
			rContext->cameras[i].vPMatrix.m02 = 0.0f;
			rContext->cameras[i].vPMatrix.m03 = 0.0f;
			
			rContext->cameras[i].vPMatrix.m10 = 0.0f;
			rContext->cameras[i].vPMatrix.m11 = 2.0f / (top - bottom);
			rContext->cameras[i].vPMatrix.m12 = 0.0f;
			rContext->cameras[i].vPMatrix.m13 = 0.0f;
			
			rContext->cameras[i].vPMatrix.m20 = 0.0f;
			rContext->cameras[i].vPMatrix.m21 = 0.0f;
			rContext->cameras[i].vPMatrix.m22 = -1.0f;
			//rContext->cameras[i].vPMatrix.m22 = -2.0f / (rContext->cameras[i].farPlane - rContext->cameras[i].nearPlane);
			rContext->cameras[i].vPMatrix.m23 = 0.0f;
			
			rContext->cameras[i].vPMatrix.m30 = -(right + left) / (right - left);
			rContext->cameras[i].vPMatrix.m31 = -(top + bottom) / (top - bottom);
			rContext->cameras[i].vPMatrix.m32 = 0.0f;
			rContext->cameras[i].vPMatrix.m33 = 1.0f;

			//rContext->cameras[i].vPMatrix.m32 = -(rContext->cameras[i].farPlane + rContext->cameras[i].nearPlane) / (rContext->cameras[i].farPlane - rContext->cameras[i].nearPlane);
			SGL_Mat4 rot = SM_QToM4(&rContext->cameras[i].rotation);
			rContext->cameras[i].vPMatrix = SM_M4Multiply(&rot, &rContext->cameras[i].vPMatrix);
			//SGL_Vec4 forward = { 0.0f, 0.0f, 1.0f, 0.0f };
			//rContext->cameras[i].vPMatrix = SM_M4Rotate(&rContext->cameras[i].vPMatrix, rContext->cameras[i].rotation.z, &forward);
			break;
		}
		case SGL_CAMERA_TYPE_ORTHO_FORCED_SIZE:
		{
			const float left = rContext->cameras[i].forcedSize.x * -rContext->cameras[i].scale;
			const float right = rContext->cameras[i].forcedSize.x * rContext->cameras[i].scale;
			const float bottom = rContext->cameras[i].forcedSize.y * -rContext->cameras[i].scale;
			const float top = rContext->cameras[i].forcedSize.y * rContext->cameras[i].scale;
			//rContext->cameras[i].vPMatrix.m00 = 2.0f / (right - left);
			//rContext->cameras[i].vPMatrix.m11 = 2.0f / (top - bottom);
			//rContext->cameras[i].vPMatrix.m22 = -1.0f;
			////rContext->cameras[i].vPMatrix.m22 = -2.0f / (rContext->cameras[i].farPlane - rContext->cameras[i].nearPlane);
			//rContext->cameras[i].vPMatrix.m30 = -(right + left) / (right - left);
			//rContext->cameras[i].vPMatrix.m31 = -(top + bottom) / (top - bottom);
			//rContext->cameras[i].vPMatrix.m32 = 0.0f;

			rContext->cameras[i].vPMatrix.m00 = 2.0f / (right - left);
			rContext->cameras[i].vPMatrix.m01 = 0.0f;
			rContext->cameras[i].vPMatrix.m02 = 0.0f;
			rContext->cameras[i].vPMatrix.m03 = 0.0f;

			rContext->cameras[i].vPMatrix.m10 = 0.0f;
			rContext->cameras[i].vPMatrix.m11 = 2.0f / (top - bottom);
			rContext->cameras[i].vPMatrix.m12 = 0.0f;
			rContext->cameras[i].vPMatrix.m13 = 0.0f;

			rContext->cameras[i].vPMatrix.m20 = 0.0f;
			rContext->cameras[i].vPMatrix.m21 = 0.0f;
			rContext->cameras[i].vPMatrix.m22 = -1.0f;
			//rContext->cameras[i].vPMatrix.m22 = -2.0f / (rContext->cameras[i].farPlane - rContext->cameras[i].nearPlane);
			rContext->cameras[i].vPMatrix.m23 = 0.0f;

			rContext->cameras[i].vPMatrix.m30 = -(right + left) / (right - left);
			rContext->cameras[i].vPMatrix.m31 = -(top + bottom) / (top - bottom);
			rContext->cameras[i].vPMatrix.m32 = 0.0f;
			rContext->cameras[i].vPMatrix.m33 = 1.0f;

			//rContext->cameras[i].vPMatrix.m32 = -(rContext->cameras[i].farPlane + rContext->cameras[i].nearPlane) / (rContext->cameras[i].farPlane - rContext->cameras[i].nearPlane);
			SGL_Mat4 rot = SM_QToM4(&rContext->cameras[i].rotation);
			rContext->cameras[i].vPMatrix = SM_M4Multiply(&rot, &rContext->cameras[i].vPMatrix);
			//SGL_Vec4 forward = { 0.0f, 0.0f, 1.0f, 0.0f };
			//rContext->cameras[i].vPMatrix = SM_M4Rotate(&rContext->cameras[i].vPMatrix, rContext->cameras[i].rotation.z, &forward);
			break;
		}
		case SGL_CAMERA_TYPE_PERSPECTIVE:
		{
			const float aspect = rContext->windowHalfSizef.x / rContext->windowHalfSizef.y;
			const float tanHalfFovy = SDL_tanf(rContext->cameras[i].FOWY / 2.0f);
			rContext->cameras[i].vPMatrix.m00 = 1.0f / (aspect * tanHalfFovy);
			rContext->cameras[i].vPMatrix.m11 = 1.0f / (tanHalfFovy);
			rContext->cameras[i].vPMatrix.m22 = -(rContext->cameras[i].farPlane + rContext->cameras[i].nearPlane) / (rContext->cameras[i].farPlane - rContext->cameras[i].nearPlane);
			rContext->cameras[i].vPMatrix.m23 = -1.0f;
			rContext->cameras[i].vPMatrix.m32 = -2.0f * rContext->cameras[i].farPlane * rContext->cameras[i].nearPlane / (rContext->cameras[i].farPlane - rContext->cameras[i].nearPlane);
			break;
		}
		default:
		{
			break;
		}
		}
		//rotate
		//SM_Rotate(,)

		SGL_Vec4 back = { 0.0f, 0.0f, -1.0f, 0.0f };
		SGL_Vec4 up = { 0.0f, 1.0f, 0.0f, 0.0f };
		SGL_Vec4 center = SM_V4Add(&rContext->cameras[i].position, &back);
		SGL_Mat4 look = SM_LookAt(&rContext->cameras[i].position, &center, &up);
		rContext->cameras[i].vPMatrix = SM_M4Multiply(&look, &rContext->cameras[i].vPMatrix);
	}
}
void SGL_StartRender(SGL_Window * window)
{
	ADD_FLAGS(window->rContext.state, SGL_RENDER_STATE_RENDERING);
	//
	//glDisable(GL_BLEND);
	SGL_UpdateCameras(&window->rContext);
	glBindBuffer(GL_UNIFORM_BUFFER, window->rContext.uniformMatrixHandle);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SGL_Mat4), &window->rContext.cameras[window->rContext.boundCamera].vPMatrix);
	//static float asd = 0.0f;
	//asd	= asd < M_PI*2.0f ? asd + 0.02f : 0.0f;
	//float color = SDL_sinf(asd)*0.25f + 0.75f;
	//float color = 0.05f;
	////float color = 0.75f;
	//glClearColor(color, color, color, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);

}
void SGL_EndRender(SGL_Window * window)
{
	window->rContext.boundShader = 0;
	REMOVE_FLAGS(window->rContext.state, SGL_RENDER_STATE_RENDERING);
	SDL_GL_SwapWindow(window->handle);
}
void SGL_BindCamera(SGL_RenderContext* rContext, U32 id)
{
	rContext->boundCamera = id;
	if (CHECK_FLAGS(rContext->state, SGL_RENDER_STATE_RENDERING))
	{
		glBindBuffer(GL_UNIFORM_BUFFER, rContext->uniformMatrixHandle);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SGL_Mat4), &rContext->cameras[rContext->boundCamera].vPMatrix);
	}
}

void SGL_Quit(void)
{
	IMG_Quit();
	SDL_Quit();
}
