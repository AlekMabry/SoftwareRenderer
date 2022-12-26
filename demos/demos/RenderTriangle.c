/******************************************************************************
Triangle Rendering Demo

This demo uses the rendering tools of SGL to render a multicolor triangle. SGL
features a programmable pipeline through the use of shaders. A custom vertex
shader is created to return a hardcoded triangle, and a custom fragment shader
is used to interpolate the vertex colors at each pixel of the triangle.

SDL2 is used to display the final framebuffer texture, as the SGL library does
not include any OS-specific window system capabilities.
******************************************************************************/

#include <sgl.h>
#include <stdio.h>
#include <SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>

const uint32_t width = 1280;
const uint32_t height = 720;

typedef struct Vertex
{
	SGLFloat4 position;
	SGLFloat4 color;
} Vertex;

const float mesh[96] = {
	-0.8f, -0.8f, 0.0f, 1.0f,	1.0f, 0.0f, 0.0f, 1.0f,
	-0.3f, -0.2f, 0.0f, 1.0f,	1.0f, 0.0f, 0.0f, 1.0f,
	-0.7f, -0.5f, 0.0f, 1.0f,	1.0f, 0.0f, 0.0f, 1.0f,

	0.1f, -0.1f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f, 1.0f,
	0.3f, -0.4f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, -0.3f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f,

	-0.2f, 0.1f, 0.0f, 1.0f,	1.0f, 1.0f, 0.0f, 1.0f,
	-0.4f, 0.7f, 0.0f, 1.0f,	1.0f, 1.0f, 0.0f, 1.0f,
	-0.8f, 0.3f, 0.0f, 1.0f,	1.0f, 1.0f, 0.0f, 1.0f,
};

void vertShd(const uint32_t vertSize, const void* pMesh,
	const void* pUniforms, const uint32_t tri, float* pVertAttrBuf)
{
	// Passthrough shader
	uint32_t triSize = vertSize * 3;
	memcpy(pVertAttrBuf, &((float*)pMesh)[triSize * tri],
		triSize * sizeof(float));
}

void fragShd(const float* pFragAttrBuf, const void* pUniforms,
	SGLBGRA* pColor)
{
	pColor->b = (uint8_t)(pFragAttrBuf[0] * 255.0f);
	pColor->g = (uint8_t)(pFragAttrBuf[1] * 255.0f);
	pColor->r = (uint8_t)(pFragAttrBuf[2] * 255.0f);
}

int main()
{
	SGLTex2D frameBuf, depthBuf;
	sglCreateTex2D(SGL_TYPE_TEX2D_BGRA, width, height, 1, 
		&frameBuf);
	sglTex2DFillBGRA((SGLBGRA){0, 0, 0, 255}, &frameBuf);
	sglCreateTex2D(SGL_TYPE_TEX2D_FLOAT, width, height, 1, 
		&depthBuf);
	sglTex2DFillFloat(SGL_DEPTH_FAR, &depthBuf);

	SGLShader shd;
	sglCreateShader(&vertShd, &fragShd, 4, &shd);

	SGLMeshInfo meshInfo = { SGL_TYPE_MESH, &shd, NULL, mesh, 0, 4 };

	SGLBin bins[120];
	for (uint32_t i = 0; i < 120; i++)
	{
		bins[i].sType = SGL_TYPE_BIN;
		bins[i].pTris = SGL_MALLOC(16);
		bins[i].tris = 0;
		bins[i].size = 16;
	}

	SGLTargetInfo targetInfo = { SGL_TYPE_TARGET_INFO, &frameBuf, &depthBuf,
		width, height, 10, 12, 128, 60, bins
	};

	float vertCache[96];
	sglCacheTris(&meshInfo, &targetInfo, vertCache);

	sglBinTris(vertCache, &meshInfo, 0, 4, &targetInfo);


	/*****************************************************************************
	End of SoftwareGL demo, using SDL2 library to display the framebuffer on PC.
	*****************************************************************************/
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("[ERROR] Failed to initialize SDL2!\n");
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("Render Triangle", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, width, height, 0);

	if (!window)
	{
		printf("[ERROR] Failed to create window!\n");
		return -1;
	}

	SDL_Surface* windowSurface = SDL_GetWindowSurface(window);

	if (!windowSurface)
	{
		printf("[ERROR] Failed to get surface from window!\n");
		return -1;
	}

	// Generate SDL2 surface from the rendered SGLTex2D framebuffer
	SDL_Surface* framebufferSurface = SDL_CreateRGBSurfaceFrom(frameBuf.pBGRA,
		width, height, 32, 4 * width,	// 4 * Pixels per row 
		0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	SDL_BlitSurface(framebufferSurface, 0, windowSurface, 0);
	SDL_UpdateWindowSurface(window);

	bool quit = false;
	SDL_Event event;
	while (!quit)
	{
		SDL_WaitEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		}
	}

	sglDestroyTex2D(&frameBuf);
	sglDestroyTex2D(&depthBuf);
	SDL_DestroyWindow(window);
	SDL_Quit();
}