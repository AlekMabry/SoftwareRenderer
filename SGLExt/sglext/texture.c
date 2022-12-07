#include "texture.h"

SGL_EXPORT void sglTex2DLoadBMP(const uint8_t* pFname, SGLTex2D* pTex)
{
	FILE* pBmp = fopen(pFname, "rb");

	/*	Map locations in file buffer. */
	SGLBMPHeader info;
	fread(&info, sizeof(SGLBMPHeader), 1, pBmp);

	if (info.version[0] != 'B' && info.version[1] != 'M')
	{
		sglValidationCallback(
			SGL_CALLBACK_SEVERITY_ERROR,
			nullptr, 0,
			"sglTex2DLoadBMP: %s is not a valid BMP.", pFname
		);
		return;
	}

	if (info.pxBits != 24 || info.compression != 0)
	{
		sglValidationCallback(
			SGL_CALLBACK_SEVERITY_ERROR,
			nullptr, 0,
			"sglTex2DLoadBMP: %s is not a 24-bit uncompressed BMP.", pFname
		);
		return;
	}

	/*	Each pixel is 3 bytes (BGR) in 24-bit BMP, however SGLTex2D uses BGRA
		which is 32-bits in size. The rows also increment in the opposite
		direction. */
	fseek(pBmp, info.offset, SEEK_SET);
	SGLBGR* pRawImg = malloc(sizeof(SGLBGR) * info.w * info.h);
	fread(pRawImg, sizeof(SGLBGR) * info.w * info.h,
		1, pBmp);

	sglCreateTex2D(SGL_TYPE_TEX2D_BGRA, info.w, info.h, 0, pTex);

	for (int32_t yPx = 0; yPx < info.h; yPx++)
	{
		for (int32_t xPx = 0; xPx < info.w; xPx++)
		{
			int32_t texPx = xPx + info.w * yPx;
			int32_t bmpPx = xPx + info.w * (info.h - yPx - 1);

			pTex->pBGRA[texPx].b = pRawImg[bmpPx].b;
			pTex->pBGRA[texPx].g = pRawImg[bmpPx].g;
			pTex->pBGRA[texPx].r = pRawImg[bmpPx].r;
			pTex->pBGRA[texPx].a = 255;
		}
	}

	free(pRawImg);
	fclose(pBmp);
}

void sglTex2DSaveBMP(const SGLTex2D* pTex, uint8_t* pFname)
{
	int32_t w = pTex->w;
	int32_t h = pTex->h;
	size_t bmpSize = sizeof(SGLBMPHeader) + w * h * 3;		// 24 bit color

	FILE* pBmp = fopen(pFname, "wb");

	// Map locations in buffer;
	SGLBMPHeader info;
	info.version[0] = 'B';
	info.version[1] = 'M';
	info.size = bmpSize;
	info.offset = sizeof(SGLBMPHeader);
	info.infoSize = 40;
	info.w = w;
	info.h = h;
	info.planes = 1;
	info.pxBits = 24;
	info.compression = 0;
	info.imgSize = 0;
	info.xPxPerM = 3780;
	info.yPxPerM = 3780;
	info.colors = 0;
	info.importantColors = 0;

	fwrite(&info, sizeof(SGLBMPHeader), 
		1, pBmp);

	// Each pixel is 3 bytes in 24-bit BMP, however CTex2D uses CBGR24
	// which contains a final padding bit (32-bits per pixel). The rows
	// also increment in the opposite direction.
	SGLBGR* pBmpBuf = malloc(sizeof(SGLBGR) * w * h);

	for (int32_t yPx = 0; yPx < h; yPx++)
	{
		for (int32_t xPx = 0; xPx < w; xPx++)
		{
			int32_t bmpBufOffset = xPx + w * yPx;
			int32_t texBufIndex = xPx + w * (h - yPx - 1);

			pBmpBuf[bmpBufOffset].r = pTex->pBGRA[texBufIndex].r;
			pBmpBuf[bmpBufOffset].g = pTex->pBGRA[texBufIndex].g;
			pBmpBuf[bmpBufOffset].b = pTex->pBGRA[texBufIndex].b;
		}
	}

	fwrite(pBmpBuf, sizeof(SGLBGR) * w * h, 
		1, pBmp);
	free(pBmpBuf);
	fclose(pBmp);
}
