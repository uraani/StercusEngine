#pragma once
#include "SGL_math.h"
typedef struct _SGL_Tex2D
{
	void* data;
	U32 format;
	U32 filter;
	U32 levelCount;
	SGL_Vec2i16 levelDimensions[12];
} SGL_Tex2D;

SGL_Tex2D GenerateMipmaps(const SGL_Tex2D * Texture);
