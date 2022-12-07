#include "render.h"

#include <sys\timeb.h>

void sglCreateShader(SGLShader* shader, const void* vertShd,
	const void* fragShd, uint32_t vertAttributes)
{
	shader->sType = SGL_TYPE_SHADER;
	shader->vertShd = vertShd;
	shader->fragShd = fragShd;
	if (vertAttributes % 4 > 0)
		vertAttributes += vertAttributes % 4;
	shader->vertAttributes = vertAttributes;
	shader->vertSize = 4 + vertAttributes;
}

void sglCacheTris(const SGLMeshInfo* pMesh, const SGLTargetInfo* pTarget,
	float* pCache)
{
	// For cache indexing
	uint32_t vertSize = pMesh->pShd->vertSize;
	uint32_t triSize = vertSize * 3;

	// For clipspace to screenspace
	uint32_t w = pTarget->w;
	uint32_t h = pTarget->h;
	SGLFloat4 offsetTransform = { 1.0f, 1.0f, 0.0f, 0.0f };
	SGLFloat4 screenTransform = { (float)w * 0.5f, (float)h * 0.5f,
		1.0f, 1.0f };

	for (uint32_t i = 0; i < pMesh->tris; i++)
	{
		uint32_t tri = i + pMesh->triStart;
		uint32_t triIndex = triSize * tri;
		uint32_t v0Index = triIndex;
		uint32_t v1Index = triIndex + vertSize;
		uint32_t v2Index = v1Index + vertSize;

		pMesh->pShd->vertShd(pMesh->pShd->vertSize, pMesh->pMesh,
			pMesh->pUniforms, tri, &pCache[triIndex]);
		
		sglFloat4PerspectiveDiv(&pCache[v0Index], &pCache[v0Index]);
		sglFloat4Add(&pCache[v0Index], &offsetTransform, 
			&pCache[v0Index]);
		sglFloat4Mul(&pCache[v0Index], &screenTransform, 
			&pCache[v0Index]);

		sglFloat4PerspectiveDiv(&pCache[v1Index], &pCache[v1Index]);
		sglFloat4Add(&pCache[v1Index], &offsetTransform,
			&pCache[v1Index]);
		sglFloat4Mul(&pCache[v1Index], &screenTransform,
			&pCache[v1Index]);

		sglFloat4PerspectiveDiv(&pCache[v2Index], &pCache[v2Index]);
		sglFloat4Add(&pCache[v2Index], &offsetTransform, 
			&pCache[v2Index]);
		sglFloat4Mul(&pCache[v2Index], &screenTransform, 
			&pCache[v2Index]);
	}
}

void sglSwizzle4Tris(const float* pVertCache, uint32_t vertSize, uint32_t lanes,
	SGLFloat4x4 pDest[3])
{
	uint32_t triSize = vertSize * 3;

	// Each lane corresponds to one triangle
	for (uint32_t lane = 0; lane < lanes; lane++)
	{
		for (uint32_t vert = 0; vert < 3; vert++)
		{
			uint32_t vertCacheIndex = (triSize * lane) + (vert * vertSize);
			pDest[vert][0][lane] = pVertCache[vertCacheIndex];
			pDest[vert][1][lane] = pVertCache[vertCacheIndex + 1];
			pDest[vert][2][lane] = pVertCache[vertCacheIndex + 2];
			pDest[vert][3][lane] = pVertCache[vertCacheIndex + 3];
		}
	}
}

