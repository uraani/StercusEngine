#include "SGL_data.h"
#include "SGL_vk.h"
#include "murmur3.h"
typedef struct _SGL_DataBlock
{
	U32 seed;
	U32 imageCount;
	U32 shaderCount;
	U32 modelCount;
} SGL_DataBlock;
typedef struct _SGL_FileBstNode
{
	U32 left;
	U32 right;
	U32 hash;
	U32 loc;
}SGL_FileBstNode;
/*typedef struct _SGL_BstNode SGL_BstNode;
typedef struct _SGL_BstNode
{
	SGL_BstNode* left;
	SGL_BstNode* right;
	U32 hash;
	U32 loc;
}SGL_BstNode;*/
typedef struct _SGL_Data
{
	SGL_DataBlock block;
//	SGL_BstNode* root;
	SGL_FileBstNode* root;
	char* path;
	char* headerPath;
	U32 treeSize;
}SGL_Data;

typedef struct _SGL_FileSurface
{
	U32 flags;
	I32 w, h;
	I32 depth;
	U32 Rmask;
	U32 Gmask;
	U32 Bmask;
	U32 Amask;
}SGL_FileSurface;
SGL_Data targetData;
inline U32 AddNode(const U32 hash, const U32 loc)
{
	SDL_RWops* rw = SDL_RWFromFile(targetData.headerPath, "rb+");
	if (rw == NULL)
	{
		SDL_Log("AddNode couldn't open file in path: %s, SDL_Error: %s", targetData.headerPath, SDL_GetError());
		return SGL_FALSE;
	}
	targetData.root = SDL_realloc(targetData.root,(targetData.treeSize+1)*sizeof(SGL_FileBstNode));
	targetData.root[targetData.treeSize].left = 0;
	targetData.root[targetData.treeSize].right = 0;
	targetData.root[targetData.treeSize].hash = hash;
	targetData.root[targetData.treeSize].loc = loc;
	U32 temp = 0;
	U32* lastTemp = NULL;
	U32 location = 0;
	if (targetData.treeSize > 0)
	{
		do
		{
			if (hash < targetData.root[temp].hash)
			{
				lastTemp = &targetData.root[temp].left;
				temp = targetData.root[temp].left;
			}
			else
			{
				lastTemp = &targetData.root[temp].right;
				temp = targetData.root[temp].right;
			}
		} while (temp != 0);
		*lastTemp = targetData.treeSize;
		location = (U32)((U8*)lastTemp - (U8*)targetData.root);
		/*
		In x64 the pointers are 8 bytes each so sizeof(SGL_BstNode) != sizeof(SGL_FileBstNode)
		this line fixes the alignment issues that might occur on x64
		*/
		//location = location / sizeof(SGL_BstNode) + (location % sizeof(SGL_BstNode)) / sizeof(SGL_BstNode*) * sizeof(U32);
		SDL_RWseek(rw, location, RW_SEEK_SET);
		if (SDL_RWwrite(rw, &targetData.treeSize, sizeof(U32), 1) != 1)
		{
			SDL_RWclose(rw);
			SDL_Log("AddNode couldn't write data to file in path: %s", targetData.headerPath);
			return SGL_FALSE;
		}
	}
	SDL_RWseek(rw, 0, RW_SEEK_END);
	SGL_FileBstNode node = { 0, 0, hash, loc };
	if (SDL_RWwrite(rw, &node, sizeof(SGL_FileBstNode), 1) != 1)
	{
		SDL_RWclose(rw);
		SDL_Log("AddNode couldn't write data to file in path: %s", targetData.headerPath);
		return SGL_FALSE;
	}
	targetData.treeSize++;
	SDL_RWclose(rw);
	return SGL_TRUE;
}
inline U32 FindNodeLoc(const U32 hash)
{
	U32 temp = 0;
	do
	{
		if (hash < targetData.root[temp].hash)
		{
			temp = targetData.root[temp].left;
		}
		else
		{
			temp = targetData.root[temp].right;
		}
	} while (temp != 0 && targetData.root[temp].hash != hash);
	if (targetData.root[temp].hash == hash)
	{
		return targetData.root[temp].loc;
	}
	return SGL_FALSE;
}

U32 SGL_InitDataSystem(void)
{
	targetData.path = NULL;
	targetData.headerPath = NULL;
	targetData.root = NULL;
	targetData.treeSize = 0;
	return SGL_TRUE;
}


