#pragma once
#include "SGL_types.h"
typedef struct _SGL_BMFontHeader
{
	I16 fontSize;
	U8 bitField;
	U8 charSet;

	U16 stretchH;
	U8 aa;
	U8 paddingUp;

	U8 paddingRight;
	U8 paddingDown;
	U8 paddingLeft;
	U8 spacingHoriz;

	U8 spacingVert;
	U8 outline;
	U8 pad0;
	U8 pad1;
};