/******************************************************************************
Texture Loading Demo

This demo is an introduction to the SGLTex2D type, which is the core of the
SGL library. Every function writes to or samples from a Tex2D. Here a BMP image
of my chickens is loaded into a texture.

SDL2 is used to display the final framebuffer texture, as the SGL library does
not include any OS-specific window system capabilities.
******************************************************************************/

#include <sgl.h>
#include <stdio.h>
#include <SDL.h>

int main()
{
	SGLTex2D tex;
	sglTex2DLoadBMP(&tex, "assets/mychickens.bmp");
	
	printf("tex width:\t%d\n", tex.w);
	printf("tex height:\t%d\n", tex.h);
	if (tex.type == SGL_TEX2D_RAW)
		printf("tex is stored as an array of SGLBGR32 pixel colors!\n");
	else if (tex.type == SGL_TEX2D_DEPTH)
		printf("tex is stored as an array of float pixel depth values!\n");

	// Don't forget to sglTex2DFree(&tex) when it is no longer used. Color
	// and depth buffers are dynamically allocated and need to be freed!

	/*****************************************************************************
	End of SoftwareGL demo, using SDL2 library to display the framebuffer on PC.
	*****************************************************************************/
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("[ERROR] Failed to initialize SDL2!\n");
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("My Chickens!", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		tex.w, tex.h, 0);

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
	SDL_Surface* framebufferSurface = SDL_CreateRGBSurfaceFrom(tex.rawBuf,
		tex.w, tex.h, 32, 4 * tex.w,	// 4 * Pixels per row 
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

	sglTex2DFree(&tex);
	SDL_DestroyTexture(framebufferSurface);
	SDL_DestroyWindow(window);
	SDL_Quit();
}