U32 SGL_DataCreate(const char * filePath, unsigned __int32 seed)
{
	if (targetData.path != NULL);
	{
		SDL_free(targetData.path);
	}
	if (targetData.headerPath != NULL);
	{
		SDL_free(targetData.headerPath);
	}
	const size_t len = SDL_strlen(filePath);
	targetData.path = SDL_malloc(len + sizeof(char) * 4); //four chars in ".sd\0"
	size_t i;
	for (i = 0; i < len; i++)
	{
		targetData.path[i] = filePath[i];
	}
	targetData.path[len] = '.';
	targetData.path[len + 1] = 's';
	targetData.path[len + 2] = 'd';
	targetData.path[len + 3] = 0;
	targetData.headerPath = SDL_malloc(len + sizeof(char) * 5); //five chars in ".sdh\0"
	for (i = 0; i < len; i++)
	{
		targetData.headerPath[i] = filePath[i];
	}
	targetData.headerPath[len] = '.';
	targetData.headerPath[len + 1] = 's';
	targetData.headerPath[len + 2] = 'd';
	targetData.headerPath[len + 3] = 'h';
	targetData.headerPath[len + 4] = 0;
	SDL_RWops* rw;
	{
		rw = SDL_RWFromFile(targetData.path, "w");
		if (rw == NULL)
		{
			SDL_Log("SGL_CreateDataFile couldn't create file in path: %s, SDL_Error:%s", targetData.path, SDL_GetError());
			return SGL_FALSE;
		}
		SGL_DataBlock data = { seed, 0,0,0 };
		const size_t size = sizeof(SGL_DataBlock);
		if (SDL_RWwrite(rw, &data, size, 1) != 1)
		{
			SDL_RWclose(rw);
			SDL_Log("SGL_CreateDataFile couldn't write to data file in path: %s", targetData.path);
			return SGL_FALSE;
		}
		SDL_RWclose(rw);
		targetData.block = data;
	}
	rw = SDL_RWFromFile(targetData.headerPath, "w");
	if (rw == NULL)
	{
		SDL_Log("SGL_CreateDataFile couldn't create file in path: %s, SDL_Error:%s", targetData.headerPath, SDL_GetError());
		return SGL_FALSE;
	}
	SDL_RWclose(rw);
	return SGL_TRUE;
}

U32 SGL_DataSelect(const char * filePath)
{
	if (targetData.path != NULL);
	{
		SDL_free(targetData.path);
	}
	if (targetData.headerPath != NULL);
	{
		SDL_free(targetData.headerPath);
	}
	const size_t len = SDL_strlen(filePath);
	targetData.path = SDL_malloc(len + sizeof(char) * 4); //four chars in ".sd\0"
	size_t i;
	for (i = 0; i < len; i++)
	{
		targetData.path[i] = filePath[i];
	}
	targetData.path[len] = '.';
	targetData.path[len + 1] = 's';
	targetData.path[len + 2] = 'd';
	targetData.path[len + 3] = 0;
	targetData.headerPath = SDL_malloc(len + sizeof(char) * 5); //five chars in ".sdh\0"
	for (i = 0; i < len; i++)
	{
		targetData.headerPath[i] = filePath[i];
	}
	targetData.headerPath[len] = '.';
	targetData.headerPath[len + 1] = 's';
	targetData.headerPath[len + 2] = 'd';
	targetData.headerPath[len + 3] = 'h';
	targetData.headerPath[len + 4] = 0;
	SDL_RWops* rw;
	{
		rw = SDL_RWFromFile(targetData.path, "rb");
		if (rw == NULL)
		{
			SDL_Log("SGL_DataSelect couldn't open file in path: %s, SDL_Error:%s", targetData.path, SDL_GetError());
			return SGL_FALSE;
		}
		const size_t size = sizeof(SGL_DataBlock);
		if (SDL_RWread(rw, &targetData.block, size, 1) != 1)
		{
			SDL_RWclose(rw);
			SDL_Log("SGL_DataSelect couldn't read data from file in path: %s", targetData.path);
			return SGL_FALSE;
		}
		SDL_RWclose(rw);
	}
	rw = SDL_RWFromFile(targetData.headerPath, "rb");
	if (rw == NULL)
	{
		SDL_Log("SGL_DataSelect couldn't open file in path: %s, SDL_Error:%s", targetData.headerPath, SDL_GetError());
		return SGL_FALSE;
	}
	size_t nodeCount = SDL_RWseek(rw, 0, RW_SEEK_END);
	if (nodeCount != 0)
	{
		targetData.root = malloc(nodeCount);
		nodeCount /= sizeof(SGL_FileBstNode);
		targetData.treeSize = (U32)nodeCount;
		SDL_RWseek(rw, 0, RW_SEEK_SET);
		if (SDL_RWread(rw, targetData.root, sizeof(SGL_FileBstNode), nodeCount) != nodeCount)
		{
			SDL_RWclose(rw);
			SDL_Log("SGL_DataSelect couldn't read data from file in path: %s", targetData.headerPath);
			return SGL_FALSE;
		}
	}
	SDL_RWclose(rw);
	return SGL_TRUE;
}

