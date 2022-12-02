/******************************************************************************
PBR Demo

This demo uses the rendering tools of SGL to do physically based rendering
(PBR). If you haven't already, read the RenderTriangle demo to understand
the graphics pipeline.

This demo loads an obj model and multiple texture maps. To understand the
shaders, I would recommend reading https://learnopengl.com/PBR/Lighting

SDL2 is used to display the final framebuffer texture, as the SGL library does
not include any OS-specific window system capabilities.
******************************************************************************/

#include <sgl.h>
#include <stdio.h>
#include <SDL.h>
#include <string.h>

const uint32_t width = 1280;
const uint32_t height = 720;

typedef struct Face {
	uint32_t vertIndex[3];
	uint32_t uvIndex[3];
	uint32_t normalIndex[3];
} Face;

typedef struct Mesh {
	uint32_t nFaces;
	SGLVec3* verts;
	SGLVec2* uvs;
	SGLVec3* normals;
	Face* faces;
} Mesh;

typedef struct Uniforms {
	SGLMat4* transform;
	SGLVec3* lightPos;
	SGLVec3* lightColor;
	uint32_t nLights;
	SGLVec3* cameraPos;
	SGLTex2D* albedo;
	SGLTex2D* roughness;
} Uniforms;

/* Comically inefficientand unsafe .obj mesh loading function, but it will get
the job done! Note that the .obj must be triangle only, and only include 
vertices, normals, and uvs. */
void loadObj(uint8_t *fname, Mesh *mesh)
{
	FILE* obj = fopen(fname, "r");
	uint8_t line[128];

	// Figure out how many of each element is in the file and allocate memory
	uint32_t nVerts, nUVs, nNormals, nFaces;
	nVerts = nUVs = nNormals = nFaces = 0;
	while (fgets(line, 128, obj) != NULL)
	{
		// It should be possible to just do a switch((uint16_t) line[0]) here
		if (line[0] == 'v' && line[1] == ' ')
			nVerts++;
		else if (line[0] == 'v' && line[1] == 't')
			nUVs++;
		else if (line[0] == 'v' && line[1] == 'n')
			nNormals++;
		else if (line[0] == 'f' && line[1] == ' ')
			nFaces++;
	}
	mesh->verts = (SGLVec3 *) malloc(sizeof(SGLVec3) * nVerts);
	mesh->uvs = (SGLVec2 *) malloc(sizeof(SGLVec2) * nUVs);
	mesh->normals = (SGLVec3 *) malloc(sizeof(SGLVec3) * nNormals);
	mesh->faces = (Face *) malloc(sizeof(Face) * nFaces);
	mesh->nFaces = nFaces;

	// Now we read it all again
	fseek(obj, 0, SEEK_SET);
	uint32_t vert, uv, normal, face;
	vert = uv = normal = face = 0;
	while (fgets(line, 128, obj) != NULL)
	{
		if (line[0] == 'v' && line[1] == ' ')
		{
			sscanf(line, "v %f %f %f\n",
				&mesh->verts[vert][0], &mesh->verts[vert][1],
				&mesh->verts[vert][2]);
			vert++;
		}
		else if (line[0] == 'v' && line[1] == 't')
		{
			sscanf(line, "vt %f %f\n",
				&mesh->uvs[uv][0], &mesh->uvs[uv][1]);
			uv++;
		}
		else if (line[0] == 'v' && line[1] == 'n')
		{
			sscanf(line, "vn %f %f %f\n",
				&mesh->normals[normal][0], &mesh->normals[normal][1],
				&mesh->normals[normal][2]);
			normal++;
		}
		else if (line[0] == 'f' && line[1] == ' ')
		{
			sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
				&mesh->faces[face].vertIndex[0], &mesh->faces[face].uvIndex[0],
				&mesh->faces[face].normalIndex[0],
				&mesh->faces[face].vertIndex[1], &mesh->faces[face].uvIndex[1],
				&mesh->faces[face].normalIndex[1], 
				&mesh->faces[face].vertIndex[2], &mesh->faces[face].uvIndex[2],
				&mesh->faces[face].normalIndex[2]);
			face++;
		}
	}
	fclose(obj);
}

