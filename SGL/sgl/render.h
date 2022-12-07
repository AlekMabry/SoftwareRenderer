#pragma once

#include "common.h"
#include "texture.h"
#include "vector.h"

/**	User-programmed and passed to renderer. Calculates on-screen
	coordinates of every triangle vertex, as well as the color for any given
	pixel being rasterized.

	Render Pipeline:
	-# 	Vertex shader feeds in triangle coordinates in NDC format
		(Normalized Device Coordinates), as well as other
		per-vertex values it would like to interpolate and later
		feed to the frag shader.
	-# 	Using these NDC triangle coordinates, the rasterizer
		calculates pixel positions of the triangles on the texture,
		and interpolates the per-vertex values given by the vertex
		shader into values for the particular fragment (pixel).
	-# 	The fragment shader is called, with the interpolated
		fragment values fed in, and it calculates the color to
		be drawn at that particular fragment. */
typedef struct SGLShader
{
	SGLType sType;

	/** The number of scalar vertex attributes to interpolate and pass between
		the vertex and fragment shader.
		
		For example if you wanted to pass an interpolated position and UV
		coordinate to the fragment shader, this value would be 5. */
	uint32_t vertAttributes;

	/**	Size of each vertex in floats (must be a multiple of 4). */
	uint32_t vertSize;

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
	void(*vertShd)(const uint32_t vertSize, const void* pMesh,
		const void* pUniforms, const uint32_t tri, float* pVertAttrBuf);

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
	void(*fragShd)(const float* pFragAttrBuf, const void* pUniforms,
		SGLBGRA* pColor);
} SGLShader;

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

#define SGL_TYPE_TARGET_INFO

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

/**	Creates a shader.
	@param[in]	vertShd			User vertex shader function
								(see SGLShader void(*vertShd) for prototype)
	@param[in]	fragShd			User fragment shader function
								(see SGLShader void(*fragShd) for prototype)
	@param[in]	vertAttrCount	*/
SGL_EXPORT void sglCreateShader(SGLShader* shader, const void* vertShd,
	const void* fragShd, uint32_t vertAttrCount);

/** Creates a target info struct. */
SGL_EXPORT void sglCreateTargetInfo(SGLTargetInfo* target,
	const void* colorTarget, const void* depthTarget);

/** Runs vertex shader and caches vertices. */
SGL_EXPORT void sglCacheTris(const SGLMeshInfo* pMesh,
	const SGLTargetInfo* pTarget, float* pCache);

SGL_EXPORT void sglSwizzle4Tris(const float* pVertCache, uint32_t vertSize,
	uint32_t lanes, SGLFloat4x4 pDest[3]);

/** Bins cached triangles. */
SGL_EXPORT void sglBinTris(SGLInstance* pContext, uint32_t triStart,
	uint32_t tris);

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