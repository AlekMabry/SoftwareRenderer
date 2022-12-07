#include "texture.h"

void sglCreateTex2D(uint32_t sType, uint32_t w, uint32_t h, uint32_t mipmaps,
	SGLTex2D* pTex)
{
	pTex->w = w;
	pTex->h = h;
	pTex->sType = sType;

	switch (sType)
	{
	case SGL_TYPE_TEX2D_BGRA:
		pTex->pBGRA = (SGLBGRA*) SGL_MALLOC(sizeof(SGLBGRA) * w * h);
		return;

	case SGL_TYPE_TEX2D_FLOAT:
		pTex->pFloat = (float *) SGL_MALLOC(sizeof(float) * w * h);
		return;
	}
}

void sglDestroyTex2D(SGLTex2D* pTex)
{
	switch (pTex->sType)
	{
	case SGL_TYPE_TEX2D_BGRA:
		free(pTex->pBGRA);
		break;

	case SGL_TYPE_TEX2D_FLOAT:
		free(pTex->pFloat);
		break;
	}
}

void sglTex2DFillBGRA(SGLBGRA color, SGLTex2D* pTex)
{
	for (uint32_t i = 0; i < pTex->w * pTex->h; i++)
		pTex->pBGRA[i] = color;
}

void sglTex2DFillFloat(float scalar, SGLTex2D* pTex)
{
	for (uint32_t i = 0; i < pTex->w * pTex->h; i++)
		pTex->pFloat [i] = scalar;
}