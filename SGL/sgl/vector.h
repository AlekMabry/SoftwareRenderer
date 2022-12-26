#pragma once

#include "common.h"
#include <immintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>

typedef SGL_ALIGN(16) float SGLFloat4[4];
typedef SGL_ALIGN(16) int32_t SGLFixed4[4];
typedef SGL_ALIGN(16) SGLFloat4 SGLFloat4x4[4];
typedef SGL_ALIGN(16) SGLFixed4 SGLFixed4x4[4];

#define SGL_FLOAT4X4_IDENTITY_INIT	{{1.0f, 0.0f, 0.0f, 0.0f},	\
									{0.0f, 1.0f, 0.0f, 0.0f},	\
									{0.0f, 0.0f, 1.0f, 0.0f},	\
									{0.0f, 0.0f, 0.0f, 1.0f}}

#define SGL_FLOAT4X4_ZERO_INIT		{{0.0f, 0.0f, 0.0f, 0.0f},	\
									{0.0f, 0.0f, 0.0f, 0.0f},	\
									{0.0f, 0.0f, 0.0f, 0.0f},	\
									{0.0f, 0.0f, 0.0f, 0.0f}}

#define SGL_FLOAT4X4_IDENTITY	((SGLFloat4x4)SGL_FLOAT4X4_IDENTITY_INIT)
#define SGL_FLOAT4X4_ZERO		((SGLFloat4x4)SGL_FLOAT4X4_ZERO_INIT)

/*********************************SET/COPY************************************/

SGL_INLINE void sglFloat4Set1(float a, SGLFloat4 dest)
{
	*((__m128*) dest) = _mm_set_ps1(a);
}

SGL_INLINE void sglFloat4Copy(SGLFloat4 a, SGLFloat4 dest)
{
	*((__m128*) dest) = *((__m128*) a);
}

SGL_INLINE void sglFixed4Set1(int32_t a, SGLFixed4 dest)
{
	*((__m128i*) dest) = _mm_set1_epi32(a);
}

SGL_INLINE void sglFixed4Copy(SGLFixed4 a, SGLFixed4 dest)
{
	*((__m128i*) dest) = *((__m128i*) a);
}

/*******************************CONVERSIONS***********************************/

SGL_INLINE void sglFloat4ToFixed4(SGLFloat4 a, SGLFixed4 dest)
{
	*((__m128i*) dest) = _mm_cvtps_epi32(*((__m128*) a));
}

SGL_INLINE void sglFloat4x4ToFixed4x4(SGLFloat4x4 a, SGLFixed4x4 dest)
{
	*((__m128i*) dest[0]) = _mm_cvtps_epi32(*((__m128*) a[0]));
	*((__m128i*) dest[1]) = _mm_cvtps_epi32(*((__m128*) a[1]));
	*((__m128i*) dest[2]) = _mm_cvtps_epi32(*((__m128*) a[2]));
	*((__m128i*) dest[3]) = _mm_cvtps_epi32(*((__m128*) a[3]));
}

/*******************************TRANSFORMS************************************/

SGL_INLINE void sglFloat4PerspectiveDiv(SGLFloat4 a, SGLFloat4 dest)
{
	__m128 denom = _mm_shuffle_ps(*((__m128*) a), *((__m128*) a),
		_MM_SHUFFLE(3, 3, 3, 3));
	*((__m128*) dest) = _mm_div_ps(*((__m128*) a), denom);
}

SGL_INLINE void sglFloat4x4LookAt(SGLFloat4 eye, SGLFloat4 center, SGLFloat4 up,
	SGLFloat4x4 dest)
{
	SGLFloat4 f, u, s;
	sglFloat4Sub(center, eye, f);
	sglFloat3Normalize(f);

	sglFloat3Cross(f, up, s);
	sglFloat3Normalize(s);
	sglFloat3Cross(s, f, u);

	dest[0][0] = s[0];
	dest[0][1] = u[0];
	dest[0][2] = -f[0];
	dest[1][0] = s[1];
	dest[1][1] = u[1];
	dest[1][2] = -f[1];
	dest[2][0] = s[2];
	dest[2][1] = u[2];
	dest[2][2] = -f[2];
	dest[3][0] = -sglFloat3Dot(s, eye);
	dest[3][1] = -sglFloat3Dot(u, eye);
	dest[3][2] = sglFloat3Dot(f, eye);
	dest[0][3] = dest[1][3] = dest[2][3] = 0.0f;
	dest[3][3] = 1.0f;
}