void vertexShader(SGLTriVec4 tri, float* vertOutBuf, void* mdlPtr,
	void* uniforms, uint32_t iTri, uint32_t nTris)
{
	// Reinterpret passed-in general pointers
	Uniforms *u = uniforms;
	Mesh* mesh = mdlPtr;

	// Transform model vertices with matrix multiplication before returning them
	for (uint32_t i = 0; i < 3; i++)
	{
		// Create a vec4 from the position with a w value of 1.0f
		SGLVec4 pos;
		pos[0] = mesh->verts[mesh->faces[iTri].vertIndex[i] - 1][0];
		pos[1] = mesh->verts[mesh->faces[iTri].vertIndex[i] - 1][1];
		pos[2] = mesh->verts[mesh->faces[iTri].vertIndex[i] - 1][2];
		pos[3] = 1.0f;

		/* Save worldspace vertex position to output buffer for per-fragment
		interpolation. */ 
		sglVec3Copy(pos, (SGLVec3*)&vertOutBuf[14 * i]);

		/* Transform vertex to clip coordinates using transformation matrix. */
		sglMat4Vec4Mul(u->transform, pos, pos);
		tri[i][0] = pos[0];
		tri[i][1] = pos[1];
		tri[i][2] = pos[2];
		tri[i][3] = pos[3];

		vertOutBuf[(14 * i) + 12] = mesh->uvs[mesh->faces[iTri].uvIndex[i] - 1][0];
		vertOutBuf[(14 * i) + 13] = mesh->uvs[mesh->faces[iTri].uvIndex[i] - 1][1];

		vertOutBuf[(14 * i) + 3] = mesh->normals[mesh->faces[iTri].normalIndex[i] - 1][0];
		vertOutBuf[(14 * i) + 4] = mesh->normals[mesh->faces[iTri].normalIndex[i] - 1][1];
		vertOutBuf[(14 * i) + 5] = mesh->normals[mesh->faces[iTri].normalIndex[i] - 1][2];
	}
}

SGL_INLINE float distributionGGX(SGLVec3 N, SGLVec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(sglVec3Dot(N, H), 0.0f);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom = M_PI * denom * denom;

	return num / denom;
}

SGL_INLINE float geometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

