#pragma once

#include "common.h"
#include "image.h"
#include "vector.h"

/**	Render Pipeline:
	-# 	Vertex shader transforms vertices to clip coordinates and
		outputs any per-vertex attributes needed in the fragment shader.
	-# 	After doing the perspective divide (divide <x, y, z> by w) on the
		clip coordinates to get NDC (normalized device coordinates), the
		rasterizer calculates pixel positions of the triangles on the texture,
		and interpolates the per-vertex values given by the vertex
		shader into values for the particular fragment (pixel).
	-# 	The fragment shader is called, with the interpolated
		fragment values fed in, and it calculates the color to
		be drawn at that particular fragment. */
typedef struct SglPipeline
{
	SglType type;

	uint32_t vertStride;	/** Size of vertex in floats. */

	/** The number of scalar vertex attributes to interpolate and pass from
		the vertex shader to the fragment shader.

		For example if you wanted to pass an interpolated position (Float3) and
		texture coordinate (Float2) to the fragment shader, this value would
		be 5. */
	uint32_t vertAttributeCount;

	/** The vertex shader serves the purpose of returning
		a triangle in right-hand clip space, other vertex attributes such as
		position/normals/UVs.
		@param[in,out]	vertAttrBuf	Output vertex attributes for triangle.
									Stored in [x0, y0, z0, w0, attr0, ...,
									x1, y1, z1, w1, attr1, ...] order.
		@param[in]		vertSize	Size of each vertex (in float count)
									for reliable indexing of vertAttrBuf.
		@param[in]		mdl			The pointer to the model to calculate
									the output triangle from. There is no
									predefined model format, the user can
									give the renderer any model they want
									and interpret it in this function.
		@param[in]		uniforms	Unchanging variables throughout the render
									(ie: perspective/lighting/textures).
		@param[in]		tri			Index of triangle to calculate. */
	void(*pVertShd)(const uint32_t vertSize, const void* pMesh,
		const void* pUniforms, const uint32_t tri, float* pOut);

	/** The fragment shader serves the purpose of returning
		the color for the current fragment (pixel) using the passed-in
		interpolated vertex attributes, as well as anything else given in
		the uniform buffer.
		@param[in,out]	color		Output color for this fragment.
		@param[in]		fragAttrBuf	Interpolated vertex attributes at this
									fragment.
									(ie: position, uv, normal).
		@param[in]		uniforms	Unchanging variables throughout the render
									(ie: perspective/lighting/textures). */
	void(*pFragShd)(const float* pIn, const void* pUniforms,
		SGLBGRA* pColor);
} SglPipeline;

typedef struct SglRenderpass
{
	SglImage* pFramebuffer;
	SglBGRA clearColor;
	float clearDepth;
} SglRenderpass;

typedef struct SGLMeshInfo
{
	SGLType sType;
	SGLShader* pShd;
	void* pUniforms;
	void* pMesh;
	uint32_t triStart;
	uint32_t tris;
} SGLMeshInfo;

typedef struct SGLBin {
	SGLType sType;
	uint32_t* pTris;
	uint32_t tris;
	uint32_t size;
} SGLBin;

typedef struct SGLTargetInfo {
	SGLType sType;
	SGLTex2D* pColorTex;
	SGLTex2D* pDepthTex;
	uint32_t w;
	uint32_t h;
	uint32_t xTiles;	/** Target texture width in tiles. */
	uint32_t yTiles;	/** Target texture height in tiles. */
	uint32_t tileW;		/** Tile width in pixels. */
	uint32_t tileH;		/** Tile height in pixels. */
	SGLBin* pBins;
} SGLTargetInfo;

typedef struct SGLInstance
{
	SGLType sType;
	float* pVertCache;		/** Transformed vertex cache. */
	uint32_t vertCacheSize;	/** Vertex cache memory allocated size. */
	uint32_t vertCacheSP;	/** Vertex cache stack pointer. */
	SGLTargetInfo* pTarget;
} SGLInstance;

/** Creates a target info struct. */
SGL_EXPORT void sglCreateTargetInfo(SGLTargetInfo* target,
	const void* colorTarget, const void* depthTarget);

/** Runs vertex shader and caches vertices. */
SGL_EXPORT void sglCacheTris(const SGLMeshInfo* pMesh,
	const SGLTargetInfo* pTarget, float* pCache);

SGL_EXPORT void sglSwizzle4Tris(const float* pVertCache, uint32_t vertSize,
	uint32_t lanes, SGLFloat4x4 pDest[3]);

/** Bins cached triangles. */
SGL_EXPORT void sglBinTris(const float* pVertCache, SGLMeshInfo* pMesh,
	uint32_t triStart, uint32_t tris, SGLTargetInfo* pTarget);

/**
	@ingroup Render
	@brief Renders triangles to texture.
	@param[in]     shd		Shader.
	@param[in,out] frameBuf	Color texture to render to.
	@param[in,out] depthBuf	Depth texture to render to and check against.
	@param[in]     mdl		Model buffer, shader defines how to read this.
	@param[in]     uniforms	Buffer of unchanging values for shader.
	@param[in]     nTris	Number of triangles to render.
*/
SGL_EXPORT void sglRenderTris(SGLShader* shd, SGLTex2D* frameBuf, SGLTex2D* depthBuf,
	void* mdl, void* uniforms, uint32_t nTris);