SGL_INLINE void sglFloat4x4Perspective(float fovY, float aspect, float nearZ,
	float farZ, SGLFloat4x4 dest)
{
	// Right hand, [-1, 1] clipspace
	float f, fn;

	sglFloat4x4Zero(dest);

	f = 1.0f / tanf(fovY * 0.5f);
	fn = 1.0f / (nearZ - farZ);

	dest[0][0] = f / aspect;
	dest[1][1] = f;
	dest[2][2] = (nearZ + farZ) * fn;
	dest[2][3] = -1.0f;
	dest[3][2] = 2.0f * nearZ * farZ * fn;
}

SGL_INLINE void sglFloat2NDCToScreenspace(SGLFloat4 ndc, int32_t w, int32_t h,
	uint32_t precision, SGLFloat4 dest)
{

	dest[0] = (int32_t)(((ndc[0] + 1.0f) * 0.5f) * (float)w * (float)precision);
	dest[1] = (int32_t)(((ndc[1] + 1.0f) * 0.5f) * (float)h * (float)precision);
}

SGL_INLINE void sglFloat2ScreenspaceToNDC(SGLFloat4 screenspace, int32_t w, int32_t h,
	uint32_t precision, SGLFloat4 dest)
{
	dest[0] = (float)(((float)screenspace[0] /
		(float)w * (float)precision * 2.0f) - 1.0f);
	dest[1] = (float)(((float)screenspace[1] /
		(float)h * (float)precision * 2.0f) - 1.0f);
}

/*******************************ARITHMETIC************************************/

SGL_INLINE float sglFloatClamp(float a, float min, float max) {
	const float t = a < min ? min : a;
	return t > max ? max : t;
}

SGL_INLINE void sglFloat4Add(SGLFloat4 a, SGLFloat4 b, SGLFloat4 dest)
{
	*((__m128*) dest) = _mm_add_ps(*((__m128*) a), *((__m128*) b));
}

SGL_INLINE void sglFloat4Sub(SGLFloat4* a, SGLFloat4* b, SGLFloat4* dest)
{
	dest->v = _mm_sub_ps(a->v, b->v);
	//*((__m128*) dest) = _mm_sub_ps(*((__m128*) a), *((__m128*) b));
	__m128 v;
	v.
}

SGL_INLINE void sglFloat4Mul(SGLFloat4 a, SGLFloat4 b, SGLFloat4 dest)
{
	*((__m128*) dest) = _mm_mul_ps(*((__m128*) a), *((__m128*) b));
}

SGL_INLINE void sglFloat4Div(SGLFloat4 a, SGLFloat4 b, SGLFloat4 dest)
{
	*((__m128*) dest) = _mm_div_ps(*((__m128*) a), *((__m128*) b));
}

SGL_INLINE void sglFloat4Min(SGLFloat4 a, SGLFloat4 b, SGLFloat4 dest)
{
	*((__m128*) dest) = _mm_min_ps(*((__m128*) a), *((__m128*) b));
}

SGL_INLINE void sglFloat4Max(SGLFloat4 a, SGLFloat4 b, SGLFloat4 dest)
{
	*((__m128*) dest) = _mm_max_ps(*((__m128*) a), *((__m128*) b));
}

SGL_INLINE void sglFixed4Add(SGLFixed4 a, SGLFixed4 b, SGLFixed4 dest)
{
	*((__m128i*) dest) = _mm_add_epi32(*((__m128i*) a), *((__m128i*) b));
}

SGL_INLINE void sglFixed4Sub(SGLFixed4 a, SGLFixed4 b, SGLFixed4 dest)
{
	*((__m128i*) dest) = _mm_sub_epi32(*((__m128i*) a), *((__m128i*) b));
}

SGL_INLINE void sglFixed4Mul(SGLFixed4 a, SGLFixed4 b, SGLFixed4 dest)
{
	*((__m128i*) dest) = _mm_mullo_epi32(*((__m128i*) a), *((__m128i*) b));
}

