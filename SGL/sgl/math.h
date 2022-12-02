#pragma once

#include <stdint.h>
#include <math.h>

#if defined (_MSC_VER)
#define SGL_INLINE __forceinline
#else
#define SGL_INLINE inline
#endif

/**
	@defgroup SGLMath Vector Math
	@brief Vector math graphics functions, based on cglm library.
*/

typedef float SGLVec2[4];
typedef int32_t SGLVec2I[4];
typedef float SGLVec3[4];
typedef int32_t SGLVec3I[4];
typedef float SGLVec4[4];
typedef SGLVec4 SGLMat4[4];
typedef SGLVec4 SGLTriVec4[3];
typedef SGLVec2I SGLTriVec2I[3];

#define SGL_MAT4_IDENTITY_INIT	{{1.0f, 0.0f, 0.0f, 0.0f},	\
								{0.0f, 1.0f, 0.0f, 0.0f},	\
								{0.0f, 0.0f, 1.0f, 0.0f},	\
								{0.0f, 0.0f, 0.0f, 1.0f}}

#define SGL_MAT4_ZERO_INIT		{{0.0f, 0.0f, 0.0f, 0.0f},	\
								{0.0f, 0.0f, 0.0f, 0.0f},	\
								{0.0f, 0.0f, 0.0f, 0.0f},	\
								{0.0f, 0.0f, 0.0f, 0.0f}}

#define SGL_MAT4_IDENTITY	((SGLMat4)SGL_MAT4_IDENTITY_INIT)
#define SGL_MAT4_ZERO		((SGLMat4)SGL_MAT4_ZERO_INIT)

