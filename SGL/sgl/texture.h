#pragma once

#include "common.h"
#include "vector.h"

#pragma pack(1)

/** 24-bit color. (little-endian member layout) */
typedef struct SGLBGR {
	uint8_t b, g, r;
} SGLBGR;

/** 32-bit color (little-endian member layout). */
typedef struct SGLBGRA {
	uint8_t b, g, r, a;
} SGLBGRA;

#pragma pack(pop)

/** The texture class, just about everything reads from or writes to this. */
typedef struct SGLTex2D
{
	int32_t sType;		/** Type, see SGL_TYPE_TEX2D_ macros. */
	int32_t w;			/** Width */
	int32_t h;			/** Height */
	int32_t mipmaps;	/** Number of mipmaps. */
	SGLBGRA* pBGRA;		/** BGRA color buffer. Size: w * h */ 
	float* pFloat;		/** Float depth buffer. Size: w * h */
} SGLTex2D;

/** Create texture.
	
	@param[in]		sType	Type of texture. See SGL_TYPE_TEX2D_ macros for
							options.
	@param[in]		w		Width
	@param[in]		h		Height
	@param[in]		mipmaps	Number of mipmaps (=0 if none)
	@param[in,out]	pTex	Uninitialized texture to initialize. */
SGL_EXPORT void sglCreateTex2D(uint32_t sType, uint32_t w, uint32_t h,
	uint32_t mipmaps, SGLTex2D* pTex);

SGL_EXPORT void sglDestroyTex2D(SGLTex2D* pTex);

/**	Fill texture with a color (BGRA). */
SGL_EXPORT void sglTex2DFillBGRA(SGLBGRA color, SGLTex2D* pTex);

/** Fill texture with a float value. */
SGL_EXPORT void sglTex2DFillFloat(float scalar, SGLTex2D* pTex);

/**	Converts BGRA to normalized Float4 color, where channels from [0, 255]
	become [0.0f, 1.0f].

	@param[in]		in	Color in SGLBGR32.
	@param[in,out]	out	Normalized color in SGLVec4. */
SGL_INLINE void sglBGRAToFloat4(SGLBGRA in, SGLFloat4 pOut)
{
	pOut[0] = (float)in.b * (1.0f / 255.0f);
	pOut[1] = (float)in.g * (1.0f / 255.0f);
	pOut[2] = (float)in.r * (1.0f / 255.0f);
	pOut[3] = (float)in.a * (1.0f / 255.0f);
}

/**	Converts Float4 normalized color to BGRA, where channels from [0.0f, 1.0f]
	become [0, 255].

	@param[in]		in	Normalized color in Float4.
	@param[in,out] out	Color in BGRA. */
SGL_INLINE void sglFloat4ToBGRA(const SGLFloat4 pIn, SGLBGRA *pOut)
{
	pOut->b = (uint8_t)(pIn[0] * 255.0f);
	pOut->g = (uint8_t)(pIn[1] * 255.0f);
	pOut->r = (uint8_t)(pIn[2] * 255.0f);
	pOut->a = (uint8_t)(pIn[3] * 255.0f);
}

/**	Sample color from color textures at normalized UV coordinate ∈
	[0.0f, 1.0f]. Coordinates over 1.0f loop causing a repeating texture
	effect.

	Does not handle filtering or mipmaps (yet).

	@param[in]     pTex		Texture to sample from.
	@param[in,out] pColor	Sets color at this address to sampled color
							∈ [0.0f, 1.0f].
	@param[in]     u		Normalized x coordinate ∈ [0.0f, 1.0f] to
							sample from.
	@param[in]     v		Normalized y coordinate ∈ [0.0f, 1.0f] to
							sample from. */
SGL_INLINE void sglTex2DSample(const SGLTex2D* pTex, SGLFloat4 pColor,
	float u, float v)
{
	uint32_t x, y;

	x = (uint32_t)(u * (float)pTex->w);
	y = (uint32_t)(v * (float)pTex->h);
	x %= pTex->w;
	y %= pTex->h;

	sglBGRAToFloat4(pTex->pBGRA[y * pTex->w + x], pColor);
}