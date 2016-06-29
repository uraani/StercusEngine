#pragma once
#include "SGL_types.h"
#ifndef _SGL_math_h
#define _SGL_math_h
#include "SDL.h"
#include <smmintrin.h>
#define degreesToRadians(angleDegrees) ((F32)((F64)angleDegrees * (M_PI / 180.0)))
#define isPowerOfTwo(x) ((x & (x - 1)) == 0);
#if !defined(ANDROID)
__declspec(align(16))
#else
#pragma message ("CREATE ARM NEON IMPLEMENTATIONS FOR VEC4")
#endif
typedef struct _SGL_Vec4
{
	union
	{
		struct
		{
			F32 f[4];
		};
		struct
		{
			F32 x;
			F32 y;
			F32 z;
			F32 w;
		};
#if !defined(ANDROID)
		struct
		{
			__m128 v;
		};
#endif
	};
} SGL_Vec4;
#if !defined(ANDROID)
__declspec(align(16))
#else
#pragma message ("CREATE ARM NEON IMPLEMENTATIONS FOR VEC4")
#endif
typedef struct _SGL_Quat
{
	union
	{
		struct
		{
			F32 f[4];
		};
		struct
		{
			F32 x;
			F32 y;
			F32 z;
			F32 w;
		};
#if !defined(ANDROID)
		struct
		{
			__m128 v;
		};
#endif
	};
} SGL_Quat;
typedef struct _SGL_Vec3
{
	float x;
	float y;
	float z;
} SGL_Vec3;
typedef struct _SGL_Vec2
{
	F32 x;
	F32 y;
} SGL_Vec2;
typedef struct _SGL_Vec2i16
{
	I16 x;
	I16 y;
} SGL_Vec2i16;
typedef struct _SGL_Vec2i
{
	I32 x;
	I32 y;
} SGL_Vec2i;
#if !defined(ANDROID)
__declspec(align(16))
#else
#pragma message ("CREATE ARM NEON IMPLEMENTATIONS FOR MAT4")
#endif
typedef struct _SGL_Mat4
{
	union
	{
		struct
		{
			F32 m00, m01, m02, m03;
			F32 m10, m11, m12, m13;
			F32 m20, m21, m22, m23;
			F32 m30, m31, m32, m33;
		};
		struct
		{
			F32 m[16];
		};
		struct
		{
			SGL_Vec4 v0, v1, v2, v3;
		};
#if !defined(ANDROID)
		struct
		{

			__m128 m0, m1, m2, m3;
		};
#endif
	};
} SGL_Mat4;
#if !defined(ANDROID)
__declspec(align(16))
#else
#pragma message ("CREATE ARM NEON IMPLEMENTATIONS FOR Vec2x4")
#endif
typedef struct _SGL_Vec2x4
{
	struct
	{
		F32 f[8];
	};
	struct
	{
		SGL_Vec2 v[4];
	};
#if !defined(ANDROID)
	struct
	{

		__m128 v0, v1;
	};
#endif
}SGL_Vec2x4;

