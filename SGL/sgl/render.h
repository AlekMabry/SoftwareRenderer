#pragma once

#include "common.h"
#include "image.h"
#include "vector.h"

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

/**	Creates a shader.
	@param[in]	vertShd			User vertex shader function
								(see SGLShader void(*vertShd) for prototype)
	@param[in]	fragShd			User fragment shader function
								(see SGLShader void(*fragShd) for prototype)
	@param[in]	vertAttrCount	*/
SGL_EXPORT void sglCreateShader(const void* pVertShd, const void* pFragShd,
	uint32_t vertAttrs, SGLShader* pShader);

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