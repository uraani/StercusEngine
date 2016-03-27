#pragma once
#include "SGL_math.h"
typedef struct _SGL_TexParams
{
	U32 wrapModeS;		//Wrap mode for the X axis (U). Possible values: GL_REPEAT(DEFAULT), GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
	U32 wrapModeT;		//Wrap mode for the Y axis (V). Possible values: GL_REPEAT(DEFAULT), GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
	U32 minFilter;		//Filtering mode when texture is displayed smaller than uniform size. Possible values: GL_LINEAR, GL_NEAREST
	U32 magFilter;		//Filtering mode when texture is displayed bigger than uniform size. Possible values: GL_LINEAR, GL_NEAREST
	F32 borderColor[4];		//borderColor if CLAMP_TO_BORDER is used
}SGL_TexParams;					//Struct that holds OpenGL parameters that affect the visual properties and behavior of the texture
typedef struct _SGL_Tex2D
{
	void* data;
	U32 handle;
	U32 type;
	U32 width;
	U32 height;
} SGL_Tex2D;
extern SGL_TexParams SGL_ParamsLinear;
extern SGL_TexParams SGL_ParamsNearest;
extern void SGL_CreateTextures(SGL_Tex2D* textures, SGL_TexParams* params, U32 count);
