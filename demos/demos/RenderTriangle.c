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

const uint32_t width = 1280;
const uint32_t height = 720;

/**
	@brief The vertex shader serves the purpose of returning
	a triangle in NDC (normalized device coordinates), as well as
	returning a buffer containing other vertex attributes such as
	position/normals/UVs. For this demo, it is hardcoded to return a
	set triangle, regardless of model input to the shader.

	@param[in,out] tri			Returned triangle (in NDC).

	@param[in,out] vertOutBuf	Vertex attributes to be interpolated
								for the frag shader. For this demo, we are
								outputing a float[3][3] array, representing
								RGB values at the 3 vertices of the triangle.

	@param[in] mdlPtr			The pointer to the model to calculate
								the output triangle from. There is no
								predefined model format, the user can
								give the renderer any model they want
								and interpret it in this function. In this
								demo, there is no model as the shader is
								hardcoded to return the same triangle
								every time it is called.

	@param[in] uniforms			Unchanging variables throughout the render
								(ie: perspective/lighting/textures). This
								is not used in this demo.

	@param[in] iTri				Index of triangle to calculate. This is
								ignored as this shader only returns
								a single hardcoded triangle very time.

	@param[in] nTris			Total triangles being rendered. This
								function only calculates one triangle
								but this is given if accessing a triangle
								in your model depends on this value.
*/
void vertexShader(SGLTriVec4 tri, float* vertOutBuf, void* mdlPtr,
	void* uniforms, uint32_t iTri, uint32_t nTris)
{
	// Return coordinates in screenspace of triangle
	tri[0][0] = -0.5f;		// Point 0
	tri[0][1] = -0.5f;
	tri[0][2] = 0.0f;
	tri[0][3] = 1.0f;		// w coordinate

	tri[1][0] = 0.5f;		// Point 1
	tri[1][1] = -0.5f;
	tri[1][2] = 0.5f;
	tri[1][3] = 1.0f;

	tri[2][0] = 0.0f;		// Point 2
	tri[2][1] = 0.5f;
	tri[2][2] = 1.0f;
	tri[2][3] = 1.0f;

	// Return attributes of the triangle (color at each vertex)
	vertOutBuf[0] = 1.0f;	// Point 0 : Blue
	vertOutBuf[1] = 0.0f;
	vertOutBuf[2] = 0.0f;

	vertOutBuf[3] = 0.0f;	// Point 1 : Green
	vertOutBuf[4] = 1.0f;
	vertOutBuf[5] = 0.0f;

	vertOutBuf[6] = 0.0f;	// Point 2 : Red
	vertOutBuf[7] = 0.0f;
	vertOutBuf[8] = 1.0f;
}

/**
	@brief The fragment shader serves the purpose of calculating
	the color for the current fragment (pixel) using the passed-in
	interpolated vertex attributes, as well as anything else given in
	the uniform buffer.

	@param[in] vertBuf		Interpolated vertex attributes at that frag,
							in this case it is the float[3] representing
							RGB color that we generated for each vertex
							in the vertex shader.

	@param[in] uniforms		Unchanging variables throughout the render,
							such as transformation matrices or textures.
							In this demo uniforms aren't passed to the
							renderer, so this can be ignored.

	@param[in,out] color	Color calculated for this fragment.
*/
void fragmentShader(SGLBGR32* color, float* fragInBuf, void* uniforms)
{
	// Convert color attribute from float[3] to GLBGR32. This demo used
	// [0, 1.0f] for each color, so it just needs to be scaled to [0,255]
	color->b = (uint8_t)(fragInBuf[0] * 255.0f);
	color->g = (uint8_t)(fragInBuf[1] * 255.0f);
	color->r = (uint8_t)(fragInBuf[2] * 255.0f);
}

int main()
{
	// Create framebuffer and depthbuffer textures to render to
	SGLTex2D framebuf, depthbuf;
	sglTex2DInit(&framebuf, SGL_TEX2D_RAW, width, height, 0, 0);
	sglTex2DColorFill(&framebuf, (SGLBGR32){0, 0, 0, 255});
	sglTex2DInit(&depthbuf, SGL_TEX2D_DEPTH, width, height, 0, 0);
	sglTex2DDepthReset(&depthbuf);

	// Configure shader	
	SGLShader gradientShader;
	uint32_t vertAttributes[1] = { SGL_SHD_VEC3 };	/* Array of attribute types
													at each vertex. For this
													demo, only a single SGLVec3
													containing the color at
													each vertex is used. */
	gradientShader.vertValues = &vertAttributes;
	gradientShader.nVertValues = 1;					/* Number of attributes per
													vertex, in this case it is
													only a single SGLVec3 color
													attribute. */
	gradientShader.vertShd = &vertexShader;
	gradientShader.fragShd = &fragmentShader;

	/* Render to framebuffer/depthbuffer. In this demo the shader always outputs
	the same triangle so no model or uniforms are necessary. */
	sglRenderTris(&gradientShader, &framebuf, &depthbuf, NULL, NULL, 1);

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