#include "image.h"

void sglCreateImage(SglType type, uint32_t w, uint32_t h, uint32_t mipCount,
	SglImage* pImage)
{
	pImage->w = w;
	pImage->h = h;
	pImage->type = type;

	switch (type)
	{
	case SGL_TYPE_IMAGE_BGRA:
		pImage->pBGRA = (SglBGRA*) SGL_MALLOC(sizeof(SglBGRA) * w * h);
		return;

	case SGL_TYPE_IMAGE_FLOAT:
		pImage->pFloat = (float *) SGL_MALLOC(sizeof(float) * w * h);
		return;
	}
}

void sglCreateImage(SglType type, uint32_t w, uint32_t h, uint32_t mipCount,
	SglImage* pImage)
{
	switch (pImage->type)
	{
	case SGL_TYPE_IMAGE_BGRA:
		free(pImage->pBGRA);
		break;

	case SGL_TYPE_IMAGE_FLOAT:
		free(pImage->pFloat);
		break;
	}
}