SGL_INLINE void sglFixed4Div(SGLFixed4 a, SGLFixed4 b, SGLFixed4 dest)
{
	*((__m128i*) dest) = _mm_div_epi32(*((__m128i*) a), *((__m128i*) b));
}

SGL_INLINE void sglFixed4Min(SGLFixed4 a, SGLFixed4 b, SGLFixed4 dest)
{
	*((__m128i*) dest) = _mm_min_epi32(*((__m128i*) a), *((__m128i*) b));
}

SGL_INLINE void sglFixed4Max(SGLFixed4 a, SGLFixed4 b, SGLFixed4 dest)
{
	*((__m128i*) dest) = _mm_max_epi32(*((__m128i*) a), *((__m128i*) b));
}

/*********************************ALGEBRA*************************************/

SGL_INLINE float sglFloat2Dot(SGLFloat4 a, SGLFloat4 b)
{
	return	(a[0] * b[0]) + (a[1] * b[1]);
}

SGL_INLINE int32_t sglFixed2Orient(SGLFixed4 a, SGLFixed4 b, SGLFixed4 c)
{
	//return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
	return (a[0] * (b[1] - c[1])) + (b[0] * (c[1] - a[1])) + (c[0] * (a[1] - b[1]));
}

SGL_INLINE void sglVec2Barycentric(SGLFloat4 p, SGLFloat4 a, SGLFloat4 b,
	SGLFloat4 c, float* u, float* v, float* w)
{
	SGLFloat4 v0, v1, v2;
	sglFloat4Sub(b, a, v0);
	sglFloat4Sub(c, a, v1);
	sglFloat4Sub(p, a, v2);

	float d00 = sglFloat2Dot(v0, v0);
	float d01 = sglFloat2Dot(v0, v1);
	float d11 = sglFloat2Dot(v1, v1);
	float d20 = sglFloat2Dot(v2, v0);
	float d21 = sglFloat2Dot(v2, v1);
	float denom = d00 * d11 - d01 * d01;
	*v = (d11 * d20 - d01 * d21) / denom;
	*w = (d00 * d21 - d01 * d20) / denom;
	*u = 1.0f - *v - *w;
}

SGL_INLINE float sglFloat3Dot(SGLFloat4 a, SGLFloat4 b)
{
	return	(a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

SGL_INLINE float sglFloat3Norm(SGLFloat4 a, SGLFloat4 b)
{
	return	sqrt((a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]));
}

SGL_INLINE void sglFloat3Normalize(SGLFloat4 a)
{
	float length = sqrt((a[0] * a[0]) + (a[1] * a[1]) + (a[2] * a[2]));
	a[0] /= length;
	a[1] /= length;
	a[2] /= length;
}

SGL_INLINE float sglFloat4Dot(SGLFloat4 a, SGLFloat4 b)
{
	return	(a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]) + (a[3] * b[3]);
}

SGL_INLINE void sglFloat3Cross(SGLFloat4 a, SGLFloat4 b, SGLFloat4 dest)
{
	SGLFloat4 c;
	c[0] = a[1] * b[2] - a[2] * b[1];
	c[1] = a[2] * b[0] - a[0] * b[2];
	c[2] = a[0] * b[1] - a[1] * b[0];
	sglFloat4Copy(c, dest);
}

SGL_INLINE void sglFloat4Mix(SGLFloat4* a, SGLFloat4* b, float t,
	SGLFloat4* dest)
{
	__m128 vecT = _mm_set_ps1(t);
	__m128 oneMinusT = _mm_set_ps1(1.0f);
	oneMinusT = _mm_sub_ps(oneMinusT, vecT);	// (1.0f - t)

	__m128 tempA = _mm_mul_ps(oneMinusT, *((__m128*) a));	// (1.0f - t) * a[]
	__m128 tempB = _mm_mul_ps(vecT, *((__m128*) b));		// t * b[]
	*((__m128*) dest) = _mm_add_ps(tempA, tempB);
}

/**********************************MATRIX*************************************/

