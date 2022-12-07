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
#include <memory.h>

const uint32_t width = 1280;
const uint32_t height = 720;

typedef struct Vertex
{
	SGLFloat4 position;
	SGLFloat4 color;
} Vertex;

const float mdl[96] = {
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

void vertexShader(float* vertAttrBuf, const uint32_t vertSize, const void* mdl,
	const void* uniforms, const uint32_t tri)
{
	uint32_t triSize = vertSize * 3;
	float* model = mdl;
	memcpy(vertAttrBuf, &model[triSize * tri], triSize * sizeof(float));
}

void fragmentShader(SGLBGR32* color, float* fragInBuf, void* uniforms)
{
	color->b = (uint8_t)(fragInBuf[0] * 255.0f);
	color->g = (uint8_t)(fragInBuf[1] * 255.0f);
	color->r = (uint8_t)(fragInBuf[2] * 255.0f);
}

int main()
{
	SGLTex2D framebuf, depthbuf;
	sglTex2DInit(&framebuf, SGL_TEX2D_RAW, width, height, 0, 0);
	sglTex2DColorFill(&framebuf, (SGLBGR32){0, 0, 0, 255});
	sglTex2DInit(&depthbuf, SGL_TEX2D_DEPTH, width, height, 0, 0);
	sglTex2DDepthReset(&depthbuf);

	SGLShader gradientShader;
	sglCreateShader(&gradientShader, &vertexShader, &fragmentShader, 4);

	SGLRenderInfo info;
	info.shd = &gradientShader;
	info.mdl = &mdl;
	info.triStart = 0;
	info.triCount = 4;
	info.frameTarget = &framebuf;
	info.depthTarget = &depthbuf;

	float* cache = _aligned_malloc(8 * 3 * 4, 16);
	sglCacheTris(&info, cache);
	sglBinTris(&info, cache, 0, 4);


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
	SDL_Surface* framebufferSurface = SDL_CreateRGBSurfaceFrom(framebuf.rawBuf,
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

	sglTex2DFree(&framebuf);
	sglTex2DFree(&depthbuf);
	SDL_DestroyWindow(window);
	SDL_Quit();
}