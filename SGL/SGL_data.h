#pragma once
#include "SGL_types.h"
typedef struct SDL_Surface SDL_Surface;
//function also selects the datafile
extern U32 SGL_InitDataSystem(void);
extern U32 SGL_DataCreate(const char* filePath, unsigned __int32 seed);
extern U32 SGL_DataSelect(const char* filePath);
extern U32 SGL_DataSaveImage(const char* name, SDL_Surface* surf);
extern SDL_Surface* SGL_DataLoadImage(const char* imageName);
