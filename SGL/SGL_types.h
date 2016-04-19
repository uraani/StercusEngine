#pragma once
#ifndef _SGL_types_h
#define _SGL_types_h
#define SGL_BUFFER_COUNT_MAX 3
typedef unsigned __int8  U8;
typedef unsigned __int16 U16;
typedef unsigned __int32 U32;
typedef unsigned __int64 U64;
typedef __int8  I8;
typedef __int16 I16;
typedef __int32 I32;
typedef __int64 I64;
typedef float F32;
typedef double F64;
typedef enum
{
	SGL_FALSE = 0,
	SGL_TRUE = 1
} SGL_bool;
typedef enum
{
	SGL_TRIANGLE = 3,
	SGL_QUAD = 4
} SGL_Geom;
typedef struct _SGL_Color
{
	U8 r,g,b,a;
}SGL_Color;
typedef struct _SGL_Mesh
{
	void* vertexData;
	U32* indexData;
	U32 vertexCount;
	U32 indexCount;
} SGL_Mesh;
typedef struct _SGL_SMesh
{
	void* vertexData;
	U16* indexData;
	U32 vertexCount;
	U32 indexCount;
} SGL_SMesh;
typedef struct _SGL_VAO
{
	//VAO id
	//contains attribute and buffer binding data
	U32 handle;
	//Vertex buffer object id
	//contains vertexData
	U32 VBO;
	//Element buffer object id
	//contains indexData
	U32 EBO;
} SGL_VAO;
typedef struct _SGL_PointVAO
{
	//VAO id
	//contains attribute and buffer binding data
	U32 handle;
	//Vertex buffer object id
	//contains vertexData
	U32 VBO;
} SGL_PointVAO;
#endif //_SGL_types_h