//MATRIX INDEXES ARE ARRANGED LIKE THIS:
//[0 ][1 ][2 ] [UNUSED]
//[4 ][5 ][6 ] [UNUSED]
//[8 ][9 ][10] [UNUSED]
#if !defined(ANDROID)
__declspec(align(16))
#else
#pragma message ("CREATE ARM NEON IMPLEMENTATIONS FOR MAT3")
#endif
typedef struct _SGL_Mat3
{
	union
	{
		struct
		{
			F32 m00, m01, m02, m03;
			F32 m10, m11, m12, m13;
			F32 m20, m21, m22, m23;
		};
		struct
		{
			F32 m[12];
		};
		struct
		{
			SGL_Vec4 v0, v1, v2;
		};
#if !defined(ANDROID)
		struct
		{

			__m128 m0, m1, m2;
		};
#endif
	};

} SGL_Mat3;
typedef struct _SGL_SquareTexRegion
{
	SGL_Vec2 offset;
	float size;
} SGL_SquareTexRegion;
typedef struct _SGL_TexRegion
{
	SGL_Vec2 offset;
	SGL_Vec2 size;
} SGL_TexRegion;
typedef struct _SGL_Trans2D
{
	SGL_Mat3 mat;
	SGL_Vec2 position;
	SGL_Vec2 scale;
	F32 rotation;
	//currently unused
	U32 state;
}SGL_Trans2D;
inline const SGL_Mat3 SM_IdentityMat3()
{
	SGL_Mat3 r;
	r.m00 = 1.0f; r.m01 = 0.0f; r.m02 = 0.0f; r.m03 = 0.0f;
	r.m10 = 0.0f; r.m11 = 1.0f; r.m12 = 0.0f; r.m13 = 0.0f;
	r.m20 = 0.0f; r.m21 = 0.0f; r.m22 = 1.0f; r.m23 = 0.0f;
	return r;
}
inline const SGL_Mat4 SM_IdentityMat4()
{
	SGL_Mat4 r;
	r.m00 = 1.0f; r.m01 = 0.0f; r.m02 = 0.0f; r.m03 = 0.0f;
	r.m10 = 0.0f; r.m11 = 1.0f; r.m12 = 0.0f; r.m13 = 0.0f;
	r.m20 = 0.0f; r.m21 = 0.0f; r.m22 = 1.0f; r.m23 = 0.0f;
	r.m30 = 0.0f; r.m31 = 0.0f; r.m32 = 0.0f; r.m33 = 1.0f;
	return r;
}
inline const SGL_Vec4 SM_IdentityVec4()
{
	SGL_Vec4 r;
	r.x = 0.0f;
	r.y = 0.0f;
	r.z = 0.0f;
	r.w = 1.0f;
	return r;
}
inline const SGL_Mat4 SM_QuatToMat4(const SGL_Vec4* q)
{
	SGL_Mat4 r;
	const F32 qxx = q->x * q->x;
	const F32 qyy = q->y * q->y;
	const F32 qzz = q->z * q->z;
	const F32 qxz = q->x * q->z;
	const F32 qxy = q->x * q->y;
	const F32 qyz = q->y * q->z;
	const F32 qwx = q->w * q->x;
	const F32 qwy = q->w * q->y;
	const F32 qwz = q->w * q->z;
	r.m00 = 1.0f - 2.0f * (qyy + qzz);
	r.m01 = 2.0f * (qxy + qwz);
	r.m02 = 2.0f * (qxz - qwy);
	r.m03 = 0.0f;

	r.m10 = 2.0f * (qxy - qwz);
	r.m11 = 1.0f - 2.0f * (qxx + qzz);
	r.m12 = 2.0f * (qyz + qwx);
	r.m13 = 0.0f;

	r.m20 = 2.0f * (qxz + qwy);
	r.m21 = 2.0f * (qyz - qwx);
	r.m22 = 1.0f - 2.0f * (qxx + qyy);
	r.m23 = 0.0f;

	r.m30 = 0.0f;
	r.m31 = 0.0f;
	r.m32 = 0.0f;
	r.m33 = 1.0f;
	return r;

}
inline const SGL_Vec2 SM_V2M3Multiply(const SGL_Vec2 b, const SGL_Mat3* a)
{
	SGL_Vec2 r;
	const float i00 = a->m00 * b.x + a->m01 * b.y + a->m02; 
	const float i01 = a->m10 * b.x + a->m11 * b.y + a->m12;
	r.x = i00;
	r.y = i01;
	return r;
}
inline const SGL_Vec2 SM_M3V2Multiply(const SGL_Mat3* a, const SGL_Vec2 b)
{
	const SGL_Vec2 r = 
	{
		a->m00 * b.x + a->m10 * b.y + a->m20,
		a->m01 * b.x + a->m11 * b.y + a->m21,
	};
	//const float i00 = a->m00 * b.x + a->m10 * b.y + a->m20;
	//const float i01 = a->m01 * b.x + a->m11 * b.y + a->m21;
	//r.x = i00;
	//r.y = i01;
	return r;
}
inline const SGL_Vec4 SM_M4V4Multiply(const SGL_Mat4* a, const SGL_Vec4* b)
{
	SGL_Vec4 r;
#if defined(ANDROID)
	const float i00 = a->m00 * b->x + a->m01 * b->y + a->m02 * b->z + a->m03 * b->w;
	const float i01 = a->m10 * b->x + a->m11 * b->y + a->m12 * b->z + a->m13 * b->w;
	const float i02 = a->m20 * b->x + a->m21 * b->y + a->m22 * b->z + a->m23 * b->w;
	const float i03 = a->m30 * b->x + a->m31 * b->y + a->m32 * b->z + a->m33 * b->w;
	r.x = i00;
	r.y = i01;
	r.z = i02;
	r.w = i03;
#else
	const SGL_Vec4 t0 = { a->m00, a->m10, a->m20, a->m30 };
	const SGL_Vec4 t1 = { a->m01, a->m11, a->m21, a->m31 };
	const SGL_Vec4 t2 = { a->m02, a->m12, a->m22, a->m32 };
	const SGL_Vec4 t3 = { a->m03, a->m13, a->m23, a->m33 };
	const __m128 v0 = _mm_load_ps((F32*)&t0);
	const __m128 v1 = _mm_load_ps((F32*)&t1);
	const __m128 v2 = _mm_load_ps((F32*)&t2);
	const __m128 v3 = _mm_load_ps((F32*)&t3);
	__m128 tmp0, tmp1;
	tmp0 = _mm_set1_ps(b->x);
	tmp1 = _mm_mul_ps(v0, tmp0);

	tmp0 = _mm_set1_ps(b->y);
	tmp0 = _mm_mul_ps(v1, tmp0);
	tmp1 = _mm_add_ps(tmp0, tmp1);

	tmp0 = _mm_set1_ps(b->z);
	tmp0 = _mm_mul_ps(v2, tmp0);
	tmp1 = _mm_add_ps(tmp0, tmp1);

	tmp0 = _mm_set1_ps(b->w);
	tmp0 = _mm_mul_ps(v3, tmp0);
	tmp1 = _mm_add_ps(tmp0, tmp1);

	_mm_store_ps(&r.x, tmp1);
#endif
	return r;
}
/*inline const SGL_Vec2x4 SM_M3V2X4Multiply(const SGL_Mat3* a, const SGL_Vec4* x, const SGL_Vec4* y)
{
	SGL_Vec2x4 r;
	__m128 t1, t2;
	SGL_Vec4 arr[2];
	t1 = _mm_set1_ps(a->m00);
	t2 = _mm_mul_ps(x->v, t1);
	t1 = _mm_set1_ps(a->m01);
	t2 = _mm_add_ps(_mm_mul_ps(y->v, t1), t2);
	t1 = _mm_set1_ps(a->m20);
	t2 = _mm_add_ps(t1, t2);
	_mm_store_ps(arr[0].f, t2);
	t1 = _mm_set1_ps(a->m10);
	t2 = _mm_mul_ps(x->v, t1);
	t1 = _mm_set1_ps(a->m11);
	t2 = _mm_add_ps(_mm_mul_ps(y->v, t1), t2);
	t1 = _mm_set1_ps(a->m21);
	t2 = _mm_add_ps(t1, t2);
	_mm_store_ps(arr[1].f, t2);
	for (size_t i = 0; i < 4; i++)
	{
		r.v[i].x = arr[0].f[i];
		r.v[i].y = arr[1].f[i];
	}
	return r;
}
inline const SGL_Vec2x4 SM_M3V2X4MultiplyTest(const SGL_Mat3* a)
{
	SGL_Vec2x4 r;
	const SGL_Vec4 x0 = { 0.5f, 0.5f, -0.5f, -0.5f };
	const SGL_Vec4 x1 = { -0.5f, -0.5f, 0.5f, 0.5f };
	const SGL_Vec4 y0 = { 0.5f,  0.5f,  0.5f,  0.5f};
	const SGL_Vec4 y1 = { -0.5f, -0.5f, -0.5f, -0.5f};
	const SGL_Vec4 col0 = { a->m00, a->m10, a->m00, a->m10 };
	const SGL_Vec4 col1 = { a->m01, a->m11, a->m01, a->m11 };
	const SGL_Vec4 col2 = { a->m20, a->m21, a->m20, a->m21 };
	__m128 t0;
	t0 = _mm_mul_ps(col0.v, x0.v);
	t0 = _mm_add_ps(_mm_mul_ps(y0.v, col1.v), t0);
	t0 = _mm_add_ps(col2.v, t0);
	_mm_store_ps(&r.v[0].x, t0);
	t0 = _mm_mul_ps(col0.v, x1.v);
	t0 = _mm_add_ps(_mm_mul_ps(y1.v, col1.v), t0);
	t0 = _mm_add_ps(col2.v, t0);
	_mm_store_ps(&r.v[2].x, t0);
	return r;
}*/
inline const SGL_Mat3 SM_M3Inverse(const SGL_Mat3* a)
{
	SGL_Mat3 r;
	F32 OneOverDeterminant = 
		1.0f / 
		(
		+ a->m00 * (a->m11 * a->m22 - a->m21 * a->m12)
		- a->m10 * (a->m01 * a->m22 - a->m21 * a->m02)
		+ a->m20 * (a->m01 * a->m12 - a->m11 * a->m02)
		);
	r.m00 = +(a->m11 * a->m22 - a->m21 * a->m12) * OneOverDeterminant;
	r.m10 = -(a->m10 * a->m22 - a->m20 * a->m12) * OneOverDeterminant;
	r.m20 = +(a->m10 * a->m21 - a->m20 * a->m11) * OneOverDeterminant;
	r.m01 = -(a->m01 * a->m22 - a->m21 * a->m02) * OneOverDeterminant;
	r.m11 = +(a->m00 * a->m22 - a->m20 * a->m02) * OneOverDeterminant;
	r.m21 = -(a->m00 * a->m21 - a->m20 * a->m01) * OneOverDeterminant;
	r.m02 = +(a->m01 * a->m12 - a->m11 * a->m02) * OneOverDeterminant;
	r.m12 = -(a->m00 * a->m12 - a->m10 * a->m02) * OneOverDeterminant;
	r.m22 = +(a->m00 * a->m11 - a->m10 * a->m01) * OneOverDeterminant;
	return r;
}
inline const SGL_Mat3 SM_M3Multiply(const SGL_Mat3* a, const SGL_Mat3* b)
{
	SGL_Mat3 r;
#if defined(ANDROID)
	const float i00 = a->m00 * b->m00 + a->m01 * b->m10 + a->m02 * b->m20;
	const float i01 = a->m00 * b->m01 + a->m01 * b->m11 + a->m02 * b->m21;
	const float i02 = a->m00 * b->m02 + a->m01 * b->m12 + a->m02 * b->m22;
	const float i03 = a->m00 * b->m03 + a->m01 * b->m13 + a->m02 * b->m23;
	const float i10 = a->m10 * b->m00 + a->m11 * b->m10 + a->m12 * b->m20;
	const float i11 = a->m10 * b->m01 + a->m11 * b->m11 + a->m12 * b->m21;
	const float i12 = a->m10 * b->m02 + a->m11 * b->m12 + a->m12 * b->m22;
	const float i13 = a->m10 * b->m03 + a->m11 * b->m13 + a->m12 * b->m23;
	const float i20 = a->m20 * b->m00 + a->m21 * b->m10 + a->m22 * b->m20;
	const float i21 = a->m20 * b->m01 + a->m21 * b->m11 + a->m22 * b->m21;
	const float i22 = a->m20 * b->m02 + a->m21 * b->m12 + a->m22 * b->m22;
	const float i23 = a->m20 * b->m03 + a->m21 * b->m13 + a->m22 * b->m23;
	r.m00 = i00;
	r.m01 = i01;
	r.m02 = i02;
	r.m03 = 0.0f;
	r.m10 = i10;
	r.m11 = i11;
	r.m12 = i12;
	r.m13 = 0.0f;
	r.m20 = i20;
	r.m21 = i21;
	r.m22 = i22;
	r.m23 = 0.0f;
#else

	const __m128 av = _mm_load_ps((F32*)&b->m00);
	const __m128 bv = _mm_load_ps((F32*)&b->m10);
	const __m128 cv = _mm_load_ps((F32*)&b->m20);


	__m128 t1, t2;

	t1 = _mm_set1_ps(a->m00);
	t2 = _mm_mul_ps(av, t1);
	t1 = _mm_set1_ps(a->m01);
	t2 = _mm_add_ps(_mm_mul_ps(bv, t1), t2);
	t1 = _mm_set1_ps(a->m02);
	t2 = _mm_add_ps(_mm_mul_ps(cv, t1), t2);

	_mm_store_ps(&r.m00, t2);

	t1 = _mm_set1_ps(a->m10);
	t2 = _mm_mul_ps(av, t1);
	t1 = _mm_set1_ps(a->m11);
	t2 = _mm_add_ps(_mm_mul_ps(bv, t1), t2);
	t1 = _mm_set1_ps(a->m12);
	t2 = _mm_add_ps(_mm_mul_ps(cv, t1), t2);

	_mm_store_ps(&r.m10, t2);

	t1 = _mm_set1_ps(a->m20);
	t2 = _mm_mul_ps(av, t1);
	t1 = _mm_set1_ps(a->m21);
	t2 = _mm_add_ps(_mm_mul_ps(bv, t1), t2);
	t1 = _mm_set1_ps(a->m22);
	t2 = _mm_add_ps(_mm_mul_ps(cv, t1), t2);

	_mm_store_ps(&r.m20, t2);

#endif
	return r;

}
inline const SGL_Mat4 SM_M4MultiplySIMPLE(const SGL_Mat4* a, const SGL_Mat4* b)
{
	SGL_Mat4 r;
	const float i00 = a->m00 * b->m00 + a->m01 * b->m10 + a->m02 * b->m20 + a->m03 * b->m30;
	const float i01 = a->m00 * b->m01 + a->m01 * b->m11 + a->m02 * b->m21 + a->m03 * b->m31;
	const float i02 = a->m00 * b->m02 + a->m01 * b->m12 + a->m02 * b->m22 + a->m03 * b->m32;
	const float i03 = a->m00 * b->m03 + a->m01 * b->m13 + a->m02 * b->m23 + a->m03 * b->m33;
	const float i10 = a->m10 * b->m00 + a->m11 * b->m10 + a->m12 * b->m20 + a->m13 * b->m30;
	const float i11 = a->m10 * b->m01 + a->m11 * b->m11 + a->m12 * b->m21 + a->m13 * b->m31;
	const float i12 = a->m10 * b->m02 + a->m11 * b->m12 + a->m12 * b->m22 + a->m13 * b->m32;
	const float i13 = a->m10 * b->m03 + a->m11 * b->m13 + a->m12 * b->m23 + a->m13 * b->m33;
	const float i20 = a->m20 * b->m00 + a->m21 * b->m10 + a->m22 * b->m20 + a->m23 * b->m30;
	const float i21 = a->m20 * b->m01 + a->m21 * b->m11 + a->m22 * b->m21 + a->m23 * b->m31;
	const float i22 = a->m20 * b->m02 + a->m21 * b->m12 + a->m22 * b->m22 + a->m23 * b->m32;
	const float i23 = a->m20 * b->m03 + a->m21 * b->m13 + a->m22 * b->m23 + a->m23 * b->m33;
	const float i30 = a->m30 * b->m00 + a->m31 * b->m10 + a->m32 * b->m20 + a->m33 * b->m30;
	const float i31 = a->m30 * b->m01 + a->m31 * b->m11 + a->m32 * b->m21 + a->m33 * b->m31;
	const float i32 = a->m30 * b->m02 + a->m31 * b->m12 + a->m32 * b->m22 + a->m33 * b->m32;
	const float i33 = a->m30 * b->m03 + a->m31 * b->m13 + a->m32 * b->m23 + a->m33 * b->m33;
	r.m00 = i00;
	r.m01 = i01;
	r.m02 = i02;
	r.m03 = i03;
	r.m10 = i10;
	r.m11 = i11;
	r.m12 = i12;
	r.m13 = i13;
	r.m20 = i20;
	r.m21 = i21;
	r.m22 = i22;
	r.m23 = i23;
	r.m30 = i30;
	r.m31 = i31;
	r.m32 = i32;
	r.m33 = i33;
	return r;
}
inline const SGL_Mat4 SM_M4Multiply(const SGL_Mat4* a, const SGL_Mat4* b)
{
	SGL_Mat4 r;
#if defined(ANDROID)
	const float i00 = a->m00 * b->m00 + a->m01 * b->m10 + a->m02 * b->m20 + a->m03 * b->m30;
	const float i01 = a->m00 * b->m01 + a->m01 * b->m11 + a->m02 * b->m21 + a->m03 * b->m31;
	const float i02 = a->m00 * b->m02 + a->m01 * b->m12 + a->m02 * b->m22 + a->m03 * b->m32;
	const float i03 = a->m00 * b->m03 + a->m01 * b->m13 + a->m02 * b->m23 + a->m03 * b->m33;
	const float i10 = a->m10 * b->m00 + a->m11 * b->m10 + a->m12 * b->m20 + a->m13 * b->m30;
	const float i11 = a->m10 * b->m01 + a->m11 * b->m11 + a->m12 * b->m21 + a->m13 * b->m31;
	const float i12 = a->m10 * b->m02 + a->m11 * b->m12 + a->m12 * b->m22 + a->m13 * b->m32;
	const float i13 = a->m10 * b->m03 + a->m11 * b->m13 + a->m12 * b->m23 + a->m13 * b->m33;
	const float i20 = a->m20 * b->m00 + a->m21 * b->m10 + a->m22 * b->m20 + a->m23 * b->m30;
	const float i21 = a->m20 * b->m01 + a->m21 * b->m11 + a->m22 * b->m21 + a->m23 * b->m31;
	const float i22 = a->m20 * b->m02 + a->m21 * b->m12 + a->m22 * b->m22 + a->m23 * b->m32;
	const float i23 = a->m20 * b->m03 + a->m21 * b->m13 + a->m22 * b->m23 + a->m23 * b->m33;
	const float i30 = a->m30 * b->m00 + a->m31 * b->m10 + a->m32 * b->m20 + a->m33 * b->m30;
	const float i31 = a->m30 * b->m01 + a->m31 * b->m11 + a->m32 * b->m21 + a->m33 * b->m31;
	const float i32 = a->m30 * b->m02 + a->m31 * b->m12 + a->m32 * b->m22 + a->m33 * b->m32;
	const float i33 = a->m30 * b->m03 + a->m31 * b->m13 + a->m32 * b->m23 + a->m33 * b->m33;
	r.m00 = i00;
	r.m01 = i01;
	r.m02 = i02;
	r.m03 = i03;
	r.m10 = i10;
	r.m11 = i11;
	r.m12 = i12;
	r.m13 = i13;
	r.m20 = i20;
	r.m21 = i21;
	r.m22 = i22;
	r.m23 = i23;
	r.m30 = i30;
	r.m31 = i31;
	r.m32 = i32;
	r.m33 = i33;
#else
	const __m128 av = _mm_load_ps((F32*)&b->m00);
	const __m128 bv = _mm_load_ps((F32*)&b->m10);
	const __m128 cv = _mm_load_ps((F32*)&b->m20);
	const __m128 dv = _mm_load_ps((F32*)&b->m30);

	__m128 t1, t2;

	t1 = _mm_set1_ps(a->m00);
	t2 = _mm_mul_ps(av, t1);
	t1 = _mm_set1_ps(a->m01);
	t2 = _mm_add_ps(_mm_mul_ps(bv, t1), t2);
	t1 = _mm_set1_ps(a->m02);
	t2 = _mm_add_ps(_mm_mul_ps(cv, t1), t2);
	t1 = _mm_set1_ps(a->m03);
	t2 = _mm_add_ps(_mm_mul_ps(dv, t1), t2);

	_mm_store_ps(&r.m00, t2);

	t1 = _mm_set1_ps(a->m10);
	t2 = _mm_mul_ps(av, t1);
	t1 = _mm_set1_ps(a->m11);
	t2 = _mm_add_ps(_mm_mul_ps(bv, t1), t2);
	t1 = _mm_set1_ps(a->m12);
	t2 = _mm_add_ps(_mm_mul_ps(cv, t1), t2);
	t1 = _mm_set1_ps(a->m13);
	t2 = _mm_add_ps(_mm_mul_ps(dv, t1), t2);

	_mm_store_ps(&r.m10, t2);

	t1 = _mm_set1_ps(a->m20);
	t2 = _mm_mul_ps(av, t1);
	t1 = _mm_set1_ps(a->m21);
	t2 = _mm_add_ps(_mm_mul_ps(bv, t1), t2);
	t1 = _mm_set1_ps(a->m22);
	t2 = _mm_add_ps(_mm_mul_ps(cv, t1), t2);
	t1 = _mm_set1_ps(a->m23);
	t2 = _mm_add_ps(_mm_mul_ps(dv, t1), t2);

	_mm_store_ps(&r.m20, t2);

	t1 = _mm_set1_ps(a->m30);
	t2 = _mm_mul_ps(av, t1);
	t1 = _mm_set1_ps(a->m31);
	t2 = _mm_add_ps(_mm_mul_ps(bv, t1), t2);
	t1 = _mm_set1_ps(a->m32);
	t2 = _mm_add_ps(_mm_mul_ps(cv, t1), t2);
	t1 = _mm_set1_ps(a->m33);
	t2 = _mm_add_ps(_mm_mul_ps(dv, t1), t2);

	_mm_store_ps(&r.m30, t2);
#endif
	return r;

}
inline const SGL_Vec2 SM_V2Normalize(const SGL_Vec2 a)
{
	SGL_Vec2 r;
	float m = SDL_sqrtf(a.x * a.x + a.y * a.y);
	r.x = a.x / m;
	r.y = a.y / m;
	return r;
}
inline const float SM_V2Magnitude(const SGL_Vec2 a)
{
	return SDL_sqrtf(a.x * a.x + a.y * a.y);
}
inline const SGL_Vec2 SM_V2Substract(const SGL_Vec2 a, const SGL_Vec2 b)
{
	SGL_Vec2 r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	return r;
}
inline const SGL_Vec2 SM_V2Add(const SGL_Vec2 a, const SGL_Vec2 b)
{
	SGL_Vec2 r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	return r;
}
inline const SGL_Vec2 SM_V2FMultiply(const SGL_Vec2 a, const float b)
{
	SGL_Vec2 r;
	r.x = a.x * b;
	r.y = a.y * b;
	return r;
}
inline const SGL_Vec2 SM_V2Multiply(const SGL_Vec2 a, const SGL_Vec2 b)
{
	SGL_Vec2 r;
	r.x = a.x * b.x;
	r.y = a.y * b.y;
	return r;
}
inline const SGL_Vec4 SM_V4MultiplySIMPLE(const SGL_Vec4* a, const SGL_Vec4* b)
{
	SGL_Vec4 r;
	r.x = a->x - b->x;
	r.y = a->y - b->y;
	r.z = a->z - b->z;
	r.w = a->w - b->w;
	return r;
}
inline const SGL_Vec4 SM_V4Multiply(const SGL_Vec4* a, const SGL_Vec4* b)
{
	SGL_Vec4 r;
#if defined(ANDROID)
	r.x = a->x - b->x;
	r.y = a->y - b->y;
	r.z = a->z - b->z;
	r.w = a->w - b->w;
#else
	r.v = _mm_mul_ps(a->v, b->v);
#endif
	return r;
}
inline const SGL_Vec4 SM_V4Subtract(const SGL_Vec4* a, const SGL_Vec4* b)
{
	SGL_Vec4 r;
#if defined(ANDROID)
	r.x = a->x - b->x;
	r.y = a->y - b->y;
	r.z = a->z - b->z;
	r.w = a->w - b->w;
#else
	r.v = _mm_sub_ps(a->v, b->v);
#endif
	return r;
}
inline const SGL_Vec4 SM_V4Add(const SGL_Vec4* a, const SGL_Vec4* b)
{
	SGL_Vec4 r;
#if defined(ANDROID)
	r.x = a->x + b->x;
	r.y = a->y + b->y;
	r.z = a->z + b->z;
	r.w = a->w + b->w;
#else
	//approx 10x faster
	r.v = _mm_add_ps(a->v, b->v);
#endif
	return r;
}
inline const SGL_Vec4 SM_V4Negate(const SGL_Vec4* a)
{
	SGL_Vec4 r;
#if defined(ANDROID)
	r.x = -a->x;
	r.y = -a->y;
	r.z = -a->z;
	r.w = -a->w;
#else
	//approx 7x faster
	r.v = _mm_sub_ps(_mm_set1_ps(0.0f), a->v);
#endif
	return r;
}
inline const SGL_Vec4 SM_V4Normalize(const SGL_Vec4* a)
{
	SGL_Vec4 r;
#if defined(ANDROID)
	float m = sqrtf(a->x * a->x + a->y * a->y + a->z * a->z);
	r.x = a->x / m;
	r.y = a->y / m;
	r.z = a->z / m;
	r.w = 0.0f;
#else
	//The immediate bits 4-7 of mask determine which of the corresponding source operand pairs are to be multiplied. 
	//Bits 0-3 determine whether the dot product result will be written.
	//If a mask bit is 0, the corresponding product result or written value is +0.0.
	//0x7F = 0111 1111, so the 'w'-component is discarded
	__m128 dotP = _mm_dp_ps(a->v, a->v, 0x7F);
	//calculate the reciprocal square root of dotP, 1 / sqrt(dotP)
	dotP = _mm_rsqrt_ps(dotP);
	// vec * rsqrt(dot(vec, vec))
	r.v = _mm_mul_ps(a->v, dotP);
#endif
	return r;
}
inline const SGL_Vec4 SM_V4NormalizePrecise(const SGL_Vec4* a)
{
	SGL_Vec4 r;
	float m = sqrtf(a->x * a->x + a->y * a->y + a->z * a->z);
	r.x = a->x / m;
	r.y = a->y / m;
	r.z = a->z / m;
	r.w = 0.0f;
	return r;
}
inline const float SM_Dot(const SGL_Vec4* a, const SGL_Vec4* b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}
inline const float SM_V2Dot(const SGL_Vec2 a, const SGL_Vec2 b)
{
	return a.x * b.x + a.y * b.y;
}
inline const SGL_Vec4 SM_V4CrossSimple(const SGL_Vec4* a, const SGL_Vec4* b)
{
	SGL_Vec4 r;
	r.x = a->y * b->z - a->z * b->y;
	r.y = a->z * b->x - a->x * b->z;
	r.z = a->x * b->y - a->y * b->x;
	r.w = 0.0f;
	return r;
}
inline const SGL_Vec4 SM_V4Cross(const SGL_Vec4* a, const SGL_Vec4* b)
{
	SGL_Vec4 r;
#if defined(ANDROID)
	r.x = a->y * b->z - a->z * b->y;
	r.y = a->z * b->x - a->x * b->z;
	r.z = a->x * b->y - a->y * b->x;
	r.w = 0.0f;
#else
	//yields very little perf 1.1x faster max
	r.v = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(a->v, a->v, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(b->v, b->v, _MM_SHUFFLE(3, 1, 0, 2))),
		_mm_mul_ps(_mm_shuffle_ps(a->v, a->v, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(b->v, b->v, _MM_SHUFFLE(3, 0, 2, 1)))
		);
	//_mm_load_ps((float*)&a->x)
#endif
	return r;
}
inline const SGL_Mat4 SM_LookAt(const SGL_Vec4* eye, const SGL_Vec4* center, const SGL_Vec4* up)
{
	SGL_Mat4 r;
	const SGL_Vec4 t0 = SM_V4Subtract(center, eye);
	const SGL_Vec4 f = SM_V4NormalizePrecise(&t0);
	const SGL_Vec4 t1 = SM_V4Cross(&f, up);
	const SGL_Vec4 s = SM_V4NormalizePrecise(&t1);
	const SGL_Vec4 u = SM_V4Cross(&s, &f);
	r.m00 = s.x;
	r.m01 = u.x;
	r.m02 = -f.x;
	r.m03 = 0.0f;

	r.m10 = s.y;
	r.m11 = u.y;
	r.m12 = -f.y;
	r.m13 = 0.0f;

	r.m20 = s.z;
	r.m21 = u.z;
	r.m22 = -f.z;
	r.m23 = 0.0f;

	r.m30 = -SM_Dot(&s, eye);
	r.m31 = -SM_Dot(&u, eye);
	r.m32 = SM_Dot(&f, eye);
	r.m33 = 1.0f;

	return r;
}
inline const SGL_Quat SM_QAngleAxis(const float radAngle, const SGL_Vec4* axis)
{
	SGL_Quat r;
	const float s = SDL_sinf(radAngle * 0.5f);
	r.w = SDL_cosf(radAngle * 0.5f);
	r.x = axis->x * s;
	r.y = axis->y * s;
	r.z = axis->z * s;
	return r;
}
inline const U32 SM_TransDecompose(SGL_Trans2D* trans)
{
	SGL_Vec2 row0 = { trans->mat.m00, trans->mat.m01 };
	SGL_Vec2 row1 = { trans->mat.m10, trans->mat.m11 };
	F32 kx = SDL_sqrtf(row0.x * row0.x + row0.y * row0.y);
	row0.x /= kx;
	row0.y /= kx;
	F32 kz = SM_V2Dot(row0, row1);
	row1.x += -kz * row0.x;
	row1.y += -kz * row0.y;
	F32 ky = SDL_sqrtf(row1.x * row1.x + row1.y * row1.y);
	if (kx == 0.0f || ky == 0.0f)
	{
		return SGL_FALSE;
	}
	trans->position.x = trans->mat.m20;
	trans->position.y = trans->mat.m21;
	trans->scale.x = kx;
	trans->scale.y = ky;
	trans->rotation = (float)SDL_atan2((double)trans->mat.m01, (double)trans->mat.m00);
	//skew = kz / ky;
	return SGL_TRUE;
}
inline const U32 SM_M3Decompose(const SGL_Mat3* m, SGL_Vec2* pos, SGL_Vec2* scale, F32* rot)
{
	SGL_Vec2 row0 = { m->m00, m->m01 };
	SGL_Vec2 row1 = { m->m10, m->m11 };
	F32 kx = SDL_sqrtf(row0.x * row0.x + row0.y * row0.y);
	row0.x /= kx;
	row0.y /= kx;
	F32 kz = SM_V2Dot(row0, row1);
	row1.x += -kz * row0.x;
	row1.y += -kz * row0.y;
	F32 ky = SDL_sqrtf(row1.x * row1.x + row1.y * row1.y);
	if (kx == 0.0f || ky == 0.0f)
	{
		return SGL_FALSE;
	}
	pos->x = m->m20;
	pos->y = m->m21;
	scale->x = kx;
	scale->y = ky;
	*rot = (float)SDL_atan2((double)m->m01, (double)m->m00);
	//skew = kz / ky;
	return SGL_TRUE;
}
inline const SGL_Mat3 SM_M3Scale(const SGL_Mat3* m, const SGL_Vec2 v)
{
	SGL_Mat3 r = *m;
	r.m00 *= v.x;
	r.m01 *= v.x;
	r.m02 *= v.x;
	r.m10 *= v.y;
	r.m11 *= v.y;
	r.m12 *= v.y;
	return r;
};
inline const SGL_Mat3 SM_M3Rotate(const SGL_Mat3* m, const float radAngle)
{
	const float c = SDL_cosf(radAngle);
	const float s = SDL_sinf(radAngle);

	SGL_Mat3 r;
	r.m00 = m->m00 * c + m->m10 * s;
	r.m01 = m->m01 * c + m->m11 * s;
	r.m02 = m->m02 * c + m->m12 * s;
	r.m03 = 0.0f;
	r.m10 = m->m00 * -s + m->m10 * c;
	r.m11 = m->m01 * -s + m->m11 * c;
	r.m12 = m->m02 * -s + m->m12 * c;
	r.m13 = 0.0f;
	r.m20 = m->m20;
	r.m21 = m->m21;
	r.m22 = m->m22;
	r.m23 = 0.0f;
	return r;
}
//inline const SGL_Mat3 SM_M3TranslateOrigin(const SGL_Mat3* m, const SGL_Vec2 v)
//{
//	SGL_Mat3 r = *m;
//	r.m02 += v.x;
//	r.m12 += v.y;
//	return r;
//};
//inline const SGL_Mat3 SM_M3TranslatePos(const SGL_Mat3* m, const SGL_Vec2 v)
//{
//	SGL_Mat3 r = *m;
//	r.m20 += v.x;
//	r.m21 += v.y;
//	return r;
//};
inline const SGL_Mat3 SM_M3Translate(const SGL_Mat3* m, const SGL_Vec2 v)
{
   SGL_Mat3 r = *m;
   r.m20 = m->m00 * v.x + m->m10 * v.y + m->m20;
   r.m21 = m->m01 * v.x + m->m11 * v.y + m->m21;
   r.m22 = m->m02 * v.x + m->m12 * v.y + m->m22;
   return r;
};
inline const SGL_Mat4 SM_QToM4(const SGL_Quat* q)
{
	SGL_Mat4 r;
	const float qxx = q->x * q->x;
	const float qyy = q->y * q->y;
	const float qzz = q->z * q->z;
	const float qxz = q->x * q->z;
	const float qxy = q->x * q->y;
	const float qyz = q->y * q->z;
	const float qwx = q->w * q->x;
	const float qwy = q->w * q->y;
	const float qwz = q->w * q->z;

	r.m00 = 1.0f - 2.0f * (qyy + qzz);
	r.m01 = 2.0f * (qxy + qwz);
	r.m02 = 2.0f * (qxz - qwy);
	r.m03 = 0.0f;

	r.m10 = 2.0f * (qxy - qwz);
	r.m11 = 1.0f - 2.0f * (qxx + qzz);
	r.m12 = 2.0f * (qyz + qwx);
	r.m13 = 0.0f;

	r.m20 = 2.0f * (qxz + qwy);
	r.m21 = 2.0f * (qyz - qwx);
	r.m22 = 1.0f - 2.0f * (qxx + qyy);
	r.m23 = 0.0f;

	r.m30 = 0.0f;
	r.m31 = 0.0f;
	r.m32 = 0.0f;
	r.m33 = 1.0f;

	return r;
}
inline const SGL_Mat4 SM_M4Translate(const SGL_Mat4* m, const SGL_Vec4* v)
{
   SGL_Mat4 r = *m;
   r.m30 = m->m00 * v->x + m->m10 * v->y + m->m20 * v->z + m->m30;
   r.m31 = m->m01 * v->x + m->m11 * v->y + m->m21 * v->z + m->m31;
   r.m32 = m->m02 * v->x + m->m12 * v->y + m->m22 * v->z + m->m32;
   r.m33 = m->m03 * v->x + m->m13 * v->y + m->m23 * v->z + m->m33;
   return r;
};
inline const SGL_Mat4 SM_M4Rotate(const SGL_Mat4* m, const F32 angle, const SGL_Vec4* v)
{
	SGL_Mat4 r;
	const F32 c = SDL_cosf(angle);
	const F32 s = SDL_sinf(angle);
	const SGL_Vec4 vC = { c, c, c, 0 };
	const SGL_Vec4 axis = SM_V4NormalizePrecise(v);
	SGL_Vec4 sinA = { s, s, s, 0 };
	sinA = SM_V4Multiply(&sinA, &axis);

	SGL_Vec4 temp = { 1.0f, 1.0f, 1.0f, 0.0f };
	temp = SM_V4Subtract(&temp, &vC);
	temp = SM_V4Multiply(&temp, &axis);
	SGL_Vec4 tempAC = SM_V4Multiply(&temp, &axis);
	tempAC = SM_V4Add(&tempAC, &vC);
	temp.x = temp.x * axis.y;
	temp.y = temp.x * axis.z;
	temp.z = temp.y * axis.z;

	r.m00 = tempAC.x;
	r.m01 = temp.x + sinA.z;
	r.m02 = temp.y - sinA.y;
	r.m03 = 0.0f;
	r.m10 = temp.x - sinA.z;
	r.m11 = tempAC.y;
	r.m12 = temp.z + sinA.x;
	r.m13 = 0.0f;
	r.m20 = temp.y + sinA.y;
	r.m21 = temp.z - sinA.x;
	r.m22 = tempAC.z;
	r.m23 = 0.0f;

	r = SM_M4Multiply(m, &r);
	r.v3 = m->v3;
	return r;
}
inline const SGL_Mat4 SM_Perspective(const F32 FOWY, const F32 aspect, const F32 nearPlane, const F32 farPlane)
{
	SGL_Mat4 r;
	const F32 tanHalfFovy = SDL_tanf(FOWY / 2.0f);

	r.m00 = 1.0f / (aspect * tanHalfFovy);
	r.m01 = 0.0f;
	r.m02 = 0.0f;
	r.m03 = 0.0f;

	r.m10 = 0.0f;
	r.m11 = 1.0f / (tanHalfFovy);
	r.m12 = 0.0f;
	r.m13 = 0.0f;

	r.m20 = 0.0f;
	r.m21 = 0.0f;
	r.m22 = -(farPlane + nearPlane) / (farPlane - nearPlane);
	r.m23 = -1.0f;

	r.m30 = 0.0f;
	r.m31 = 0.0f;
	r.m32 = -2.0f * farPlane * nearPlane / (farPlane - nearPlane);
	r.m33 = 1.0f;
	return r;
}
inline const SGL_Mat4 SM_Ortho(F32 left, F32 right, F32 bottom, F32 top)
{
	SGL_Mat4 r;
	r.m00 = 2.0f / (right - left);
	r.m01 = 0.0f;
	r.m02 = 0.0f;
	r.m03 = 0.0f;

	r.m10 = 0.0f;
	r.m11 = 2.0f / (top - bottom);
	r.m12 = 0.0f;
	r.m13 = 0.0f;

	r.m20 = 0.0f;
	r.m21 = 0.0f;
	r.m22 = -1.0f;
	r.m23 = 0.0f;

	r.m30 = -(right + left) / (right - left);
	r.m31 = -(top + bottom) / (top - bottom);
	r.m32 = 0.0f;
	r.m33 = 1.0f;
	return r;
}
/*inline void SM_CalculateSpriteData(const SGL_TexRegion* reg, const SGL_Vec4* texSize, const SGL_Mat3* a, const SGL_Vec4* x, const SGL_Vec4* y, SGL_Vec2* dst)
{
	SGL_Vec4 arr[2];
	SGL_Vec4 mReg;
	{
		const __m128 vx = _mm_load_ps((F32*)&x->x);
		const __m128 vy = _mm_load_ps((F32*)&y->x);
		__m128 t1, t2;
		t1 = _mm_set1_ps(a->m00);
		t2 = _mm_mul_ps(vx, t1);
		t1 = _mm_set1_ps(a->m01);
		t2 = _mm_add_ps(_mm_mul_ps(vy, t1), t2);
		t1 = _mm_set1_ps(a->m20);
		t2 = _mm_add_ps(t1, t2);
		_mm_store_ps(arr[0].f, t2);
		t1 = _mm_set1_ps(a->m10);
		t2 = _mm_mul_ps(vx, t1);
		t1 = _mm_set1_ps(a->m11);
		t2 = _mm_add_ps(_mm_mul_ps(vy, t1), t2);
		t1 = _mm_set1_ps(a->m21);
		t2 = _mm_add_ps(t1, t2);
		_mm_store_ps(arr[1].f, t2);
		mReg.v = _mm_div_ps(reg->v, texSize->v);
		mReg.v = _mm_add_ps(mReg.v, _mm_loadh_pi(_mm_setzero_ps(), (__m64*)&mReg.v));
	}
	dst[0].x = arr[0].f[0];
	dst[0].y = arr[1].f[0];
	dst[1].x = mReg.f[2];
	dst[1].y = mReg.f[3];
	dst[2].x = arr[0].f[1];
	dst[2].y = arr[1].f[1];
	dst[3].x = mReg.f[0];
	dst[3].y = mReg.f[3];
	dst[4].x = arr[0].f[2];
	dst[4].y = arr[1].f[2];
	dst[5].x = mReg.f[0];
	dst[5].y = mReg.f[1];
	dst[6].x = arr[0].f[3];
	dst[6].y = arr[1].f[3];
	dst[7].x = mReg.f[2];
	dst[7].y = mReg.f[1];
}*/
inline const SGL_Vec4 SM_CalculateUVs(const SGL_TexRegion* reg, const SGL_Vec2* texSize)
{
	SGL_Vec4 mReg;
	mReg.x = reg->offset.x / texSize->x;
	mReg.y = reg->offset.y / texSize->y;
	mReg.z = reg->size.x / texSize->x + mReg.x;
	mReg.w = reg->size.y / texSize->y + mReg.y;
	return mReg;
}
inline void SM_UpdateTransform(SGL_Trans2D* trans)
{
	SGL_Mat3 mat = SM_IdentityMat3();
	mat = SM_M3Translate(&mat, trans->position);
	mat = SM_M3Rotate(&mat, trans->rotation);
	mat = SM_M3Scale(&mat, trans->scale);
	trans->mat = mat;
}
inline const U32 SM_GlobalToLocalTrans(SGL_Trans2D* trans, const SGL_Mat3* invertedParentMat)
{
	//create global matrix
	SGL_Mat3 mat = SM_IdentityMat3();
	mat = SM_M3Translate(&mat, trans->position);
	mat = SM_M3Rotate(&mat, trans->rotation);
	mat = SM_M3Scale(&mat, trans->scale);
	//convert it to local matrix
	mat = SM_M3Multiply(&mat, invertedParentMat);
	trans->mat = mat;
	//decompose matrix into position, scale and rotation
	return SM_TransDecompose(trans);
}
#endif