void sglBinTris(SGLInstance* pContext, uint32_t triStart,
	uint32_t tris)
{
	// For cache indexing
	uint32_t vertSize = info->shd->vertSize;
	uint32_t triSize = vertSize * 3;

	uint32_t laneCount = 4;	// 4 lanes for 128 bit SSE
	for (uint32_t tri = start; tri < end; tri += 4)
	{
		if (tri + 4 > end)
			laneCount = end - tri + 1;

		/*	Storing x, y, z, w for 3 vertices of[laneCount] triangles:
		* 
		*	packedPosition[vertex] = 
		*	[[x0, x1, x2, xn],[y0, y1, y2, yn],[z0, z1, z2, zn]]
		*	
		*	Where n correlates to the triangle (laneCount) */
		SGLPackedFloat4 packedPos[3];
		float* cacheCurTri = &cache[triSize * tri];
		sglSwizzle4Tris(vertSize, cacheCurTri, laneCount,
			packedPos);

		SGLPackedFixed4 packedFixedPos[3];
		for (uint32_t i = 0; i < 3; i++)
		{
			sglPackedFloat4ToPackedFixed4(packedPos[i], packedFixedPos[i]);
		}

		// Find bounding box for screenspace triangles
		SGLFixed4 packedStartX, packedStartY, packedEndX, packedEndY,
			packedW, packedH, zero, one, packedScreenArea;
		sglFixed4(0, &zero);
		sglFixed4(1, &one);
		sglFixed4(info->frameTarget->w, &packedW);
		sglFixed4(info->frameTarget->h, &packedH);

		// Start X
		sglFixed4Min(&packedFixedPos[0][0], &packedFixedPos[1][0], &packedStartX);
		sglFixed4Min(&packedStartX, &packedFixedPos[2][0], &packedStartX);
		sglFixed4Max(&packedStartX, &zero, &packedStartX);

		// Start Y
		sglFixed4Min(&packedFixedPos[0][1], &packedFixedPos[1][1], &packedStartY);
		sglFixed4Min(&packedStartY, &packedFixedPos[2][1], &packedStartY);
		sglFixed4Max(&packedStartY, &zero, &packedStartY);

		// End X
		sglFixed4Max(&packedFixedPos[0][0], &packedFixedPos[1][0], &packedEndX);
		sglFixed4Max(&packedEndX, &packedFixedPos[2][0], &packedEndX);
		sglFixed4Add(&packedEndX, &one, &packedEndX);
		sglFixed4Min(&packedEndX, &packedW, &packedEndX);

		// End Y
		sglFixed4Max(&packedFixedPos[0][1], &packedFixedPos[1][1], &packedEndY);
		sglFixed4Max(&packedEndY, &packedFixedPos[2][1], &packedEndY);
		sglFixed4Add(&packedEndY, &one, &packedEndY);
		sglFixed4Min(&packedEndY, &packedH, &packedEndY);

		for (uint32_t i = 0; i < laneCount; i++)
		{
			// Todo - Discard if screen size <= 0

			// Reject if any verts are behind nearclip
			if (packedPos[0][3].m128_f32[i] > 1.0f ||
				packedPos[1][3].m128_f32[i] > 1.0f ||
				packedPos[2][3].m128_f32[i] > 1.0f) continue;

			uint32_t startX = max(packedStartX.m128i_i32[i] / info->tileWPx, 0);
			uint32_t startY = max(packedStartY.m128i_i32[i] / info->tileHPx, 0);

			uint32_t endX = min(packedEndX.m128i_i32[i] / info->tileWPx,
				info->targetXTileCount - 1);
			uint32_t endY = min(packedEndY.m128i_i32[i] / info->tileHPx,
				info->targetYTileCount - 1);

			uint32_t row, col;
			for (row = startY; row <= endY; row++)
			{
				for (col = startX; row <= endX; col++)
				{

				}
			}
			
		}
	}
}


