#pragma once

#include "texture.h"

/**
	@defgroup Render Render
	@brief For rendering triangle arrays using vertex/fragment shaders.
*/

#define SGL_SHD_FLOAT 1		/// @ingroup Render
#define SGL_SHD_VEC2 2		/// @ingroup Render
#define SGL_SHD_VEC3 3		/// @ingroup Render
#define SGL_SHD_VEC4 4		/// @ingroup Render
#define SGL_SHD_MAT4 5		/// @ingroup Render

/**
	@ingroup Render
	@brief User-programmed and passed to renderer. Calculates on-screen
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
		be drawn at that particular fragment.
*/
typedef struct SGLShader
{
	/**
		@brief The number of vertex attributes to interpolate and pass between
		the vertex and fragment shader.
		For example if you wanted to pass an interpolated position and UV
		coordinate to the fragment shader, this value would be 2.
	*/
	uint32_t nVertValues;

	/**
		@brief Array of vertex attribute types that describes the format of
		the vertex attributes. See SHD_ macros for types.
		For example if you wanted to pass an interpolated position and UV
		coordinate to the fragment shader, this array would be
		{SHD_CVEC2, SHD_CVEC3}.
	*/
	uint32_t* vertValues;

	/**
		@brief The vertex shader serves the purpose of returning
		a triangle in NDC (normalized device coordinates), as well as
		returning a buffer containing other vertex attributes such as
		position/normals/UVs.
		@param[in,out] tri			Returned triangle (in NDC).
		@param[in,out] vertOutBuf	Vertex attributes to be interpolated
									for the frag shader. These are in format:
									Vertex 0 values, Vertex 1 values, Vertex
									2 values, where values has the format
									defined by vertValues in CShader.
		@param[in] mdlPtr			The pointer to the model to calculate
									the output triangle from. There is no
									predefined model format, the user can
									give the renderer any model they want
									and interpret it in this function.
		@param[in] uniforms			Unchanging variables throughout the render
									(ie: perspective/lighting/textures).
		@param[in] iTri				Index of triangle to calculate.
		@param[in] nTris			Total triangles being rendered. This
									function only calculates one triangle
									but this is given if accessing a triangle
									in your model depends on this value.
	*/
	void(*vertShd)(SGLTri tri, float* vertOutBuf, void* mdlPtr,
		void* uniforms, uint32_t iTri, uint32_t nTris);

	/**
		@brief The fragment shader serves the purpose of calculating
		the color for the current fragment (pixel) using the passed-in
		interpolated vertex attributes, as well as anything else given in
		the uniform buffer.
		@param[in] vertBuf		Interpolated vertex attributes at that frag
								(ie: position, uv, normal).
		@param[in] uniforms		Unchanging variables throughout the render
								(ie: perspective/lighting/textures).
		@param[in,out] color	Color calculated for this fragment.
	*/
	void(*fragShd)(SGLBGR32* color, float* fragInBuf, void* uniforms);
} SGLShader;

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
void sglRenderTris(SGLShader* shd, SGLTex2D* frameBuf, SGLTex2D* depthBuf,
	void* mdl, void* uniforms, uint32_t nTris);