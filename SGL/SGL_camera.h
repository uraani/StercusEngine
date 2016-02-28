#pragma once
#include "SGL_math.h"
typedef enum SGL_CameraID
{
	SGL_CAMERA_GUI,
	SGL_CAMERA_ORTHO,
	SGL_CAMERA_PERSPECTIVE,
	SGL_CAMERA_CUSTOM0,
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
	SGL_CAMERA_TYPE_PERSPECTIVE
} SGL_CameraType;
struct SGL_Camera
{
	SGL_Mat4 vPMatrix;
	SGL_Vec4 rotation;
	SGL_Vec3 position;
	float nearPlane;
	float farPlane;
	float FOWY;
	float scale;
	U32 camType;
}typedef SGL_Camera;
typedef struct SGL_RenderContext
{
	SGL_Camera cameras[SGL_CAMERA_COUNT];
	SGL_Vec2 windowHalfSizef;
	SGL_Vec2i windowSize;
} SGL_RenderContext;

extern SGL_Camera* SGL_CreateCamera(SGL_RenderContext* rContext, const unsigned int type, const unsigned int id, const float near, const float far, const float FOWY, const float scale);