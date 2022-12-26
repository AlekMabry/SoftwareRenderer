#pragma once

#include "common.h"
#include "vector.h"

#pragma pack(1)

/** 24-bit color. (little-endian member layout) */
typedef struct SglBGR {
	uint8_t b, g, r;
} SglBGR;

/** 32-bit color (little-endian member layout). */
typedef struct SglBGRA {
	uint8_t b, g, r, a;
} SglBGRA;

/** BC1 4x4 pixel block (DXT1 3-color opaque). */
typedef struct SglBC1 {
	uint16_t color0;
	uint16_t color3;
	uint8_t texelColors[4];
} SglBC1;

#pragma pack(pop)

/** The image class, just about everything reads from or writes to this. */
typedef struct SglImage
{
	SglType type;		/** Type, see SGL_TYPE_IMAGE_ macros. */
	uint32_t w;			/** Width */
	uint32_t h;			/** Height */
	uint32_t mipCount;	/** Number of mipmaps. */
	SglBGRA* pBGRA;		/** BGRA color buffer. Size: w * h */ 
	SglBC1* pBC1;		/** BC1 color buffer. Size: (w * h) / 16 */
	float* pFloat;		/** Float depth buffer. Size: w * h */
} SglImage;

/** Create image.
	
	@param[in]		sType	SGL structure type.
	@param[in]		w		Width
	@param[in]		h		Height
	@param[in]		mipmaps	Number of mipmaps (1 minimum).
	@param[in,out]	pImage	Uninitialized image to initialize. */
SGL_EXPORT void sglCreateImage(SglType type, uint32_t w, uint32_t h,
	uint32_t mipCount, SglImage* pImage);

/** Image cleanup. */
SGL_EXPORT void sglDestroyImage(SglImage* pImage);

/**	Converts BGRA to normalized Float4 color, where channels from [0, 255]
	become [0.0f, 1.0f].

	@param[in]		in		Color in SglBGRA.
	@param[in,out]	pOut	Normalized color in SglFloat4. */
SGL_INLINE void sglBGRAToFloat4(SglBGRA in, SGLFloat4 pOut)
{
	pOut[0] = (float)in.b * (1.0f / 255.0f);
	pOut[1] = (float)in.g * (1.0f / 255.0f);
	pOut[2] = (float)in.r * (1.0f / 255.0f);
	pOut[3] = (float)in.a * (1.0f / 255.0f);
}

/**	Converts Float4 normalized color to BGRA, where channels from [0.0f, 1.0f]
	become [0, 255].

	@param[in]		in		Normalized color in Float4.
	@param[in,out]	pOut	Color in BGRA. */
SGL_INLINE void sglFloat4ToBGRA(const SGLFloat4 pIn, SglBGRA *pOut)
{
	pOut->b = (uint8_t)(pIn[0] * 255.0f);
	pOut->g = (uint8_t)(pIn[1] * 255.0f);
	pOut->r = (uint8_t)(pIn[2] * 255.0f);
	pOut->a = (uint8_t)(pIn[3] * 255.0f);
}

/**	Sample color from color images at normalized UV coordinate ∈
	[0.0f, 1.0f]. Coordinates over 1.0f loop causing a repeating texture
	effect.

	Does not handle filtering or mipmaps (yet).

	@param[in]     pImage	Image to sample from.
	@param[in,out] pColor	Sets color at this address to sampled color
							∈ [0.0f, 1.0f].
	@param[in]     u		Normalized x coordinate ∈ [0.0f, 1.0f] to
							sample from.
	@param[in]     v		Normalized y coordinate ∈ [0.0f, 1.0f] to
							sample from. */
SGL_INLINE void sglTex2DSample(const SglImage* pImage, SGLFloat4 pColor,
	float u, float v)
{
	uint32_t x, y;

	x = (uint32_t)(u * (float)pImage->w);
	y = (uint32_t)(v * (float)pImage->h);
	x %= pImage->w;
	y %= pImage->h;

	sglBGRAToFloat4(pImage->pBGRA[y * pImage->w + x], pColor);
}