void sglRenderTris(SGLShader* shd, SGLTex2D* frameBuf, SGLTex2D* depthBuf,
	void* mdl, void* uniforms, uint32_t nTris)
{
	// Benchmarking (Windows only)
	struct timeb startTime, endTime;
	ftime(&startTime);

	SGLVec2I screenBounds = { frameBuf->w - 1, frameBuf->h - 1 };

	// This would be like Out from GLSL, it is 3x the size of fIn (3 vertices)
	float* vOut = malloc(4 * sizeof(float) * 3);

	// This would be like In from GLSL
	float* fIn = malloc(4 * sizeof(float));		// Single set of interpolated values

	for (uint32_t tri = 0; tri < nTris; tri++)
	{
		SGLTriVec4 vTri;	// Triangle from vertex shader (floating point, clip space)

		// Get current triangle in NDC from vertex shader
		shd->vertShd(vTri, vOut, mdl, uniforms, tri, nTris);

		sglVec3PerspectiveDivide(vTri[0], vTri[0]);
		sglVec3PerspectiveDivide(vTri[1], vTri[1]);
		sglVec3PerspectiveDivide(vTri[2], vTri[2]);

		/** Todo!!! - Must clip triangles to prevent overflow when converted to fixed point! */

		SGLTriVec2I cTri;	// Current triangle (fixed point, screen coordinates)
		sglVec2IScreenSpaceFromNDC(vTri[0], frameBuf->w, frameBuf->h, 1, cTri[0]);
		sglVec2IScreenSpaceFromNDC(vTri[1], frameBuf->w, frameBuf->h, 1, cTri[1]);
		sglVec2IScreenSpaceFromNDC(vTri[2], frameBuf->w, frameBuf->h, 1, cTri[2]);

		SGLVec2I triMin, triMax;	// Triangle bounding box
		sglTriVec2IBBox(cTri, triMin, triMax);
		sglVec2IClipBBox(triMin, triMax, (SGLVec2I) { 0, 0 }, screenBounds);

		SGLVec2I p;	// Current output pixel position
		for (p[1] = triMin[1]; p[1] <= triMax[1]; p[1]++)
		{
			for (p[0] = triMin[0]; p[0] <= triMax[0]; p[0]++)
			{
				uint64_t pixelIndex = (screenBounds[1] - p[1]) * frameBuf->w + p[0];

				// Determine barycentric coordinates
				int32_t w0 = sglVec2IOrient(cTri[1], cTri[2], p);
				int32_t w1 = sglVec2IOrient(cTri[2], cTri[0], p);
				int32_t w2 = sglVec2IOrient(cTri[0], cTri[1], p);

				// If p is on or inside all edges, render pixel
				if ((w0 | w1 | w2) >= 0)
				{
					//frameBuf->rawBuf[pixelIndex] = (SGLBGR32){ 255, 0, 0, 255 };//fragColor;

					SGLVec2 pNDC;	// Convert p back to NDC to get barycentric coords from triangle
					sglVec2NDCToScreenspace(p, frameBuf->w, frameBuf->h, 1, pNDC);
					
					float v0Weight, v1Weight, v2Weight;
					sglVec2Barycentric(pNDC, vTri[0], vTri[1], vTri[2], &v0Weight, &v1Weight, &v2Weight);

					float fragZ = (v0Weight * vTri[0][2]) + (v1Weight * vTri[1][2]) + (v2Weight * vTri[2][2]);

					// Depth check
					if (fragZ <= depthBuf->depthBuf[pixelIndex])
					{
						// Interpolate all vOut values to fIn values for the fragment shader
						for (uint32_t i = 0; i < 4; i++)
						{
							fIn[i] = (vOut[i] * v0Weight) + (vOut[i + 4] * v1Weight) + (vOut[i + (2 * 4)] * v2Weight);
						}

						SGLBGR32 fragColor;
						shd->fragShd(&fragColor, fIn, uniforms); // fIn not defined
						depthBuf->depthBuf[pixelIndex] = fragZ;
						frameBuf->rawBuf[pixelIndex] = fragColor;
					}
				}
			}
		}
	}

	free(vOut);
	free(fIn);

	ftime(&endTime);
	uint64_t dt = (uint64_t) (1000.0 * (endTime.time - startTime.time) + (endTime.millitm - startTime.millitm));
	printf("[INFO] Render took %u milliseconds\n", dt);
}