U32 SGL_DataSaveImage(const char* name, SDL_Surface * surf)
{
	U32 loc;
	U32 hash;
	SDL_Surface* convSurf;
	if (surf == NULL)
	{
		SDL_Log("SGL_DataSaveImage couldn't save because surf pointer is NULL");
		return SGL_FALSE;
	}
	if (SGL_PixelFormatToVkFormat(surf->format->format) != VK_FORMAT_UNDEFINED)
	{
		convSurf = surf;
	}
	else if (surf->format->BytesPerPixel <= 4)
	{
		//SDL_PIXELFORMAT_ARGB8888 == VK_FORMAT_B8G8R8A8_UNORM
		//I havent had time to check why the SDL pixel format is reversed in comparison to the vulkan format
		//27.2.2016 Vulkan doesnt currently support 3 component pixel formats, actually the only format i found working was mentioned above
		SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
		convSurf = SDL_ConvertSurface(surf, format, 0);
		SDL_FreeFormat(format);
		if (convSurf == NULL)
		{
			SDL_Log("SGL_DataSaveImage couldn't convert surface to SDL_PIXELFORMAT_ARGB8888/VK_FORMAT_B8G8R8A8_UNORM, you could maybe add unimplemented format if vulkan supports it?");
			return SGL_FALSE;
		}
	}
	else
	{
		SDL_Log("SGL_DataSaveImage encountered unsupported pixel format. Aborting save");
		return SGL_FALSE;
	}
	targetData.block.imageCount++;
	{
		SDL_RWops* rw = SDL_RWFromFile(targetData.path, "rb+");
		if (rw == NULL)
		{
			SDL_Log("SGL_DataSaveImage couldn't open file in path: %s, SDL_Error:%s", targetData.path, SDL_GetError());
			return SGL_FALSE;
		}
		loc = (U32)SDL_RWseek(rw, 0, RW_SEEK_END);
		const SGL_FileSurface surfData = 
		{ 
			convSurf->flags, 
			convSurf->w, convSurf->h,
			convSurf->format->BitsPerPixel, 
			convSurf->format->Rmask, convSurf->format->Gmask, convSurf->format->Bmask, convSurf->format->Amask
		};
		if (SDL_RWwrite(rw, &surfData, sizeof(SGL_FileSurface), 1) != 1)
		{
			SDL_RWclose(rw);
			SDL_Log("SGL_DataSaveImage couldn't write to data file in path: %s", targetData.path);
			return SGL_FALSE;
		}
		{
			I32 y;
			const U32 pixelSize = convSurf->format->BytesPerPixel;
			//y axis gets flipped here
			//this prolly hurts the writing speed but this is done only when saving the textures
			for (y = surfData.h-1; y >= 0; y--)
			{
				I32 x;
				for (x = 0; x < surfData.w; x++)
				{
					SDL_RWwrite(rw, ((U8*)convSurf->pixels)+(y*surfData.w*pixelSize)+(x*pixelSize), pixelSize, 1);
				}
			}
		}
		SDL_RWseek(rw, 0, RW_SEEK_SET);
		if (SDL_RWwrite(rw, &targetData.block, sizeof(SGL_DataBlock), 1) != 1)
		{
			SDL_RWclose(rw);
			SDL_Log("SGL_DataSaveImage couldn't write to data file in path: %s, SDL_Error: %s", targetData.path, SDL_GetError());
			return SGL_FALSE;
		}
		SDL_RWclose(rw);
	}
	MurmurHash3_x86_32(name, (I32)SDL_strlen(name), targetData.block.seed, &hash);
	return AddNode(hash, loc);
}
U32 SGL_DataSaveShader(const char * filePath, const char* name)
{
	U32 loc;
	U32 hash;
	SDL_RWops* rw;
	U32 size;
	U8* shaderData;
	{
		rw = SDL_RWFromFile(filePath, "rb");
		if (rw == NULL)
		{
			SDL_Log("SGL_DataSaveShader couldn't open file in path: %s, SDL_Error:%s", filePath, SDL_GetError());
			return SGL_FALSE;
		}
		size = SDL_RWseek(rw, 0, RW_SEEK_END);
		shaderData = SDL_malloc(size);
		SDL_RWseek(rw, 0, RW_SEEK_SET);
		if (SDL_RWread(rw, shaderData, size, 1) != 1)
		{
			SDL_RWclose(rw);
			SDL_Log("SGL_DataSaveShader couldn't read data from file in path: %s", filePath);
			return SGL_FALSE;
		}
		SDL_RWclose(rw);
	}
	targetData.block.shaderCount++;
	{
		rw = SDL_RWFromFile(targetData.path, "rb+");
		if (rw == NULL)
		{
			SDL_Log("SGL_DataSaveShader couldn't open file in path: %s, SDL_Error:%s", targetData.path, SDL_GetError());
			return SGL_FALSE;
		}
		loc = (U32)SDL_RWseek(rw, 0, RW_SEEK_END);
		if (SDL_RWwrite(rw, &size, sizeof(U32), 1) != 1)
		{
			SDL_RWclose(rw);
			SDL_Log("SGL_DataSaveShader couldn't write to data file in path: %s", targetData.path);
			return SGL_FALSE;
		}
		if (SDL_RWwrite(rw, shaderData, size, 1) != 1)
		{
			SDL_RWclose(rw);
			SDL_Log("SGL_DataSaveShader couldn't write to data file in path: %s", targetData.path);
			return SGL_FALSE;
		}
		SDL_RWseek(rw, 0, RW_SEEK_SET);
		if (SDL_RWwrite(rw, &targetData.block, sizeof(SGL_DataBlock), 1) != 1)
		{
			SDL_RWclose(rw);
			SDL_Log("SGL_DataSaveShader couldn't write to data file in path: %s, SDL_Error: %s", targetData.path, SDL_GetError());
			return SGL_FALSE;
		}
		SDL_RWclose(rw);
	}
	MurmurHash3_x86_32(name, (I32)SDL_strlen(name), targetData.block.seed, &hash);
	return AddNode(hash, loc);
	return SGL_TRUE;
}
SDL_Surface * SGL_DataLoadImage(const char * imageName)
{
	U32 hash;
	MurmurHash3_x86_32(imageName, (I32)SDL_strlen(imageName), targetData.block.seed, &hash);
	U32 loc = FindNodeLoc(hash);
	if (loc == SGL_FALSE)
	{
		return NULL;
	}
	SDL_RWops* rw = SDL_RWFromFile(targetData.path, "rb");
	if (rw == NULL)
	{
		SDL_Log("SGL_DataLoadImage couldn't open file in path: %s, SDL_Error:%s", targetData.path, SDL_GetError());
		return NULL;
	}
	SDL_RWseek(rw, loc, RW_SEEK_SET);
	SGL_FileSurface surfData;
	if (SDL_RWread(rw, &surfData, sizeof(SGL_FileSurface), 1) != 1)
	{
		SDL_RWclose(rw);
		SDL_Log("SGL_DataSaveImage couldn't read data from file in path: %s", targetData.path);
		return NULL;
	}
	const size_t size = surfData.h * surfData.w * (surfData.depth/8);
	void* pixels = SDL_malloc(size);
	if (SDL_RWread(rw, pixels, size, 1) != 1)
	{
		SDL_RWclose(rw);
		SDL_Log("SGL_DataSaveImage couldn't read data from file in path: %s", targetData.path);
		return NULL;
	}
	return SDL_CreateRGBSurfaceFrom(pixels, surfData.w, surfData.h, surfData.depth, surfData.w*(surfData.depth / 8), surfData.Rmask, surfData.Gmask, surfData.Bmask, surfData.Amask);
}

