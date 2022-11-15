#pragma once

#include "math.h"
#include <stdbool.h>
#include <stdlib.h>

/**
	@defgroup SGLTex2D Texture
	@brief The core of the library, almost all functions either read from
	or write to textures.
*/

/**
	@ingroup SGLTex2D
	@brief Raw format is a SGLBGR24 rawBuf array of size w * h in row-major
	order.
*/
#define SGL_TEX2D_RAW	1

/**
	@ingroup SGLTex2D
	@brief Depth format is a float depthBuf array of size w * h in row-major
	order.
*/
#define SGL_TEX2D_DEPTH	2

#pragma pack(1)

/**
	@ingroup SGLTex2D
	@brief 24-bit color.
*/
typedef struct SGLBGR24 {
	uint8_t b, g, r;
} SGLBGR24;

/**
	@ingroup SGLTex2D
	@brief 32-bit color.
*/
typedef struct SGLBGR32 {
	uint8_t b, g, r, a;
} SGLBGR32;

/**
	@ingroup SGLTex2D
	@brief The texture class, just about everything reads from or writes to this.
*/
typedef struct SGLTex2D
{
	int32_t type;			// Type, see TEX2D_ macros above
	int32_t w;				// Width
	int32_t h;				// Height
	int32_t l;				// Left offset if applicable (for use with sprite textures)
	int32_t t;				// Top offset if applicable (for use with sprite textures)
	int32_t mipmaps;		// Mipmaps, not currently being used
	SGLBGR32* rawBuf;		// Raw color buffer. Size: w * h
	float* depthBuf;		// Depth buffer. Size: w * h, pixels are depth.
} SGLTex2D;

/**
	@internal
	@ingroup SGLTex2D
	@brief BMP file header.
*/
typedef struct SGLBMPHeader
{
	uint8_t szVersion[2];	// Should have 'BM' signature
	uint32_t nSize;			// File size in bytes
	uint32_t pad;			// Unused
	uint32_t nOffset;		// Offset from beginning of file to data;

	/* Info header is right after first header, so it is merged into this class */
	uint32_t nInfoSize;		// Size of this info part of the header (=40)
	uint32_t nWidth;		// Image width in pixels
	uint32_t nHeight;		// Image height in pixels
	uint16_t nPlanes;		// Number of planes (=1)
	uint16_t nPxBits;		// Number of bits per pixel. This also indicates palette method:
						// 1-bit:	Monochrome
						// 4-bit:	16-color palette
						// 8-bit:	256-color palette
						// 16-bit:	65565 colors
						// 24-bit:	16M colors
	uint32_t nCompression;	// Type of compression:
						// 0: BI_BGR	no-compression
						// 1: BI_RLE8	8-bit RLE encoding
						// 2: BI_RLE4 	4-bit RLE encoding
	uint32_t nImageSize;		// Size of image if compressed, (can =0 if not compressed)
	uint32_t nXPxPerM;		// Horizontal resolution, pixels per meter
	uint32_t nYPxPerM;		// Vertical resolution, pixels per meter
	uint32_t nColors;		// Number of actually used colors (ie. 256 for 8-bit color)
	uint32_t nImptColors;	// Number of important colors (=0 for all) 
} SGLBMPHeader;

#pragma pack(pop)

/**
	@ingroup SGLTex2D
	@brief Initialize texture.

	@param[in,out] tex	Uninitialized texture to initialize.
	@param[in]     type	Type of texture. See TEX2D_ macros for options.
	@param[in]     w	Width
	@param[in]     h	Height
	@param[in]     l	(Optional) left offset for use with sprite textures.
	@param[in]     t	(Optional) top offset for use with sprite textures.
*/
void sglTex2DInit(SGLTex2D * tex, int32_t type, int32_t w,
	int32_t h, int32_t l, int32_t t);

/**
	@ingroup SGLTex2D
	@brief Free texture.

	@param[in,out] tex	Texture to free.
*/
void sglTex2DFree(SGLTex2D* tex);

/**
	@ingroup SGLTex2D
	@brief Fill texture with color. Only works with raw textures.

	@param[in,out] tex		Texture to color fill.
	@param[in]     color	Color to fill texture with.
*/
void sglTex2DColorFill(SGLTex2D* tex, SGLBGR32 color);

/**
	@ingroup SGLTex2D
	@brief Reset depth texture. Set all pixels to far depth.

	@param[in,out] tex	Depth texture to reset.
*/
void sglTex2DDepthReset(SGLTex2D* tex);

/**
	@ingroup SGLTex2D
	@brief Converts SGLBGR32 to normalized SGLVec4 color, where
	channels from [0, 255] become [0.0f, 1.0f]

	@param[in]		in	Color in SGLBGR32.
	@param[in,out] out	Normalized color in SGLVec4.
*/
SGL_INLINE void sglBGR32ToVec4(SGLBGR32 in, SGLVec4 out)
{
	out[0] = (float)in.r / 255.0f;
	out[1] = (float)in.g / 255.0f;
	out[2] = (float)in.b / 255.0f;
	out[3] = (float)in.a / 255.0f;
}

/**
	@ingroup SGLTex2D
	@brief Converts Vec4 normalized color to BGR32, where channels from 
	[0.0f, 1.0f] become [0, 255].

	@param[in]		in	Normalized color in SGLVec4.
	@param[in,out] out	Color in SGLBGR32.
*/
SGL_INLINE void sglVec4ToBGR32(SGLVec4 in, SGLBGR32 *out)
{
	out->r = (uint8_t)(in[0] * 255.0f);
	out->g = (uint8_t)(in[1] * 255.0f);
	out->b = (uint8_t)(in[2] * 255.0f);
	out->a = (uint8_t)(in[3] * 255.0f);
}

/**
	@ingroup SGLTex2D
	@brief Sample color from color textures at normalized UV coordinate
	(0.0-1.0). Coordinates over 1.0 loop causing a repeating texture effect.

	Does not handle filtering or mip maps (yet).

	@param[in]     tex	Texture to sample from.
	@param[in,out] col	Sets color at this address to sampled color
						(0.0f < r, g, b, a < 1.0f)
	@param[in]     u	Normalized (0.0-1.0) x coordinate to sample from.
	@param[in]     v	Normalized (0.0-1.0) y coordinate to sample from.
*/
SGL_INLINE void sglTex2DSample(SGLTex2D* tex, SGLVec4 col, float u, float v)
{
	int32_t x, y;

	x = (int32_t)(u * (float)tex->w);
	y = (int32_t)(v * (float)tex->h);
	x %= tex->w;
	y %= tex->h;

	sglBGR32ToVec4(tex->rawBuf[x + y * tex->w], col);
}

/**
	@ingroup SGLTex2D
	@brief Loads BMP image.

	Supports standard uncompressed 24-bit color BMP images.

	@param[in,out] tex			Uninitialized texture to load into.
	@param[in]     fname		File name of BMP.
*/
void sglTex2DLoadBMP(SGLTex2D* tex, uint8_t* fname);

/**
	@ingroup SGLTex2D
	@brief Saves BMP image.

	Creates standard uncompressed 24-bit color BMP images.

	@param[in] tex		Texture to load from.
	@param[in] fname	File name of BMP to save to.
*/
void sglTex2DSaveBMP(SGLTex2D* tex, uint8_t* fname);