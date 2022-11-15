#include "render.h"

void sglRenderTris(SGLShader* shd, SGLTex2D* frameBuf, SGLTex2D* depthBuf,
	void* mdl, void* uniforms, uint32_t nTris)
{
	bool middle_line_drawn = false;
	float x_delta;
	float y_delta;
	float left_delta;
	float right_delta;
	float left_x;
	float right_x;
	int32_t y, lX, rX;
	int32_t left_index, right_index;
	int32_t width;

	float ftemp;
	float z; // Interpolated value to depth check

	// Stores baryocentric coordinates for current fragment
	float u, v, w;
	float bc[3];

	// Vertex coordinates. P is the point to get baryocentric coordinate for
	SGLVec2 p, a, b, c;

	SGLBGR32 fragColor;	// Final color to write to fragment

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
	float *vOut = malloc(attrBufSize * sizeof(float) * 3);

	// This would be like In from GLSL
	float *fIn = malloc(attrBufSize * sizeof(float));		// Single set of interpolated values

	SGLTri cTri;			// Current Triangle in NDC
	SGLVec2I cTriPx[3];		// Current Triangle in pixels

	for (uint32_t tri = 0; tri < nTris; tri++)
	{
		// Get current triangle in NDC from vertex shader
		shd->vertShd(cTri, vOut, mdl, uniforms, tri, nTris);

		// Triangle is not outside NDC range, it is on screen (Logic removed)
		if (1 == 1)
		{
			// This triangle may not be in the correct order.
			// y0 should be lowest in texture coordinates, then y1 and y2.
			// This means y0 should be the highest in NDC coordinates.

			// In VI (Vertex Indices), the array index is the index of the final point,
			// and it's value is the point from the original tri it get's it's
			// coordinates from.
			uint32_t VI[3];
			VI[0] = 0;
			VI[1] = 1;
			VI[2] = 2;

			if (cTri[VI[0]][1] < cTri[VI[1]][1])
			{
				uint32_t temp = VI[0];
				VI[0] = VI[1];
				VI[1] = temp;
			}

			if (cTri[VI[0]][1] < cTri[VI[2]][1])
			{
				uint32_t temp = VI[0];
				VI[0] = VI[2];
				VI[2] = temp;
			}

			if (cTri[VI[1]][1] < cTri[VI[2]][1])
			{
				uint32_t temp = VI[1];
				VI[1] = VI[2];
				VI[2] = temp;
			}

			// By now, VI[0] points to the point with the lowest
			// y in texture coordinates, and VI[2] points to the highest

			// Convert NDC coordinates into tex coordinates for rasterization process
			cTriPx[0][0] = (int32_t)(((cTri[VI[0]][0] * 0.5) + 0.5) * (float)frameBuf->w);
			cTriPx[0][1] = (int32_t)(((-cTri[VI[0]][1] * 0.5) + 0.5) * (float)frameBuf->h);
			cTriPx[1][0] = (int32_t)(((cTri[VI[1]][0] * 0.5) + 0.5) * (float)frameBuf->w);
			cTriPx[1][1] = (int32_t)(((-cTri[VI[1]][1] * 0.5) + 0.5) * (float)frameBuf->h);
			cTriPx[2][0] = (int32_t)(((cTri[VI[2]][0] * 0.5) + 0.5) * (float)frameBuf->w);
			cTriPx[2][1] = (int32_t)(((-cTri[VI[2]][1] * 0.5) + 0.5) * (float)frameBuf->h);

			// Prepare F64 triangle point vectors for calculating barycentric coords later
			//Userd to use VI[0]
			a[0] = cTriPx[0][0];
			a[1] = cTriPx[0][1];
			b[0] = cTriPx[1][0];
			b[1] = cTriPx[1][1];
			c[0] = cTriPx[2][0];
			c[1] = cTriPx[2][1];

			// Rasterize Bottom Half (Tex coordinates, not NDC)
			if (cTriPx[0][1] != cTriPx[1][1])
			{
				x_delta = (cTriPx[1][0] - cTriPx[0][0]);
				y_delta = (cTriPx[1][1] - cTriPx[0][1]);
				left_delta = x_delta / y_delta;
				x_delta = (cTriPx[2][0] - cTriPx[0][0]);
				y_delta = (cTriPx[2][1] - cTriPx[0][1]);
				right_delta = x_delta / y_delta;

				if (left_delta > right_delta)
				{
					ftemp = left_delta;
					left_delta = right_delta;
					right_delta = ftemp;
				}

				left_x = cTriPx[0][0];
				right_x = cTriPx[0][0];

				middle_line_drawn = true;

				for (y = cTriPx[0][1]; y < (cTriPx[1][1] + 1); y++)
				{
					lX = left_x + 0.5;
					rX = right_x + 0.5;
					left_index = y * frameBuf->w + lX;
					right_index = y * frameBuf->w + rX;

					// Temporary fix, the y should just be clipped and left_x/right_x adjusted accordingly
					if (true)

					{
						// Check if x coordinate is off screen and clip it.
						if (lX < 0)
						{
							lX = 0;
						}
						if (lX > frameBuf->w)
						{
							lX = frameBuf->w;
						}
						if (rX > frameBuf->w)
						{
							rX = frameBuf->w;
						}
						if (rX < 0)
						{
							rX = 0;
						}

						// For every pixel along the line, set it's color
						for (int32_t i = lX; i < rX; i++)
						{
							// Get barycentric coordinates (u, v, w)
							p[0] = i;
							p[1] = y;
							sglVec2Barycentric(p, a, b, c, &bc[VI[0]], &bc[VI[1]], &bc[VI[2]]);

							z = (bc[VI[0]] * cTri[VI[0]][2]) + (bc[VI[1]] * cTri[VI[1]][2]) + (bc[VI[2]] * cTri[VI[2]][2]);

							if (z <= depthBuf->depthBuf[y * frameBuf->w + i])
							{
								// Interpolate all vOut values to fIn values for the fragment shader
								for (int32_t j = 0; j < attrBufSize; j++)
								{
									fIn[j] = (vOut[j] * bc[0]) + (vOut[j + attrBufSize] * bc[1]) + (vOut[j + (2 * attrBufSize)] * bc[2]);
								}

								depthBuf->depthBuf[y * frameBuf->w + i] = z;
								shd->fragShd(&fragColor, fIn, uniforms);
								left_index = y * frameBuf->w + i;
								frameBuf->rawBuf[left_index] = fragColor;
							}
						}
					}
					left_x += left_delta;
					right_x += right_delta;
				}
			}

			// Rasterize Top Half (Tex coordinates, not NDC)
			if (cTriPx[1][1] != cTriPx[2][1])
			{
				x_delta = -(cTriPx[1][0] - cTriPx[2][0]);
				y_delta = (cTriPx[1][1] - cTriPx[2][1]);
				left_delta = x_delta / y_delta;
				x_delta = -(cTriPx[0][0] - cTriPx[2][0]);
				y_delta = (cTriPx[0][1] - cTriPx[2][1]);
				right_delta = x_delta / y_delta;

				if (left_delta > right_delta)
				{
					ftemp = left_delta;
					left_delta = right_delta;
					right_delta = ftemp;
				}

				left_x = cTriPx[2][0];
				right_x = cTriPx[2][0];

				if (middle_line_drawn == true)
				{
					cTriPx[1][1] += 1;
				}

				a[0] = cTriPx[0][0];
				a[1] = cTriPx[0][1];
				b[0] = cTriPx[1][0];
				b[1] = cTriPx[1][1];
				c[0] = cTriPx[2][0];
				c[1] = cTriPx[2][1];


				for (y = cTriPx[2][1]; y > (cTriPx[1][1] - 1); y--)
				{
					lX = left_x + 0.5;
					rX = right_x + 0.5;
					left_index = y * frameBuf->w + lX;
					right_index = y * frameBuf->w + rX;

					// Temporary fix, the y should just be clipped and left_x/right_x adjusted accordingly
					if (true)
					{
						if (lX < 0)
						{
							lX = 0;
						}
						if (lX > frameBuf->w)
						{
							lX = frameBuf->w;
						}
						if (rX > frameBuf->w)
						{
							rX = frameBuf->w;
						}
						if (rX < 0)
						{
							rX = 0;
						}

						// For every pixel along the line, set it's color
						for (int32_t i = lX; i < rX; i++)
						{
							// Get barycentric coordinates (u, v, w)
							p[0] = i;
							p[1] = y;
							sglVec2Barycentric(p, a, b, c, &bc[VI[0]], &bc[VI[1]], &bc[VI[2]]);

							z = (bc[VI[0]] * cTri[VI[0]][2]) + (bc[VI[1]] * cTri[VI[1]][2]) + (bc[VI[2]] * cTri[VI[2]][2]);

							if (z <= depthBuf->depthBuf[y * frameBuf->w + i])
							{
								// Interpolate all vOut values to fIn values for the fragment shader
								for (int32_t j = 0; j < attrBufSize; j++)
								{
									fIn[j] = (vOut[j] * bc[0]) + (vOut[j + attrBufSize] * bc[1]) + (vOut[j + (2 * attrBufSize)] * bc[2]);
								}

								depthBuf->depthBuf[y * frameBuf->w + i] = z;
								shd->fragShd(&fragColor, fIn, uniforms);
								left_index = y * frameBuf->w + i;
								frameBuf->rawBuf[left_index] = fragColor;
							}
						}
					}
					left_x += left_delta;
					right_x += right_delta;
				}
			}
		}

	}

	free(vOut);
	free(fIn);
}