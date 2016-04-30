#include <SGL.h>
#include "entity.h"
#include <AntTweakBar.h>
#include <thread>
int main(int argc, char* argv[])
{
#define SPRITE_COUNT 1000000
#define SPRITE_COUNT_STRING "1000000"
	SGL_Init();
	SDL_Log("%i concurrent threads are suggested (std)", std::thread::hardware_concurrency());
	SDL_Log("%i concurrent threads are suggested (sdl2)", SDL_GetCPUCount());
	U32 bufferCount = 3;
	U32 lastBufferCount = bufferCount;
	SGL_Window window = SGL_CreateWindow("SGLDemo", 4, 5, bufferCount, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 576, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SGL_Camera* camera0 = SGL_CreateCamera(&window.rContext, SGL_CAMERA_TYPE_ORTHO, SGL_CAMERA_ORTHO, 0.1f, 2.0f, degreesToRadians(45.0f), 1.0f);
	SGL_BindCamera(&window.rContext, SGL_CAMERA_ORTHO);
	SGL_DataSelect("Demo_data");
	U32 format = 390076419;
	SDL_Surface* surf = SGL_DataLoadImage("spriteGridTex");
	SGL_Tex2D tex = { surf->pixels, 0, GL_BGRA, GL_RGBA8, surf->w, surf->h };
	SGL_CreateTextures(&tex, &SGL_ParamsNearest, 1);
	U32 spriteMaxCount = SPRITE_COUNT;
	U32 spriteCount = 0;
	U32 entityCount = 16;
	U32 entityMaxCount = 16;
	U32 psCount = 0;
	U32 actualDrawCount = 0;
	float rotation = degreesToRadians(-0.0f);
	float speed = 0.0f;
	float spriteSize = 8.0f;
	float spriteSizeHalf = spriteSize*0.5f;
	U32 anim = false;
	float animSpeed = 0.1f;
	float opacity = 0.0f;
	//SGL_StaticSpriteRenderer sssr = SGL_CreateStaticSpriteRenderer(spriteMaxCount, &tex, &window.rContext);
	SGL_PointSpriteRenderer psr = SGL_CreatePointSpriteRenderer(spriteMaxCount, &tex, &window.rContext);
	SGL_SimpleSpriteRenderer ssr = SGL_CreateSimpleSpriteRenderer(spriteMaxCount, &tex, &window.rContext);
	SGL_TexRegion texReg = {spriteSize, spriteSize, spriteSize, spriteSize };
	SGL_SquareTexRegion sqrTexReg = { spriteSize, spriteSize, spriteSize };
	SGL_Vec2 * scaleVecs = (SGL_Vec2*)SDL_malloc(sizeof(SGL_Vec2)*spriteMaxCount);
	SGL_Vec2 * posVecs = (SGL_Vec2*)SDL_malloc(sizeof(SGL_Vec2)*spriteMaxCount);
	SGL_Vec2 * directions = (SGL_Vec2*)SDL_malloc(sizeof(SGL_Vec2)*spriteMaxCount);
	Entity* entities = (Entity*)SDL_calloc(1, sizeof(Entity)*entityMaxCount);
	entities[0].transform.rotation = rotation;
	entities[0].transform.scale.x = spriteSize;
	entities[0].transform.scale.y = spriteSize;
	entities[0].transform.position.x = 0.375f;
	entities[0].transform.position.y = 0.375f;
	SM_UpdateTransform(&entities[0].transform);
	SGL_Mat3 inverse = SM_M3Inverse(&entities[0].transform.mat);
	SGL_Mat3 combinedMat = entities[0].transform.mat;
	for (size_t i = 1; i < entityMaxCount; i++)
	{
		entities[i].transform.rotation = rotation;
		entities[i].transform.scale = { spriteSize, spriteSize };
		entities[i].transform.position = { 0.375f, spriteSize*i+ 0.375f };
		SM_GlobalToLocalTrans(&entities[i].transform, &inverse);
		combinedMat = SM_M3Multiply(&entities[i].transform.mat, &combinedMat);
		inverse = SM_M3Inverse(&combinedMat);
	}
	SM_UpdateTransform(&entities[0].transform);
	for (size_t i = 0; i < spriteMaxCount; i++)
	{

		scaleVecs[i] = 
		{ 
			spriteSize, 
			spriteSize 
		};
		posVecs[i] = 
		{ 
			((F32)rand() / (F32)(RAND_MAX)) * (window.rContext.windowHalfSizef.x * 2 - spriteSize) - (window.rContext.windowHalfSizef.x - spriteSizeHalf),
			((F32)rand() / (F32)(RAND_MAX)) * (window.rContext.windowHalfSizef.y * 2 - spriteSize) - (window.rContext.windowHalfSizef.y - spriteSizeHalf)
		};
		float rot = ((F32)rand() / (F32)(RAND_MAX)) *M_PI*2.0f;
		float speed = ((F32)rand() / (F32)(RAND_MAX)) * 1.0f + 0.5f;
		directions[i] =
		{
			SDL_cosf(rot)*speed,
			SDL_sinf(rot)*speed,
		};
		//SGL_AddStaticSpritePS(&sssr, posVecs[i], scaleVecs[i], texReg);
	}
	//SGL_StaticSpriteRendererCommit(&sssr);
	float animTimer = 0;
	float spriteOffset = 0;
	bool quit = false;
	U32 debug = 0;
	float timeDelta = SDL_GetTicks();
	float fpsTimer = 0.0f;
	U32 fps = 0;
	U32 time = SDL_GetTicks();
	U32 frameCount = 0;

	U32 gpuStall = 0;
	//U32 timer = SDL_GetTicks();
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(window.rContext.windowSize.x, window.rContext.windowSize.y);
	TwBar* bar = TwNewBar("Console");
	//color='A R G B'
	TwDefine(" Console color='192 35 35 37' help='This bar contains debug data and input' ");

	TwAddVarRO(bar, "Width", TW_TYPE_INT32, &window.rContext.windowSize.x, " label='Window width:' help='Width of the graphics window (in pixels)' ");
	TwAddVarRO(bar, "Height", TW_TYPE_INT32, &window.rContext.windowSize.y, " label='Window height:' help='Height of the graphics window (in pixels)' ");
	TwAddVarRO(bar, "FPS", TW_TYPE_UINT32, &fps, " label='FPS:' help='Frames Per Second' ");
	TwAddVarRO(bar, "GPUStall", TW_TYPE_UINT32, &gpuStall, " label='GPU Stall:' help='Microseconds spent waiting for buffer to update' ");

	TwAddVarRW(bar, "SpriteCount", TW_TYPE_UINT32, &spriteCount," label='Sprite Count:' min=0 max=" SPRITE_COUNT_STRING " step=10 help='Defines the number of sprites in the scene.' ");
	TwAddVarRW(bar, "PSCount", TW_TYPE_UINT32, &psCount, " label='PointSprite Count:' min=0 max=" SPRITE_COUNT_STRING " step=10 help='Defines the number of point sprites in the scene.' ");
	TwAddVarRW(bar, "EntityCount", TW_TYPE_UINT32, &entityCount, " label='Entity Count:' min=1 max=16 step=1 help='Defines the number of entity sprites in the scene.' ");
	TwAddVarRW(bar, "BufferCount", TW_TYPE_UINT32, &bufferCount, " label='Buffer Count:' min=1 max=3 step=1 help='Defines the number of buffers used in the scene.' ");
	TwAddVarRW(bar, "Animate", TW_TYPE_BOOL32, &anim, " label='Animate:' help='Frames Per Second' ");
	TwAddVarRW(bar, "Debug", TW_TYPE_BOOL32, &debug, " label='Debug shader:' help='Using debug shader' ");
	TwAddVarRW(bar, "Speed", TW_TYPE_FLOAT, &speed, " label='Speed:' min=-100.0 max=100.0 step=20.0 help='Speed in pixels per second' ");
	TwAddVarRW(bar, "PosX", TW_TYPE_FLOAT, &entities[0].transform.position.x, " label='PosX:' min=-1000.0 max=1000.0 step=8.0 help='Speed in pixels per second' ");
	TwAddVarRW(bar, "PosY", TW_TYPE_FLOAT, &entities[0].transform.position.y, " label='PosY:' min=-1000.0 max=1000.0 step=8.0 help='Speed in pixels per second' ");
	TwAddVarRW(bar, "Rotation", TW_TYPE_FLOAT, &rotation, " label='rotation:' min=-3.14159265358 max=3.14159265358 step=0.00314159265 help='Entity rotation in radians' ");
	TwAddVarRW(bar, "Opacity", TW_TYPE_FLOAT, &opacity, " label='Opacity:' min=-1.0 max=1.0 step=0.1 help='Shader opacity' ");
	bool firstFrame = true;
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
		if (bufferCount != lastBufferCount)
		{
			window.rContext.bufferCount = bufferCount;
			lastBufferCount = bufferCount;
			SGL_DestroySSRenderer(&ssr);
			ssr = SGL_CreateSimpleSpriteRenderer(spriteMaxCount, &tex, &window.rContext);
		}
		frameCount++;
		time = SDL_GetTicks();
		SDL_Event e = SDL_Event();
		while(SDL_PollEvent(&e))
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
			case SDL_KEYUP:
			{
				//switch (e.key.keysym.sym)
				//{
				//case SDLK_KP_4:
				//{
				//	rotation += 0.025f*M_PI;
				//	SGL_Vec4 forward = { 0.0f,0.0f,1.0f,0.0f };
				//	window.rContext.cameras[window.rContext.boundCamera].rotation = SM_QAngleAxis(rotation, &forward);
				//	break;
				//}
				//default:
				//	break;
				//}
			}
			case SDL_KEYDOWN:
			{
				switch (e.key.keysym.sym)
				{
				//case SDLK_KP_0:
				//{
				//	break;
				//}
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
		SGL_RendererSync(psr.syncs[psr.bufferOffset]);
		gpuStall = SDL_GetTicks() - gpuStall;
		if (entityCount > 0)
		{
			entities[0].transform.rotation = rotation;
			SM_UpdateTransform(&entities[0].transform);
			SGL_AddSpriteM3(&ssr, &entities[0].transform.mat, texReg);
			SGL_Mat3 combinedMat = entities[0].transform.mat;
			for (size_t i = 1; i < entityCount; i++)
			{
				entities[i].transform.rotation = rotation;
				SM_UpdateTransform(&entities[i].transform);
				combinedMat = SM_M3Multiply(&entities[i].transform.mat, &combinedMat);
				SGL_AddSpriteM3(&ssr, &combinedMat, texReg);
			}
		}
		for (size_t i = 0; i < spriteCount; i++)
		{
			posVecs[i] = SM_V2Add(posVecs[i], SM_V2FMultiply(directions[i], speed*timeDelta));
			if (posVecs[i].x + spriteSizeHalf > window.rContext.windowHalfSizef.x && directions[i].x*speed>0.0f)
			{
				directions[i].x = -directions[i].x;
			}
			else if(posVecs[i].x - spriteSizeHalf < -window.rContext.windowHalfSizef.x && directions[i].x*speed<0.0f)
			{
				directions[i].x = -directions[i].x;
			}
			else if (posVecs[i].y + spriteSizeHalf > window.rContext.windowHalfSizef.y && directions[i].y*speed>0.0f)
			{
				directions[i].y = -directions[i].y;
			}
			else if (posVecs[i].y - spriteSizeHalf < -window.rContext.windowHalfSizef.y && directions[i].y*speed<0.0f)
			{
				directions[i].y = -directions[i].y;
			}
			SGL_AddSpritePS(&ssr, posVecs[i], scaleVecs[i], texReg);
		}
		for (size_t i = 0; i < psCount; i++)
		{
			posVecs[i] = SM_V2Add(posVecs[i], SM_V2FMultiply(directions[i], speed*timeDelta));
			if (posVecs[i].x + spriteSizeHalf > window.rContext.windowHalfSizef.x && directions[i].x*speed>0.0f)
			{
				directions[i].x = -directions[i].x;
			}
			else if (posVecs[i].x - spriteSizeHalf < -window.rContext.windowHalfSizef.x && directions[i].x*speed<0.0f)
			{
				directions[i].x = -directions[i].x;
			}
			else if (posVecs[i].y + spriteSizeHalf > window.rContext.windowHalfSizef.y && directions[i].y*speed>0.0f)
			{
				directions[i].y = -directions[i].y;
			}
			else if (posVecs[i].y - spriteSizeHalf < -window.rContext.windowHalfSizef.y && directions[i].y*speed<0.0f)
			{
				directions[i].y = -directions[i].y;
			}
			SGL_AddPointSpritePS(&psr, posVecs[i], sqrTexReg);
		}
		SGL_StartRender(&window);
		//SGL_StaticSpriteRendererDraw(&sssr, &window.rContext, opacity);
		SGL_SimpleSpriteRendererDraw(&ssr, &window.rContext, opacity);
		SGL_PointSpriteRendererDraw(&psr, &window.rContext);
		TwDraw();
		SGL_EndRender(&window);
		SGL_CHECK_GL_ERROR;
		//timer = SDL_GetTicks();
	}
	TwTerminate();
	SGL_DestroyWindow(&window);
	SGL_Quit();
	return EXIT_SUCCESS;
}