SGL_INLINE float geometrySmith(SGLVec3 N, SGLVec3 V, SGLVec3 L,
	float roughness)
{
	float NdotV = max(sglVec3Dot(N, V), 0.0);
	float NdotL = max(sglVec3Dot(N, L), 0.0);
	float ggx2 = geometrySchlickGGX(NdotV, roughness);
	float ggx1 = geometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

SGL_INLINE void fresnelSchlick(float cosTheta, SGLVec3 F0, SGLVec3 dest)
{
	SGLVec3 temp;
	sglFloatVec3Sub(1.0f, F0, temp);
	sglVec3FloatMul(temp, pow(sglFloatClamp(1.0 - cosTheta, 0.0, 1.0), 5.0), temp);
	sglVec3Add(F0, temp, dest);
}

/* Physically based rendering (PBR) shader. The lack of shader compilation
makes this a difficult read as the interpolated values have to be passed
in as a float array. */
void fragmentShader(SGLBGR32* color, float* fragInBuf, void* uniforms)
{
	Uniforms* u = uniforms;
	SGLVec4 albedoCol, roughnessCol;
	sglTex2DSample(u->albedo, albedoCol, fragInBuf[12], fragInBuf[13]);
	sglTex2DSample(u->roughness, roughnessCol, fragInBuf[12], fragInBuf[13]);

	// Material parameters
	SGLVec3 albedo = {
		pow(albedoCol[0], 2.2f),
		pow(albedoCol[1], 2.2f),
		pow(albedoCol[2], 2.2f)
	};
	float metallic = 1.0f;
	float roughness = roughnessCol[0];
	float ao = 1.0f;

	// World position
	SGLVec3 worldPos = {fragInBuf[0], fragInBuf[1], fragInBuf[2]};

	// Normal
	SGLVec3 N = { fragInBuf[3], fragInBuf[4], fragInBuf[5] };
	sglVec3Normalize(N);

	// Camera vector
	SGLVec3 V; sglVec3Sub(u->cameraPos, worldPos, V);

	SGLVec3 F0 = { 0.04f, 0.04f, 0.04f };
	sglVec3Mix(F0, albedo, metallic, F0);

	// Reflectance equation
	SGLVec3 Lo = { 0.0f, 0.0f, 0.0f };
	for (uint32_t l = 0; l < u->nLights; l++)
	{
		// Calculate per-light radiance
		SGLVec3 fragToLight;
		sglVec3Sub(u->lightPos[l], worldPos, fragToLight);
		
		SGLVec3 L;  sglVec3Copy(fragToLight, L);
		sglVec3Normalize(L);

		SGLVec3 H;  sglVec3Add(V, L, H);
		sglVec3Normalize(H);

		float distance = sglVec3Length(fragToLight);
		float attenuation = 1.0 / (distance * distance);
		SGLVec3 radiance;
		sglVec3FloatMul(u->lightColor[l], attenuation, radiance);

		// Cook-torrance BRDF
		float NDF = distributionGGX(N, H, roughness);
		float G = geometrySmith(N, V, L, roughness);
		SGLVec3 F;
		fresnelSchlick(max(sglVec3Dot(H, V), 0.0), F0, F);

		SGLVec3 kS; sglVec3Copy(F, kS);
		SGLVec3 kD; sglFloatVec3Sub(1.0f, kS, kD);
		sglVec3FloatMul(kD, 1.0f - metallic, kD);

		SGLVec3 numerator; sglVec3FloatMul(F, NDF * G, numerator);
		float denominator = 4.0 * max(sglVec3Dot(N, V), 0.0f) * 
			max(sglVec3Dot(N, L), 0.0f) + 0.0001f;
		SGLVec3 specular;
		sglVec3FloatDiv(numerator, denominator, specular);

		// add to outgoing radiance Lo
		float NdotL = max(sglVec3Dot(N, L), 0.0);
		SGLVec3 temp = {
			(kD[0] * albedo[0] / M_PI + specular[0]) * radiance[0] * NdotL,
			(kD[1] * albedo[1] / M_PI + specular[1]) * radiance[1] * NdotL,
			(kD[2] * albedo[2] / M_PI + specular[2]) * radiance[2] * NdotL
		};
		sglVec3Add(Lo, temp, Lo);
	}

	SGLVec3 ambient = { 0.03f, 0.03f, 0.03f };
	sglVec3Mul(ambient, albedo, ambient);
	sglVec3FloatMul(ambient, ao, ambient);
	SGLVec3 col; sglVec3Add(ambient, Lo, col);

	SGLVec3 adjustedColor = {
		col[0] / (col[0] + 1.0f),
		col[1] / (col[1] + 1.0f),
		col[2] / (col[2] + 1.0f)
	};
	SGLVec4 adjustedColor2 = {
		pow(adjustedColor[0], 1.0f / 2.2f),
		pow(adjustedColor[1], 1.0f / 2.2f),
		pow(adjustedColor[2], 1.0f / 2.2f),
		1.0f
	};

	sglVec4ToBGR32(adjustedColor2, color);
}

int main()
{
	Mesh cube;
	loadObj("assets/sphere.obj", &cube);

	// Create framebuffer and depthbuffer textures
	SGLTex2D framebuf, depthbuf;
	sglTex2DInit(&framebuf, SGL_TEX2D_RAW, width, height, 0, 0);
	sglTex2DColorFill(&framebuf, (SGLBGR32){0, 0, 0, 255});
	sglTex2DInit(&depthbuf, SGL_TEX2D_DEPTH, width, height, 0, 0);
	sglTex2DDepthReset(&depthbuf);

	/* Configure transformation matrix.This matrix will be multiplied by every
	vertex in the model to convert from model coordinates to normalized device
	coordinates. */ 
	SGLMat4 view, projection, transform;
	SGLVec3 cameraPos = { 0.0f, 3.0f, 1.0f };
	sglMat4LookAt(cameraPos,
		(SGLVec3) { 0.0f, 0.0f, 0.0f },
		(SGLVec3) {0.0f, 0.0f, 1.0f}, view);
	sglMat4Perspective(42.0f * (M_PI / 180.0f),
		16.0f / 9.0f, 0.1f, 10.0f, projection);
	sglMat4Mat4Mul(projection, view, transform);

	/* Generate uniforms, which are user defined values that will be
	given to the shader. A Uniforms type was defined at the beginning
	of this demo to make it easier to keep track of these values. */
	SGLVec3 lightPos[2] = { {-3.0f, 3.0f, 5.0f}, {3.0f, 3.0f, -2.0f} };
	SGLVec3 lightColor[2] = {{30.0f, 30.0f, 100.0f}, {100.0f, 100.0f, 80.0f}};
	SGLTex2D albedo, roughness;
	sglTex2DLoadBMP(&albedo, "assets/BrushedStainlessSteel_Albedo.bmp");
	sglTex2DLoadBMP(&roughness, "assets/BrushedStainlessSteel_Roughness.bmp");
	Uniforms uniforms = {
		transform,							// Transform matrix
		lightPos, lightColor, 2, cameraPos,	// Light positions, colors, and number
		&albedo, &roughness					// Textures
	};	
	
	// Configure shader
	uint32_t vertAttributes[5] = {
		SGL_SHD_VEC3,	// World position
		SGL_SHD_VEC3,	// Normal
		SGL_SHD_VEC3,	// Binormal		// Not generated in this demo
		SGL_SHD_VEC3,	// Tangent		// Not generated in this demo
		SGL_SHD_VEC2	// UV coordinate
	
	};	// Only color attribute per vertex
	SGLShader shader = {
		5, vertAttributes,					/* Number of attributes per vertex,
											list of attribute type list (see
											macros in render.h). */ 
		&vertexShader, &fragmentShader		/* Vertex and fragment shader
											function pointers. */
	};

	// Render
	sglRenderTris(&shader, &framebuf, &depthbuf, &cube, &uniforms, cube.nFaces);

	/*****************************************************************************
	End of SoftwareGL demo, using SDL2 library to display the framebuffer on PC.
	*****************************************************************************/
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("[ERROR] Failed to initialize SDL2!\n");
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("PBR Render", SDL_WINDOWPOS_CENTERED,
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

	sglTex2DFree(&albedo);
	sglTex2DFree(&roughness);
	sglTex2DFree(&framebuf);
	sglTex2DFree(&depthbuf);
	SDL_DestroyWindow(window);
	SDL_Quit();
}