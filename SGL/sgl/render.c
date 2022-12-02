#include "render.h"

void sglRenderTris(SGLShader* shd, SGLTex2D* frameBuf, SGLTex2D* depthBuf,
	void* mdl, void* uniforms, uint32_t nTris)
{
	SGLVec2I screenBounds = { frameBuf->w - 1, frameBuf->h - 1 };

	// Calculate dimensions and allocate vertex attributes buffer
	// to pass interpolated per-frag values to the fragment
	// shader from the vertex shader:
	uint32_t attrBufSize = 0;
	for (uint32_t i = 0; i < shd->nVertValues; i++)
	{
		switch (shd->vertValues[i])
		{
		case SGL_SHD_FLOAT:
			attrBufSize += 1;
			break;

		case SGL_SHD_VEC2:
			attrBufSize += 2;
			break;

		case SGL_SHD_VEC3:
			attrBufSize += 3;
			break;

		case SGL_SHD_MAT4:
			attrBufSize += 16;
			break;
		}
	}

	// This would be like Out from GLSL, it is 3x the size of fIn (3 vertices)
	float* vOut = malloc(attrBufSize * sizeof(float) * 3);

	// This would be like In from GLSL
	float* fIn = malloc(attrBufSize * sizeof(float));		// Single set of interpolated values

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
						for (uint32_t i = 0; i < attrBufSize; i++)
						{
							fIn[i] = (vOut[i] * v0Weight) + (vOut[i + attrBufSize] * v1Weight) + (vOut[i + (2 * attrBufSize)] * v2Weight);
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
}