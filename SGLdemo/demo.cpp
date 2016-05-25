#include <SGL.h>
#include "entity.h"
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
void TestRenderLoop()
{
	SGL_Init();
	SGL_Window window = SGL_CreateWindow("SGLDemo", 4, 5, 3, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SGL_Camera* camera0 = SGL_CreateCamera(&window.rContext, SGL_CAMERA_TYPE_ORTHO, SGL_CAMERA_ORTHO, 0.1f, 2.0f, degreesToRadians(45.0f), 1.0f);
	SGL_BindCamera(&window.rContext, SGL_CAMERA_ORTHO);
	SGL_DataSelect("Demo_data");
	SDL_Surface* surf = SGL_DataLoadImage("spriteGridTex");
	SGL_Tex2D tex = { surf->pixels, 0, GL_BGRA, GL_RGBA8, surf->w, surf->h };
	SGL_CreateTextures(&tex, &SGL_ParamsNearest, 1);
	SGL_DynamicRenderer ssr = SGL_CreateSimpleSpriteRenderer(10, &tex, &window.rContext);
	SGL_TexRegion texReg = { 64.0f,  64.0f,  64.0f,  64.0f };
	SGL_Vec2 scale = { 64.0f, 64.0f };
	bool quit = false;
	while (!quit)
	{
		SGL_RendererSync(ssr.syncs[ssr.bufferOffset]);
		for (size_t i = 0; i < 10; i++)
		{
			SGL_Vec2 pos = { i*64.0f-window.rContext.windowHalfSizef.x+32.0f, 0.0f };
			SGL_AddSpritePS(&ssr, pos, scale, texReg);
		}
		SGL_StartRender(&window);
		SGL_SimpleSpriteRendererDraw(&ssr, &window.rContext);
		SGL_EndRender(&window);
	}
}
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
	SGL_Window window = SGL_CreateWindow("SGLDemo", 4, 5, bufferCount, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	//SGL_Window window = SGL_CreateWindow("SGLDemo", 4, 5, bufferCount, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL);
	SGL_Camera* camera0 = SGL_CreateCamera(&window.rContext, SGL_CAMERA_TYPE_ORTHO, SGL_CAMERA_ORTHO, 0.1f, 2.0f, degreesToRadians(45.0f), 1.0f);
	SGL_Vec2 cameraStartPos = { camera0->position.x, camera0->position.y };
	//SGL_Camera* cameraScreenSpace = SGL_CreateCamera(&window.rContext, SGL_CAMERA_TYPE_ORTHO, SGL_CAMERA_ORTHO, 0.1f, 2.0f, degreesToRadians(45.0f), 1.0f);

	SGL_BindCamera(&window.rContext, SGL_CAMERA_ORTHO);
	SGL_DataSelect("Demo_data");
	//SDL_Surface* img = IMG_Load("particle2.tga");
	//SGL_DataSaveImage("particle2Tex", img);
	//U32 format = 390076419;
	SDL_Surface* surf = SGL_DataLoadImage("mountainsTex");
	SDL_Surface* surf2 = SGL_DataLoadImage("particle2Tex");
	SGL_Tex2D tex[2] = 
	{ 
		{ surf->pixels, 0, GL_BGRA, GL_RGBA8, surf->w, surf->h }, 
		{ surf2->pixels, 0, GL_BGRA, GL_RGBA8, surf2->w, surf2->h }
	};
	SGL_TexParams params[2]
	{
		SGL_ParamsNearest,
		SGL_ParamsLinear,
	};
	SGL_CreateTextures(tex, params, 2);
	U32 particleMaxCount = PARTICLE_COUNT;
	U32 particleCount = 100;
	//U32 spriteMaxCount = SPRITE_COUNT;
	//U32 spriteCount = 0;
	U32 psCount = 0;
	U32 actualDrawCount = 0;
	float rotation = degreesToRadians(-0.0f);
	float speed = 0.0f;
	float spriteSize = 17.0f;
	float spriteSizeHalf = spriteSize*0.5f;
	U32 anim = false;
	float animSpeed = 0.1f;
	//SGL_PointRenderer pr = SGL_CreatePointRenderer(particleMaxCount, SGL_SHADER_COLOR, &window.rContext);
	SGL_DynamicRenderer ssr = SGL_CreateSimpleSpriteRenderer(particleMaxCount, &tex[1], &window.rContext);
	const U32 bigMountainCount = 8;
	const U32 smallMountainCount = 4;
	float mountainsBigParallax = 0.25f;
	float mountainsSmallParallax = 0.75f;
	SGL_StaticRenderer mountains = SGL_CreateStaticSpriteRenderer(16, &tex[0], &window.rContext);
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
			{ 0, 111, 230, 255 },
			{ 0, 111, 230, 255 },
			{ 230, 242, 255, 255 },
			{ 230, 242, 255, 255 }
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
	TwAddVarRW(bar, "ParticleCount", TW_TYPE_UINT32, &particleCount," label='Sprite Count:' min=0 max=" PARTICLE_COUNT_STRING " step=10 help='Defines the number of particles in the scene.' ");
	TwAddVarRW(bar, "BufferCount", TW_TYPE_UINT32, &bufferCount, " label='Buffer Count:' min=1 max=3 step=1 help='Defines the number of buffers used in the scene.' ");
	GLsync syncObj = NULL;
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
		if (bufferCount != lastBufferCount)
		{
			window.rContext.bufferCount = bufferCount;
			lastBufferCount = bufferCount;
			SGL_DestroySSRenderer(&ssr);
			ssr = SGL_CreateSimpleSpriteRenderer(particleMaxCount, &tex[1], &window.rContext);
		}
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
			case SDL_KEYDOWN:
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_a:
				{
					movement.x = -1.0f;
					break;
				}
				case SDLK_d:
				{
					movement.x = 1.0f;
					break;
				}
				case SDLK_w:
				{
					movement.y = 1.0f;
					break;
				}
				case SDLK_s:
				{
					movement.y = -1.0f;
					break;
				}
				case SDLK_e:
				{
					window.rContext.cameras[window.rContext.boundCamera].scale *= 0.95f;
					break;
				}
				case SDLK_q:
				{
					window.rContext.cameras[window.rContext.boundCamera].scale *= 1.05f;
					break;
				}
				default:
					break;
				}
				break;
			}
			case SDL_KEYUP:
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
				{
					quit = true;
					break;
				}
				case SDLK_a:
				{
					movement.x = 0.0f;
					break;
				}
				case SDLK_d:
				{
					movement.x = 0.0f;
					break;
				}
				case SDLK_w:
				{
					movement.y = 0.0f;
					break;
				}
				case SDLK_s:
				{
					movement.y = 0.0f;
					break;
				}
				default:
					break;
				}
			}
			default:
				break;
			}
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
		SGL_RendererSync(ssr.syncs[ssr.bufferOffset]);
		gpuStall = SDL_GetTicks() - gpuStall;
		int x, y;
		SDL_GetMouseState(&x, &y);
		SGL_Vec2 pos = { (float)(x - window.rContext.windowSize.x/2), (float)(window.rContext.windowSize.y - y - window.rContext.windowSize.y/2) };
		pos = SM_V2FMultiply(pos, window.rContext.cameras[window.rContext.boundCamera].scale);
		pos.x += window.rContext.cameras[window.rContext.boundCamera].position.x;
		pos.y += window.rContext.cameras[window.rContext.boundCamera].position.y;

		for (size_t i = 0; i < particleCount; i++)
		{
			SGL_Vec2 distance = SM_V2Substract(pos, particles[i].position);
			float mag = SM_V2Magnitude(distance);
			if (mag < particles[i].zeroRadius)
			{
				particles[i].energy = 0.0f;
			}
			particles[i].energy += timeDelta*100.0f;
			particles[i].velocity = SM_V2Add(SM_V2FMultiply(SM_V2Normalize(distance), particles[i].energy*timeDelta), particles[i].velocity);
			particles[i].velocity = SM_V2Substract(particles[i].velocity, SM_V2FMultiply(particles[i].velocity, 0.5f*timeDelta));
			particles[i].position.x += particles[i].velocity.x * timeDelta;
			particles[i].position.y += particles[i].velocity.y * timeDelta;
			SGL_AddSpritePS(&ssr, particles[i].position, particles[i].scale, texReg);
		}
		if (movement.x != 0.0f || movement.y != 0.0f)
		{
			SGL_Vec2 delta = SM_V2FMultiply(SM_V2Normalize(movement), window.rContext.cameras[window.rContext.boundCamera].scale*cameraSpeed*timeDelta);
			window.rContext.cameras[window.rContext.boundCamera].position.x += delta.x;
			window.rContext.cameras[window.rContext.boundCamera].position.y += delta.y;
		}
		//SGL_CHECK_GL_ERROR;
		SGL_StartRender(&window);
		SGL_StaticColorRendererDraw(&background, &window.rContext);
		SGL_StaticSpriteRendererDrawRange(&mountains, &window.rContext, 0, smallMountainCount, { (window.rContext.cameras[window.rContext.boundCamera].position.x - cameraStartPos.x)*mountainsSmallParallax, (window.rContext.cameras[window.rContext.boundCamera].position.y - cameraStartPos.y)*mountainsSmallParallax});
		SGL_StaticSpriteRendererDrawRange(&mountains, &window.rContext, smallMountainCount, bigMountainCount, { (window.rContext.cameras[window.rContext.boundCamera].position.x - cameraStartPos.x)*mountainsBigParallax, (window.rContext.cameras[window.rContext.boundCamera].position.y - cameraStartPos.y)*mountainsBigParallax });
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		SGL_SimpleSpriteRendererDraw(&ssr, &window.rContext);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		TwDraw();
		SGL_EndRender(&window);
	}
	TwTerminate();
	SGL_DestroyWindow(&window);
	SGL_Quit();
	return EXIT_SUCCESS;
	//*/
}