SGL_Shader SGL_DataLoadShader(const char * shaderName)
{
	SGL_Shader shader =
	{
		.data = NULL,
		.size = 0
	};
	U32 hash;
	U32 loc;
	MurmurHash3_x86_32(shaderName, (I32)SDL_strlen(shaderName), targetData.block.seed, &hash);
	loc = FindNodeLoc(hash);
	if (loc == SGL_FALSE)
	{
		return shader;
	}
	SDL_RWops* rw = SDL_RWFromFile(targetData.path, "rb");
	if (rw == NULL)
	{
		SDL_Log("SGL_DataLoadShader couldn't open file in path: %s, SDL_Error:%s", targetData.path, SDL_GetError());
		return shader;
	}
	SDL_RWseek(rw, loc, RW_SEEK_SET);
	if (SDL_RWread(rw, &shader.size, sizeof(U32), 1) != 1)
	{
		SDL_RWclose(rw);
		shader.size = 0;
		SDL_Log("SGL_DataLoadShader couldn't read data from file in path: %s", targetData.path);
		return shader;
	}
	//SDL_RWseek(rw, loc + sizeof(U32), RW_SEEK_SET);
	shader.data = SDL_malloc(shader.size);
	if (SDL_RWread(rw, shader.data, 1, shader.size) != shader.size)
	{
		SDL_RWclose(rw);
		SDL_free(shader.data);
		shader.data = NULL;
		shader.size = 0;
		SDL_Log("SGL_DataLoadShader couldn't read data from file in path: %s", targetData.path);
		return shader;
	}
	SDL_RWclose(rw);
	return shader;
}
