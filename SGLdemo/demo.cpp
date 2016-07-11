#include <SGL.h>
#include "entity.h"
#include "input.h"
#include <AntTweakBar.h>
#include <thread>
#include <chrono>
struct Particle
{
	SGL_Vec2 position;
	SGL_Vec2 velocity;
	SGL_Vec2 scale;
	float energy;
	float zeroRadius;
};
int main(int argc, char* argv[])
{
	//TestRenderLoop();
#define PARTICLE_COUNT 1000000
#define PARTICLE_COUNT_STRING "1000000"
#define SPRITE_COUNT 1000000
#define SPRITE_COUNT_STRING "1000000"
	SGL_Init();
	SDL_Log("%i concurrent threads are suggested (std)", std::thread::hardware_concurrency());
	SDL_Log("%i concurrent threads are suggested (sdl2)", SDL_GetCPUCount());
	U32 bufferCount = 3;
	U32 lastBufferCount = bufferCount;
	//SGL_Window window = SGL_CreateWindow("SGLDemo", 4, 5, bufferCount, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SGL_Window window = SGL_CreateWindow("SGLDemo", 4, 5, bufferCount, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_MAXIMIZED | SDL_WINDOW_OPENGL);
	SGL_Camera* camera0 = SGL_CreateCamera(&window.rContext, SGL_CAMERA_TYPE_ORTHO, SGL_CAMERA_ORTHO, 0.1f, 2.0f, degreesToRadians(45.0f), 1.0f);
	SGL_Vec2 cameraStartPos = { camera0->position.x, camera0->position.y };
	InputManager input = InputManager();
	//SGL_Camera* cameraScreenSpace = SGL_CreateCamera(&window.rContext, SGL_CAMERA_TYPE_ORTHO, SGL_CAMERA_ORTHO, 0.1f, 2.0f, degreesToRadians(45.0f), 1.0f);
	SGL_BindCamera(&window.rContext, SGL_CAMERA_ORTHO);
	SGL_DataSelect("Demo_data");
	//SDL_Surface* img = IMG_Load("testTex.tga");
	//if (img != NULL && img->pixels != nullptr)
	//{
	//	SGL_DataSaveImage("test1Tex", img);
	//}
	//U32 format = 390076419;
	SDL_Surface* surf = SGL_DataLoadImage("mountainsTex");
	SDL_Surface* surf2 = SGL_DataLoadImage("particle2Tex");
	SDL_Surface* surf3 = SGL_DataLoadImage("bjarneLightTex");
	SDL_Surface* surf4 = SGL_DataLoadImage("test1Tex");
	SGL_Tex2D tex[4] = 
	{ 
		{ surf->pixels, 0, GL_BGRA, GL_RGBA8, surf->w, surf->h }, 
		{ surf2->pixels, 0, GL_BGRA, GL_RGBA8, surf2->w, surf2->h },
		{ surf3->pixels, 0, GL_BGRA, GL_RGBA8, surf3->w, surf3->h },
		{ surf4->pixels, 0, GL_BGRA, GL_RGBA8, surf4->w, surf4->h },
	};
	SGL_TexParams params[4]
	{
		SGL_ParamsNearest,
		SGL_ParamsLinear,
		SGL_ParamsNearest,
		SGL_ParamsNearest,
	};
	SGL_CreateTextures(tex, params, 4);
	U32 particleMaxCount = PARTICLE_COUNT;
	U32 particleCount = 0;
	//U32 spriteMaxCount = SPRITE_COUNT;
	//U32 spriteCount = 0;
	U32 psCount = 0;
	U32 actualDrawCount = 0;
	const U32 lightSize = 512;
	const F32 lightSizef = (F32)lightSize;
	F32 rotation = degreesToRadians(-0.0f);
	float speed = 0.0f;
	float spriteSize = 17.0f;
	float spriteSizeHalf = spriteSize*0.5f;
	U32 anim = false;
	F32 animSpeed = 0.1f;
	//SGL_PointRenderer pr = SGL_CreatePointRenderer(particleMaxCount, SGL_SHADER_COLOR, &window.rContext);
	SGL_SpriteRenderer ssr = SGL_CreateSpriteRenderer(particleMaxCount, &tex[1], &window.rContext);
	const U32 bigMountainCount = 8;
	const U32 smallMountainCount = 4;
	F32 mountainsBigParallax = 0.25f;
	F32 mountainsSmallParallax = 0.75f;
	camera0->forcedSize = { lightSizef*0.5f, lightSizef*0.5f };
	SGL_StaticRenderer testRender = SGL_CreateStaticSpriteRenderer(1, &tex[3], &window.rContext);
	SGL_AddStaticSpritePS(&testRender, { 0.375f,0.375f }, { 256.0f, 256.0f }, { 0.0f,0.0f,256.0f,256.0f });
	SGL_StaticSpriteRendererCommit(&testRender);
	SGL_StaticRenderer mountains = SGL_CreateStaticSpriteRenderer(17, &tex[0], &window.rContext);
	{
		SGL_Vec2 bigMountainSize[2] =
		{
			{ 1024.0f,1006.0f },
			{ 1024.0f,614.0f },
		};
		SGL_TexRegion bigMountainRegion[2] = 
		{
			{ 0.0f,1.0f, bigMountainSize[0]},
			{ bigMountainSize[0].x ,1.0f, bigMountainSize[1]}
		};
		SGL_Vec2 smallMountainSize[2] =
		{
			{ 1024.0f,510.0f },
			{ 1024.0f,328.0f },
		};
		SGL_TexRegion smallMountainRegion[2] =
		{
			{ 0.0f,1025.0f, smallMountainSize[0]},
			{ smallMountainSize[0].x, 1025.0f, smallMountainSize[1]}
		};
		for (size_t i = 0; i < smallMountainCount; i++)
		{
			U32 randMtn = i % 2;
			SGL_AddStaticSpritePS(&mountains, { -smallMountainSize[0].x*(smallMountainCount / 2) + smallMountainSize[0].x / 2.0f + 0.375f + i * smallMountainSize[0].x,-360.0f + 0.375f + smallMountainSize[randMtn].y / 2.0f }, smallMountainSize[randMtn], smallMountainRegion[randMtn]);
		}
		for (size_t i = 0; i < bigMountainCount; i++)
		{
			U32 randMtn = i % 2;
			SGL_AddStaticSpritePS(&mountains, {-bigMountainSize[0].x*(bigMountainCount/2)+ bigMountainSize[0].x/2.0f + 0.375f + i * bigMountainSize[0].x,-738.0f + 0.375f + bigMountainSize[randMtn].y/2.0f}, bigMountainSize[randMtn], bigMountainRegion[randMtn]);
		}
		SGL_StaticSpriteRendererCommit(&mountains);
	}
	SGL_StaticRenderer background = SGL_CreateStaticColorRenderer(1, SGL_SHADER_SP_COLOR, &window.rContext);
	{
		SGL_Color colors[4] =
		{
			//{ 0, 111, 230, 255 },
			//{ 0, 111, 230, 255 },
			//{ 230, 242, 255, 255 },
			//{ 230, 242, 255, 255 }
			{ 64, 64, 64, 255 },
			{ 64, 64, 64, 255 },
			{ 128, 128, 128, 255 },
			{ 128, 128, 128, 255 }
		};
		SGL_AddStaticColoredRectanglePS(&background, { 0.0f,0.0f }, { 2.0f,2.0f }, colors);
		SGL_StaticColorRendererCommit(&background);
	}
	SGL_TexRegion texReg = {0.0f, 0.0f, spriteSize, spriteSize };
	SGL_SquareTexRegion sqrTexReg = { 0.0f, 0.0f, spriteSize };
	Particle* particles = (Particle*)SDL_malloc(sizeof(Particle)*particleMaxCount);
	for (size_t i = 0; i < particleMaxCount; i++)
	{
		particles[i].position =
		{
			((F32)rand() / (F32)(RAND_MAX)) * (window.rContext.windowHalfSizef.x * 2 - spriteSize) - (window.rContext.windowHalfSizef.x - spriteSizeHalf),
			((F32)rand() / (F32)(RAND_MAX)) * (window.rContext.windowHalfSizef.y * 2 - spriteSize) - (window.rContext.windowHalfSizef.y - spriteSizeHalf)
		};
		float size = (((F32)rand() / (F32)(RAND_MAX)) * 0.5f + 0.5f)* spriteSize;
		particles[i].scale = { size, size };
		particles[i].velocity = { 0.0f,0.0f };
		//particles[i].velocity = { (((F32)rand() / (F32)(RAND_MAX)) * 10.0f - 10.0f), (((F32)rand() / (F32)(RAND_MAX)) * 10.0f - 10.0f) };
		particles[i].energy = 0.0f;
		particles[i].zeroRadius = ((F32)rand() / (F32)(RAND_MAX)) * 25.0f + 5.0f;
	}
	float cameraSpeed = 512.0f;
	float animTimer = 0;
	float spriteOffset = 0;
	//float timer = 0.0f;
	//float testAverage = 1.0f;
	bool quit = false;
	U32 debug = 0;
	float timeDelta = 0.0f;
	float fpsTimer = 0.0f;
	U32 fps = 0;
	U32 time = SDL_GetTicks();
	//U32 testTime = 0;
	//U32 testFrames = 0;
	U32 frameCount = 0;
	SGL_Vec2 movement = { 0.0f,0.0f };
	float zoom = 1.0f;

	U32 gpuStall = 0;
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(window.rContext.windowSize.x, window.rContext.windowSize.y);
	TwBar* bar = TwNewBar("Console");
	//color='A R G B'
	TwDefine(" Console color='192 35 35 37' help='This bar contains debug data and input' ");

	TwAddVarRO(bar, "Width", TW_TYPE_INT32, &window.rContext.windowSize.x, " label='Window width:' help='Width of the graphics window (in pixels)' ");
	TwAddVarRO(bar, "Height", TW_TYPE_INT32, &window.rContext.windowSize.y, " label='Window height:' help='Height of the graphics window (in pixels)' ");
	TwAddVarRO(bar, "FPS", TW_TYPE_UINT32, &fps, " label='FPS:' help='Frames Per Second' ");
	TwAddVarRO(bar, "GPUStall", TW_TYPE_UINT32, &gpuStall, " label='GPU Stall:' help='Microseconds spent waiting for buffer to update' ");
	TwAddVarRW(bar, "Debug", TW_TYPE_BOOL32, &window.rContext.debug, " label='Debug Draw:' help='Defines the number of particles in the scene.' ");
	TwAddVarRW(bar, "ParticleCount", TW_TYPE_UINT32, &particleCount," label='Sprite Count:' min=0 max=" PARTICLE_COUNT_STRING " step=10 help='Defines the number of particles in the scene.' ");
	TwAddVarRW(bar, "BufferCount", TW_TYPE_UINT32, &bufferCount, " label='Buffer Count:' min=1 max=3 step=1 help='Defines the number of buffers used in the scene.' ");
	U32 fbo;
	U32 colorTex;
	// Create a framebuffer object and bind it 
	glGenFramebuffers(1, &fbo); 
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// Create a texture for our color buffer 
	glGenTextures(1, &colorTex);
	glBindTexture(GL_TEXTURE_2D, colorTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 4096, 4096);
	// We’re going to read from this, but it won’t have mipmaps, 
	// so turn off mipmaps for this texture. 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Create a texture that will be our FBO’s depth buffer 
	// Now, attach the color and depth textures to the FBO 
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTex, 0); 
	// Tell OpenGL that we want to draw into the framebuffer’s color // attachment static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 }; glDrawBuffers(1, draw_buffers); 
	//Does the GPU support current FBO configuration?
	GLenum status;
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		SDL_Log("good");
		break;
	default:
		SDL_Log("bad");
		break;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//glViewport(0, 0, window.rContext.windowSize.x, window.rContext.windowSize.y);
	//camera0->camType = SGL_CAMERA_TYPE_ORTHO_FORCED_SIZE;
	SGL_StartRender(&window);
	//camera0->camType = SGL_CAMERA_TYPE_ORTHO;
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	SGL_StaticSpriteRendererDraw(&testRender, &window.rContext);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glViewport(0, 0, window.rContext.windowSize.x, window.rContext.windowSize.y);
	SGL_Tex2D frameBufferTex = { nullptr, colorTex, GL_BGRA, GL_RGBA8, window.rContext.windowSize.x, window.rContext.windowSize.y };
	SGL_StaticRenderer frameBufferRenderer = SGL_CreateStaticSpriteRenderer(1, &frameBufferTex, &window.rContext);
	frameBufferRenderer.shaderHandle = window.rContext.shaders[SGL_SHADER_SP_SPRITE].handle;
	SGL_AddStaticSpriteMask(&frameBufferRenderer);
	//SGL_AddStaticSpritePS(&frameBufferRenderer, { 0.0f,0.0f }, { (F32)window.rContext.windowSize.x,  (F32)window.rContext.windowSize.y }, { 0.0f,0.0f,  (F32)window.rContext.windowSize.x,  (F32)window.rContext.windowSize.y });
	SGL_StaticSpriteRendererCommit(&frameBufferRenderer);
	U32 shadowFbo;
	U32 shadowTex;
	// Create a framebuffer object and bind it 
	glGenFramebuffers(1, &shadowFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);
	// Create a texture for our color buffer 
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, lightSize, 1);
	// We’re going to read from this, but it won’t have mipmaps, 
	// so turn off mipmaps for this texture. 
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Create a texture that will be our FBO’s depth buffer 
	// Now, attach the color and depth textures to the FBO 
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadowTex, 0);
	// Tell OpenGL that we want to draw into the framebuffer’s color // attachment static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 }; glDrawBuffers(1, draw_buffers); 
	//Does the GPU support current FBO configuration?
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		SDL_Log("shadow good");
		break;
	default:
		SDL_Log("shadow bad");
		break;
	}
	//glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);
	//glViewport(0, 0, lightSize, 1);
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	//SGL_MapSectorShadows(lightSizef, { -1.0f, 0.0f }, degreesToRadians(45.0), frameBufferTex.handle, &window.rContext);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glViewport(0, 0, window.rContext.windowSize.x, window.rContext.windowSize.y);
	SGL_Vec2 lightPos = { 0.0f,0.0f };
	SGL_Tex2D frameBufferTex1 = { nullptr, shadowTex, GL_BGRA, GL_RGBA8, lightSize, 1 };
	SGL_StaticRenderer frameBufferRenderer1 = SGL_CreateStaticSpriteRenderer(1, &frameBufferTex1, &window.rContext);
	frameBufferRenderer1.shaderHandle = window.rContext.shaders[SGL_SHADER_SPRITE].handle;
	SGL_AddStaticSpritePS(&frameBufferRenderer1, { 0.0f,lightSizef*-0.5f }, { lightSizef, 1 }, { 0.0f,0.0f, lightSizef, 1 });
	SGL_StaticSpriteRendererCommit(&frameBufferRenderer1);
	SGL_DynamicRenderer sectorRenderer = SGL_CreateSectorRenderer(&window.rContext);
	GLsync syncObj = NULL;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	while (!quit)
	{
		timeDelta = float(SDL_GetTicks() - time)*0.001f;
		fpsTimer += timeDelta;
		if (fpsTimer >= 1.0f)
		{
			fps = frameCount;
			frameCount = 0;
			fpsTimer--;
		}
		time = SDL_GetTicks();
		frameCount++;
		//if (bufferCount != lastBufferCount)
		//{
		//	window.rContext.bufferCount = bufferCount;
		//	lastBufferCount = bufferCount;
		//	SGL_DestroySSRenderer(&ssr);
		//	ssr = SGL_CreateSimpleSpriteRenderer(particleMaxCount, &tex[1], &window.rContext);
		//}
		input.Update();
		SDL_Event e = SDL_Event();

		while (SDL_PollEvent(&e))
		{
			if (e.key.keysym.sym == SDLK_RETURN)
			{
				int asdfssasf = 0;
			}
			TwEventSDL(&e, 2, 0);
			switch (e.type)
			{
			case SDL_QUIT:
			{
				quit = true;
				break;
			}
			case SDL_WINDOWEVENT:
			{
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					int w, h;
					SDL_GetWindowSize(window.handle, &w, &h);
					if (w != window.rContext.windowSize.x || h != window.rContext.windowSize.y)
					{
						window.rContext.windowSize.x = w;
						window.rContext.windowSize.y = h;
						window.rContext.windowHalfSizef.x = (float)w * 0.5f;
						window.rContext.windowHalfSizef.y = (float)h * 0.5f;
						glViewport(0, 0, w, h);
					}
				}
				break;
			}
			default:
				break;
			}
		}
		movement.x = 0.0f;
		movement.y = 0.0f;
		zoom = 1.0f;
		if (input.IsKeyUp(SDLK_ESCAPE))
		{
			quit = true;
		}
		if (input.IsKeyPressed(SDLK_d))
		{
			movement.x += 1.0f;
		}
		if (input.IsKeyPressed(SDLK_a))
		{
			movement.x -= 1.0f;
		}
		if (input.IsKeyPressed(SDLK_w))
		{
			movement.y += 1.0f;
		}
		if (input.IsKeyPressed(SDLK_s))
		{
			movement.y -= 1.0f;
		}
		if (input.IsKeyPressed(SDLK_q))
		{
			zoom += 0.5 * timeDelta;
		}
		if (input.IsKeyPressed(SDLK_e))
		{
			zoom -= 0.5 * timeDelta;
		}
		texReg = { spriteSize*spriteOffset,spriteSize,spriteSize, spriteSize };
		if (anim)
		{
			animTimer += timeDelta;
			if (animTimer >= animSpeed)
			{
				animTimer = 0;
				spriteOffset++;
			}
		}
		gpuStall = SDL_GetTicks();
		//SGL_RendererSync(ssr.syncs[ssr.bufferOffset]);
		gpuStall = SDL_GetTicks() - gpuStall;
		int x, y;
		SDL_GetMouseState(&x, &y);
		SGL_Vec2 pos = { (float)(x - window.rContext.windowSize.x/2), (float)(window.rContext.windowSize.y - y - window.rContext.windowSize.y/2) };
		pos = SM_V2FMultiply(pos, window.rContext.cameras[window.rContext.boundCamera].scale);
		pos.x += window.rContext.cameras[window.rContext.boundCamera].position.x;
		pos.y += window.rContext.cameras[window.rContext.boundCamera].position.y;

		if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) 
		{
			lightPos = pos;
			//lightPos.x -= fmodf(lightPos.x, 1.0f) + 0.375f;
			//lightPos.y -= fmodf(lightPos.y, 1.0f) + 0.375f;
		}
		for (size_t i = 0; i < particleCount; i++)
		{
			//SGL_Vec2 distance = SM_V2Substract(pos, particles[i].position);
			//float mag = SM_V2Magnitude(distance);
			//if (mag < particles[i].zeroRadius)
			//{
			//	particles[i].energy = 0.0f;
			//}
			//particles[i].energy += timeDelta*100.0f;
			//particles[i].velocity = SM_V2Add(SM_V2FMultiply(SM_V2Normalize(distance), particles[i].energy*timeDelta), particles[i].velocity);
			//particles[i].velocity = SM_V2Substract(particles[i].velocity, SM_V2FMultiply(particles[i].velocity, 0.5f*timeDelta));
			//particles[i].position.x += particles[i].velocity.x * timeDelta;
			//particles[i].position.y += particles[i].velocity.y * timeDelta;
			SGL_AddSpritePS(&ssr, particles[i].position, particles[i].scale, texReg);
		}
		if (movement.x != 0.0f || movement.y != 0.0f)
		{
			SGL_Vec2 delta = SM_V2FMultiply(SM_V2Normalize(movement), window.rContext.cameras[window.rContext.boundCamera].scale*cameraSpeed*timeDelta);
			window.rContext.cameras[window.rContext.boundCamera].position.x += delta.x;
			window.rContext.cameras[window.rContext.boundCamera].position.y += delta.y;
		}
		if (zoom != 1.0f)
		{
			window.rContext.cameras[window.rContext.boundCamera].scale *= zoom;
		}
		SGL_StartRender(&window);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		SGL_StaticSpriteRendererDraw(&testRender, &window.rContext);
		SGL_StaticSpriteRendererDraw(&frameBufferRenderer1, &window.rContext);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);
		glViewport(0, 0, lightSize, 1);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		//SGL_Vec2 lightDir = SM_V2Substract(pos, lightPos);
		SGL_Vec2 lightDir = {1.0f,0.0f};
		float lightAngle = SM_V2Magnitude(pos)*0.004f;
		lightAngle = lightAngle > M_PI ? M_PI : lightAngle;
		SGL_MapSectorShadows(lightSizef, lightPos, SM_V2Normalize(pos), M_PI, &frameBufferTex, &window.rContext);
		glViewport(0, 0, window.rContext.windowSize.x, window.rContext.windowSize.y);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		SGL_StaticRendererDrawSP(&background, &window.rContext);
		//SGL_StaticSpriteRendererDrawRange(&mountains, &window.rContext, 0, smallMountainCount, { (window.rContext.cameras[window.rContext.boundCamera].position.x - cameraStartPos.x)*mountainsSmallParallax, (window.rContext.cameras[window.rContext.boundCamera].position.y - cameraStartPos.y)*mountainsSmallParallax});
		//SGL_StaticSpriteRendererDrawRange(&mountains, &window.rContext, smallMountainCount, bigMountainCount, { (window.rContext.cameras[window.rContext.boundCamera].position.x - cameraStartPos.x)*mountainsBigParallax, (window.rContext.cameras[window.rContext.boundCamera].position.y - cameraStartPos.y)*mountainsBigParallax });

		SGL_DrawLightSector(lightSizef, lightPos, SM_V2Normalize(pos), M_PI, &frameBufferTex1, &sectorRenderer, &window.rContext);
		SGL_StaticSpriteRendererDraw(&frameBufferRenderer, &window.rContext);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		SGL_SpriteRendererDraw(&ssr, &window.rContext);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		SGL_CHECK_GL_ERROR;
		TwDraw();
		SGL_EndRender(&window);
	}
	TwTerminate();
	SGL_DestroyWindow(&window);
	SGL_Quit();
	return EXIT_SUCCESS;
	//*/
}