SGL_INLINE void sglFloat4x4Zero(SGLFloat4x4 m)
{
	*((__m128*)m[0]) = _mm_set1_ps(0.0f);
	*((__m128*)m[1]) = _mm_set1_ps(0.0f);
	*((__m128*)m[2]) = _mm_set1_ps(0.0f);
	*((__m128*)m[3]) = _mm_set1_ps(0.0f);
}

SGL_INLINE void sglFloat4x4Identity(SGLFloat4x4 m)
{
	*((__m128*)m[0]) = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
	*((__m128*)m[1]) = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f);
	*((__m128*)m[2]) = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
	*((__m128*)m[3]) = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
}

SGL_INLINE void sglMat4Transpose(SGLFloat4x4 m, SGLFloat4x4 dest)
{
	dest[0][0] = m[0][0]; dest[1][0] = m[0][1];
	dest[0][1] = m[1][0]; dest[1][1] = m[1][1];
	dest[0][2] = m[2][0]; dest[1][2] = m[2][1];
	dest[0][3] = m[3][0]; dest[1][3] = m[3][1];
	dest[2][0] = m[0][2]; dest[3][0] = m[0][3];
	dest[2][1] = m[1][2]; dest[3][1] = m[1][3];
	dest[2][2] = m[2][2]; dest[3][2] = m[2][3];
	dest[2][3] = m[3][2]; dest[3][3] = m[3][3];
}

SGL_INLINE void sglFloat4x4Float4Mul(SGLFloat4x4 m, SGLFloat4 v, SGLFloat4 dest)
{
	SGLFloat4 res;
	res[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
	res[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
	res[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
	res[3] = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];
	sglFloat4Copy(res, dest);
}

SGL_INLINE void sglFloat4x4Float4x4Mul(SGLFloat4x4 m1, SGLFloat4x4 m2,
	SGLFloat4x4 dest)
{
	float	a00 = m1[0][0], a01 = m1[0][1], a02 = m1[0][2], a03 = m1[0][3],
		a10 = m1[1][0], a11 = m1[1][1], a12 = m1[1][2], a13 = m1[1][3],
		a20 = m1[2][0], a21 = m1[2][1], a22 = m1[2][2], a23 = m1[2][3],
		a30 = m1[3][0], a31 = m1[3][1], a32 = m1[3][2], a33 = m1[3][3],

		b00 = m2[0][0], b01 = m2[0][1], b02 = m2[0][2], b03 = m2[0][3],
		b10 = m2[1][0], b11 = m2[1][1], b12 = m2[1][2], b13 = m2[1][3],
		b20 = m2[2][0], b21 = m2[2][1], b22 = m2[2][2], b23 = m2[2][3],
		b30 = m2[3][0], b31 = m2[3][1], b32 = m2[3][2], b33 = m2[3][3];

	dest[0][0] = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;
	dest[0][1] = a01 * b00 + a11 * b01 + a21 * b02 + a31 * b03;
	dest[0][2] = a02 * b00 + a12 * b01 + a22 * b02 + a32 * b03;
	dest[0][3] = a03 * b00 + a13 * b01 + a23 * b02 + a33 * b03;
	dest[1][0] = a00 * b10 + a10 * b11 + a20 * b12 + a30 * b13;
	dest[1][1] = a01 * b10 + a11 * b11 + a21 * b12 + a31 * b13;
	dest[1][2] = a02 * b10 + a12 * b11 + a22 * b12 + a32 * b13;
	dest[1][3] = a03 * b10 + a13 * b11 + a23 * b12 + a33 * b13;
	dest[2][0] = a00 * b20 + a10 * b21 + a20 * b22 + a30 * b23;
	dest[2][1] = a01 * b20 + a11 * b21 + a21 * b22 + a31 * b23;
	dest[2][2] = a02 * b20 + a12 * b21 + a22 * b22 + a32 * b23;
	dest[2][3] = a03 * b20 + a13 * b21 + a23 * b22 + a33 * b23;
	dest[3][0] = a00 * b30 + a10 * b31 + a20 * b32 + a30 * b33;
	dest[3][1] = a01 * b30 + a11 * b31 + a21 * b32 + a31 * b33;
	dest[3][2] = a02 * b30 + a12 * b31 + a22 * b32 + a32 * b33;
	dest[3][3] = a03 * b30 + a13 * b31 + a23 * b32 + a33 * b33;
}