#pragma once
#include "SGL_math.h"
typedef enum _SGL_CameraID
{
	SGL_CAMERA_GUI,
	SGL_CAMERA_ORTHO,
	SGL_CAMERA_PERSPECTIVE,
	SGL_CAMERA_LIGHT,
	SGL_CAMERA_CUSTOM1,
	SGL_CAMERA_CUSTOM2,
	SGL_CAMERA_CUSTOM3,
	SGL_CAMERA_CUSTOM4,
	SGL_CAMERA_COUNT
} SGL_CameraID;
typedef enum SGL_CameraType
{
	SGL_CAMERA_TYPE_UNUSED,
	SGL_CAMERA_TYPE_ORTHO,
	SGL_CAMERA_TYPE_LIGHT,
	SGL_CAMERA_TYPE_PERSPECTIVE
} SGL_CameraType;
typedef struct _SGL_Camera
{
	SGL_Mat4 vPMatrix;
	SGL_Quat rotation;
	SGL_Vec4 position;
	SGL_Vec2 forcedSize;
	F32 nearPlane;
	F32 farPlane;
	F32 FOWY;
	F32 scale;
	U32 camType;
} SGL_Camera;
typedef struct _SGL_RenderContext SGL_RenderContext;
extern SGL_Camera* SGL_CreateCamera(const unsigned int type, const unsigned int id, const float nearPlane, const float farPlane, const float FOWY, const float scale, SGL_RenderContext* rContext);