/**
	@ingroup SGLMath
*/
SGL_INLINE float sglFloatClamp(float a, float min, float max) {
	const float t = a < min ? min : a;
	return t > max ? max : t;
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec2Copy(SGLVec2 a, SGLVec2 dest)
{
	dest[0] = a[0];
	dest[1] = a[1];
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec2Add(SGLVec2 a, SGLVec2 b, SGLVec2 dest)
{
	dest[0] = a[0] + b[0];
	dest[1] = a[1] + b[1];
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec2Sub(SGLVec2 a, SGLVec2 b, SGLVec2 dest)
{
	dest[0] = a[0] - b[0];
	dest[1] = a[1] - b[1];
}

/**
	@ingroup SGLMath
*/
SGL_INLINE float sglVec2Dot(SGLVec2 a, SGLVec2 b)
{
	return (a[0] * b[0]) + (a[1] * b[1]);
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec3Copy(SGLVec3 a, SGLVec3 dest)
{
	dest[0] = a[0];
	dest[1] = a[1];
	dest[2] = a[2];
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec3Add(SGLVec3 a, SGLVec3 b, SGLVec3 dest)
{
	dest[0] = a[0] + b[0];
	dest[1] = a[1] + b[1];
	dest[2] = a[2] + b[2];
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec3Sub(SGLVec3 a, SGLVec3 b, SGLVec3 dest)
{
	dest[0] = a[0] - b[0];
	dest[1] = a[1] - b[1];
	dest[2] = a[2] - b[2];
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec3Mul(SGLVec3 a, SGLVec3 b, SGLVec3 dest)
{
	dest[0] = a[0] * b[0];
	dest[1] = a[1] * b[1];
	dest[2] = a[2] * b[2];
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec3FloatAdd(SGLVec3 a, float b, SGLVec3 dest)
{
	dest[0] = a[0] + b;
	dest[1] = a[1] + b;
	dest[2] = a[2] + b;
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglFloatVec3Sub(float a, SGLVec3 b, SGLVec3 dest)
{
	dest[0] = a - b[0];
	dest[1] = a - b[1];
	dest[2] = a - b[2];
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec3FloatMul(SGLVec3 a, float b, SGLVec3 dest)
{
	dest[0] = a[0] * b;
	dest[1] = a[1] * b;
	dest[2] = a[2] * b;
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec3Div(SGLVec3 a, SGLVec3 b, SGLVec3 dest)
{
	dest[0] = a[0] / b[0];
	dest[1] = a[1] / b[1];
	dest[2] = a[2] / b[2];
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec3FloatDiv(SGLVec3 a, float b, SGLVec3 dest)
{
	dest[0] = a[0] / b;
	dest[1] = a[1] / b;
	dest[2] = a[2] / b;
}

/**
	@ingroup SGLMath
*/
SGL_INLINE float sglVec3Dot(SGLVec3 a, SGLVec3 b)
{
	return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

/**
	@ingroup SGLMath
*/
SGL_INLINE float sglVec3Length(SGLVec3 a)
{
	return sqrt(sglVec3Dot(a, a));
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec3Normalize(SGLVec3 v)
{
	float length = sqrt(sglVec3Dot(v, v));
	v[0] /= length;
	v[1] /= length;
	v[2] /= length;
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec3Cross(SGLVec3 a, SGLVec3 b, SGLVec3 dest)
{
	SGLVec3 c;
	c[0] = a[1] * b[2] - a[2] * b[1];
	c[1] = a[2] * b[0] - a[0] * b[2];
	c[2] = a[0] * b[1] - a[1] * b[0];
	sglVec3Copy(c, dest);
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec3Mix(SGLVec3 a, SGLVec3 b, float t, SGLVec3 dest)
{
	dest[0] = (1.0f - t) * a[0] + t * b[0];
	dest[1] = (1.0f - t) * a[1] + t * b[1];
	dest[2] = (1.0f - t) * a[2] + t * b[2];
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec4Copy(SGLVec4 a, SGLVec4 dest)
{
	dest[0] = a[0];
	dest[1] = a[1];
	dest[2] = a[2];
	dest[3] = a[3];
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec2Barycentric(SGLVec2 p, SGLVec2 a, SGLVec2 b, SGLVec2 c,
	float* u, float* v, float* w)
{
	SGLVec2 v0, v1, v2;
	sglVec2Sub(b, a, v0);
	sglVec2Sub(c, a, v1);
	sglVec2Sub(p, a, v2);

	float d00 = sglVec2Dot(v0, v0);
	float d01 = sglVec2Dot(v0, v1);
	float d11 = sglVec2Dot(v1, v1);
	float d20 = sglVec2Dot(v2, v0);
	float d21 = sglVec2Dot(v2, v1);
	float denom = d00 * d11 - d01 * d01;
	*v = (d11 * d20 - d01 * d21) / denom;
	*w = (d00 * d21 - d01 * d20) / denom;
	*u = 1.0f - *v - *w;
}

/**
	@ingroup SGLMath
	Determinant of [[a[0] a[1] 1], [b[0] b[1] 1], [c[0] c[1] 1]] 
*/
SGL_INLINE int32_t sglVec2IOrient(SGLVec2I a, SGLVec2I b, SGLVec2I c)
{
	//return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
	return (a[0] * (b[1] - c[1])) + (b[0] * (c[1] - a[1])) + (c[0] * (a[1] - b[1]));
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec2IMin(SGLVec2I a, SGLVec2I b, SGLVec2I dest)
{
	dest[0] = (a[0] < b[0]) ? a[0] : b[0];
	dest[1] = (a[1] < b[1]) ? a[1] : b[1];
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec2IMax(SGLVec2I a, SGLVec2I b, SGLVec2I dest)
{
	dest[0] = (a[0] > b[0]) ? a[0] : b[0];
	dest[1] = (a[1] > b[1]) ? a[1] : b[1];
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglTriVec2IBBox(SGLTriVec2I tri, SGLVec2I minVec, SGLVec2I maxVec)
{
	sglVec2IMin(tri[0], tri[1], minVec);
	sglVec2IMin(minVec, tri[2], minVec);
	sglVec2IMax(tri[0], tri[1], maxVec);
	sglVec2IMax(maxVec, tri[2], maxVec);
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec2IClipBBox(SGLVec2I minVec, SGLVec2I maxVec,
	SGLVec2I lowerBound, SGLVec2I upperBound)
{
	sglVec2IMax(minVec, lowerBound, minVec);
	sglVec2IMin(maxVec, upperBound, maxVec);
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglVec3PerspectiveDivide(SGLVec4 in, SGLVec4 dest)
{
	dest[0] = in[0] / in[3];
	dest[1] = in[1] / in[3];
	dest[2] = in[2] / in[3];
}

/**
	@ingroup SGLMath
	Converts NDC coordinate (-1.0f < SGLVec3 < 1.0f) to screen space
	(0 < SGLVec2I < screen dimensions) with subpixel precision.
*/
SGL_INLINE void sglVec2IScreenSpaceFromNDC(SGLVec4 ndc, int32_t w, int32_t h,
	uint32_t precision, SGLVec2I dest)
{

	dest[0] = (int32_t)(((ndc[0] + 1.0f) * 0.5f) * (float)w * (float)precision);
	dest[1] = (int32_t)(((ndc[1] + 1.0f) * 0.5f) * (float)h * (float)precision);
}

/**
	@ingroup SGLMath
	Converts screen space (0 < SGLVec2I < screen dimensions) with subpixel precision to
	NDC coordinate (-1.0f < SGLVec3 < 1.0f).
*/
SGL_INLINE void sglVec2NDCToScreenspace(SGLVec2I screenspace, int32_t w, int32_t h,
	uint32_t precision, SGLVec2 dest)
{
	dest[0] = (float)(((float)screenspace[0] / (float)w * (float)precision * 2.0f) - 1.0f);
	dest[1] = (float)(((float)screenspace[1] / (float)h * (float)precision * 2.0f) - 1.0f);
}

/**
	@ingroup SGLMath
	@brief Clear a SGLMat4 with zeros.
*/
SGL_INLINE void sglMat4Zero(SGLMat4 m)
{
	m[0][0] = m[0][1] = m[0][2] = m[0][3] = 0;
	m[1][0] = m[1][1] = m[1][2] = m[1][3] = 0;
	m[2][0] = m[2][1] = m[2][2] = m[2][3] = 0;
	m[3][0] = m[3][1] = m[3][2] = m[3][3] = 0;
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglMat4Vec4Mul(SGLMat4 m, SGLVec4 v, SGLVec4 dest)
{
	SGLVec4 res;
	res[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
	res[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
	res[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
	res[3] = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];
	sglVec4Copy(res, dest);
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglMat4Vec3Mul(SGLMat4 m, SGLVec3 v, float last, SGLVec3 dest)
{
	SGLVec4 res;
	res[0] = v[0];
	res[1] = v[1];
	res[2] = v[2];
	res[3] = last;
	sglMat4Vec4Mul(m, res, res);
	dest[0] = res[0];
	dest[1] = res[1];
	dest[2] = res[2];
	//sglVec3Copy(res, dest);
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglMat4Transpose(SGLMat4 m, SGLMat4 dest)
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

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglMat4Mat4Mul(SGLMat4 m1, SGLMat4 m2, SGLMat4 dest)
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

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglMat4LookAt(SGLVec3 eye, SGLVec3 center, SGLVec3 up,
	SGLMat4 dest)
{
	SGLVec3 f, u, s;
	sglVec3Sub(center, eye, f);
	sglVec3Normalize(f);

	sglVec3Cross(f, up, s);
	sglVec3Normalize(s);
	sglVec3Cross(s, f, u);

	dest[0][0] = s[0];
	dest[0][1] = u[0];
	dest[0][2] = -f[0];
	dest[1][0] = s[1];
	dest[1][1] = u[1];
	dest[1][2] = -f[1];
	dest[2][0] = s[2];
	dest[2][1] = u[2];
	dest[2][2] = -f[2];
	dest[3][0] = -sglVec3Dot(s, eye);
	dest[3][1] = -sglVec3Dot(u, eye);
	dest[3][2] = sglVec3Dot(f, eye);
	dest[0][3] = dest[1][3] = dest[2][3] = 0.0f;
	dest[3][3] = 1.0f;
}

/**
	@ingroup SGLMath
*/
SGL_INLINE void sglMat4Perspective(float fovY, float aspect, float nearZ,
	float farZ, SGLMat4 dest)
{
	// Right hand, [-1, 1] clipspace
	float f, fn;

	sglMat4Zero(dest);

	f = 1.0f / tanf(fovY * 0.5f);
	fn = 1.0f / (nearZ - farZ);

	dest[0][0] = f / aspect;
	dest[1][1] = f;
	dest[2][2] = (nearZ + farZ) * fn;
	dest[2][3] = -1.0f;
	dest[3][2] = 2.0f * nearZ * farZ * fn;
}