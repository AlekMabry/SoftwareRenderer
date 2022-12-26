#include "render.h"

#include <sys\timeb.h>

void sglCreateShader(const void* pVertShd, const void* pFragShd,
	uint32_t vertAttrs, SGLShader* pShader)
{
	pShader->sType = SGL_TYPE_SHADER;
	pShader->pVertShd = pVertShd;
	pShader->pFragShd = pFragShd;
	if (vertAttrs % 4 > 0)
		vertAttrs += vertAttrs % 4;
	pShader->vertAttributes = vertAttrs;
	pShader->vertSize = 4 + vertAttrs;
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

		pMesh->pShd->pVertShd(pMesh->pShd->vertSize, pMesh->pMesh,
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

void sglBinTris(const float* pVertCache, SGLMeshInfo* pMesh,
	uint32_t triStart, uint32_t tris, SGLTargetInfo* pTarget)
{
	// For cache indexing
	uint32_t vertSize = pMesh->pShd->vertSize;
	uint32_t triSize = vertSize * 3;

	uint32_t laneCount = 4;	// 4 lanes for 128 bit SSE
	uint32_t endTri = triStart + tris;
	for (uint32_t tri = triStart; tri < endTri; tri += 4)
	{
		if (tri + 4 > endTri)
			laneCount = endTri - tri + 1;

		/*	Storing x, y, z, w for 3 vertices of[laneCount] triangles:
		* 
		*	packedPosition[vertex] = 
		*	[[x0, x1, x2, xn],[y0, y1, y2, yn],[z0, z1, z2, zn]]
		*	
		*	Where n correlates to the triangle (laneCount) */
		SGLFloat4x4 packedPos[3];
		float* cacheCurTri = &pVertCache[triSize * tri];
		sglSwizzle4Tris(vertSize, cacheCurTri, laneCount,
			packedPos);

		SGLFixed4x4 packedFixedPos[3];
		for (uint32_t i = 0; i < 3; i++)
		{
			sglFloat4x4ToFixed4x4(packedPos[i], packedFixedPos[i]);
		}

		// Find bounding box for screenspace triangles
		SGLFixed4 packedStartX, packedStartY, packedEndX, packedEndY,
			packedW, packedH, zero, one, packedScreenArea;
		sglFixed4Set1(0, &zero);
		sglFixed4Set1(1, &one);
		sglFixed4Set1(pTarget->w, &packedW);
		sglFixed4Set1(pTarget->h, &packedH);

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
			if (packedPos[0][3][i] > 1.0f ||
				packedPos[1][3][i] > 1.0f ||
				packedPos[2][3][i] > 1.0f) continue;

			uint32_t startX = max(packedStartX[i] / pTarget->tileW, 0);
			uint32_t startY = max(packedStartY[i] / pTarget->tileH, 0);

			uint32_t endX = min(packedEndX[i] / pTarget->tileW,
				pTarget->xTiles - 1);
			uint32_t endY = min(packedEndY[i] / pTarget->tileH,
				pTarget->yTiles - 1);

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