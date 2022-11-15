#include <stdio.h>
#include "texture.h"

void sglTex2DInit(SGLTex2D * tex, int32_t type, int32_t w, int32_t h,
	int32_t l, int32_t t)
{
	tex->w = w;
	tex->h = h;
	tex->l = l;
	tex->t = t;
	tex->type = type;

	switch (type)
	{
	case SGL_TEX2D_RAW:
		tex->rawBuf = (SGLBGR32 *) malloc(sizeof(SGLBGR32) * tex->w * tex->h);
		break;

	case SGL_TEX2D_DEPTH:
		tex->depthBuf = (float *) malloc(sizeof(float) * tex->w * tex->h);
		break;
	}
}

void sglTex2DFree(SGLTex2D* tex)
{
	switch (tex->type)
	{
	case SGL_TEX2D_RAW:
		free(tex->rawBuf);
		break;

	case SGL_TEX2D_DEPTH:
		free(tex->depthBuf);
		break;
	}
}

void sglTex2DColorFill(SGLTex2D* tex, SGLBGR32 color)
{
	for (uint32_t i = 0; i < tex->w * tex->h; i++)
		tex->rawBuf[i] = color;
}

void sglTex2DDepthReset(SGLTex2D* tex)
{
	for (uint32_t i = 0; i < tex->w * tex->h; i++)
		tex->depthBuf[i] = 1.0f;
}

void sglTex2DLoadBMP(SGLTex2D* tex, uint8_t* fname)
{
	FILE* bmp = fopen(fname, "rb");

	// Map locations in buffer;
	SGLBMPHeader info;
	fread(&info, sizeof(SGLBMPHeader), 1, bmp);

	if (info.szVersion[0] != 'B' && info.szVersion[1] != 'M')
	{
		printf("[ERROR] %s IS NOT A VALID BMP FILE!\n", fname);
		return;
	}

	if (info.nPxBits != 24 || info.nCompression != 0)
	{
		printf("[ERROR] %s IS NOT A 24-BIT UNCOMPRESSED BMP!\n", fname);
		return;
	}

	// Each pixel is 3 bytes in 24-bit BMP, however CTex2D uses GLBGR24
	// which contains a final padding bit (32-bits per pixel). The rows
	// also increment in the opposite direction.
	fseek(bmp, info.nOffset, SEEK_SET);
	SGLBGR24* rawImg = malloc(sizeof(SGLBGR24) * info.nWidth * info.nHeight);
	fread(rawImg, sizeof(SGLBGR24) * info.nWidth * info.nHeight, 1, bmp);

	// Initialize with RAW format for 24-bit non-compressed color
	sglTex2DInit(tex, SGL_TEX2D_RAW, info.nWidth, info.nHeight, 0, 0);

	for (int32_t yPx = 0; yPx < info.nHeight; yPx++)
	{
		for (int32_t xPx = 0; xPx < info.nWidth; xPx++)
		{
			int32_t texPx = xPx + info.nWidth * yPx;
			int32_t bmpPx = xPx + info.nWidth * (info.nHeight - yPx - 1);

			tex->rawBuf[texPx].b = rawImg[bmpPx].b;
			tex->rawBuf[texPx].g = rawImg[bmpPx].g;
			tex->rawBuf[texPx].r = rawImg[bmpPx].r;
			tex->rawBuf[texPx].a = 255;
		}
	}

	free(rawImg);
	fclose(bmp);
}

void sglTex2DSaveBMP(SGLTex2D* tex, uint8_t* fname)
{
	int32_t w = tex->w;
	int32_t h = tex->h;
	size_t bmpSize = sizeof(SGLBMPHeader) + w * h * 3;		// 24 bit color

	FILE* bmp = fopen(fname, "wb");
	
	// Map locations in buffer;
	SGLBMPHeader info;
	info.szVersion[0] = 'B';
	info.szVersion[1] = 'M';
	info.nSize = bmpSize;
	info.nOffset = sizeof(SGLBMPHeader);
	info.nInfoSize = 40;
	info.nWidth = w;
	info.nHeight = h;
	info.nPlanes = 1;
	info.nPxBits = 24;
	info.nCompression = 0;
	info.nImageSize = 0;
	info.nXPxPerM = 3780;
	info.nYPxPerM = 3780;
	info.nColors = 0;
	info.nImptColors = 0;

	fwrite(&info, sizeof(SGLBMPHeader), 1, bmp);

	// Each pixel is 3 bytes in 24-bit BMP, however CTex2D uses CBGR24
	// which contains a final padding bit (32-bits per pixel). The rows
	// also increment in the opposite direction.
	SGLBGR24 *bmpBuf = malloc(sizeof(SGLBGR24) * w * h);

	for (int32_t yPx = 0; yPx < h; yPx++)
	{
		for (int32_t xPx = 0; xPx < w; xPx++)
		{
			int32_t bmpBufOffset = xPx + w * yPx;
			int32_t texBufIndex = xPx + w * (h - yPx - 1);

			bmpBuf[bmpBufOffset].r = tex->rawBuf[texBufIndex].r;
			bmpBuf[bmpBufOffset].g = tex->rawBuf[texBufIndex].g;
			bmpBuf[bmpBufOffset].b = tex->rawBuf[texBufIndex].b;
		}
	}

	fwrite(bmpBuf, sizeof(SGLBGR24) * w * h, 1, bmp);
	free(bmpBuf);
	fclose(bmp);
}
