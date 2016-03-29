#include <SGL.h>
int main(int argc, char* argv[])
{
	SGL_Init();
	SGL_Window window = SGL_CreateWindow("SGLDemo", 3, 3, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 576, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SGL_Camera* camera0 = SGL_CreateCamera(&window.rContext, SGL_CAMERA_TYPE_ORTHO, SGL_CAMERA_ORTHO, 0.1f, 2.0f, degreesToRadians(45.0f), 1.0f);
	SGL_BindCamera(&window.rContext, SGL_CAMERA_ORTHO);
	SGL_DataSelect("Demo_data");
	U32 format = 390076419;
	SDL_Surface* surf = SGL_DataLoadImage("spriteGridTex");
	SGL_Tex2D tex = { surf->pixels, 0, GL_BGRA, GL_RGBA8, surf->w, surf->h };
	SGL_CreateTextures(&tex, &SGL_ParamsNearest, 1);
	U32 spriteMaxCount = 10000;
	U32 spriteCount = 0;
	float speed = 128.0f;
	float spriteSize = 128.0f;
	float spriteSizeHalf = 64.0f;
	bool anim = false;
	float animSpeed = 0.1f;
	SGL_SimpleSpriteRenderer ssr = SGL_CreateSimpleSpriteRenderer(spriteMaxCount, &tex, &window.rContext);
	SGL_TexRegion texReg;
	SGL_Vec2 * scaleVecs = (SGL_Vec2*)SDL_malloc(sizeof(SGL_Vec2)*spriteMaxCount);
	SGL_Vec2 * posVecs = (SGL_Vec2*)SDL_malloc(sizeof(SGL_Vec2)*spriteMaxCount);
	SGL_Vec2 * directions = (SGL_Vec2*)SDL_malloc(sizeof(SGL_Vec2)*spriteMaxCount);
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
	}
	float animTimer = 0;
	float spriteOffset = 0;
	float gamma = 0.0f;
	bool quit = false;
	U32 debug = 1;
	float timeDelta = SDL_GetTicks();
	U32 time = SDL_GetTicks();
	//U32 timer = SDL_GetTicks();
	while (!quit)
	{
		timeDelta = float(SDL_GetTicks() - time)*0.001f;
		time = SDL_GetTicks();
		SDL_Event e = SDL_Event();
		while(SDL_PollEvent(&e))
		{
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
				switch (e.key.keysym.sym)
				{
				case SDLK_KP_4:
				{
					anim = !anim;
					break;
				}
				case SDLK_KP_7:
				{
					debug = !debug;
					break;
				}
				default:
					break;
				}
			}
			case SDL_KEYDOWN:
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_KP_0:
				{
					spriteCount = 0;
					break;
				}
				case SDLK_KP_1:
				{
					spriteCount = spriteMaxCount;
					break;
				}
				case SDLK_KP_2:
				{
					speed += 10.0f;
					break;
				}
				case SDLK_KP_3:
				{
					speed -= 10.0f;
					break;
				}
				case SDLK_KP_5:
				{
					gamma = gamma + 0.1f < 1.0f ? gamma + 0.1f : 1.0f;
					break;
				}
				case SDLK_KP_6:
				{
					gamma = gamma - 0.1f > -1.0f ? gamma - 0.1f : -1.0f;
					break;
				}
				case SDLK_KP_PLUS:
				{
					spriteCount = (spriteCount + 1) % spriteMaxCount;
					break;
				}
				case SDLK_KP_MINUS:
				{
					spriteCount = (spriteCount - 1) % spriteMaxCount;
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
		texReg = { 128.0f*spriteOffset,128.0f,128.0f, 128.0f };
		if (anim)
		{
			animTimer += timeDelta;
			if (animTimer >= animSpeed)
			{
				animTimer = 0;
				spriteOffset++;
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
		SGL_StartRender(&window);
		SGL_SimpleSpriteRendererDraw(&ssr, &window.rContext, gamma, debug);
		SGL_EndRender(&window);
		SGL_CheckGLErrors();
		//SDL_Log("%ims", SDL_GetTicks() - timer);
		//timer = SDL_GetTicks();
	}
	SGL_DestroyWindow(&window);
	SGL_Quit();
	return EXIT_SUCCESS;
}