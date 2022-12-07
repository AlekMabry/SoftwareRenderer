#pragma once

#include "common.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#pragma pack(1)

/** BMP file header. */
typedef struct SGLBMPHeader
{
	uint8_t version[2];		/** Should have 'BM' signature */
	uint32_t size;			/** File size in bytes. */
	uint32_t pad;
	uint32_t offset;		/** Offset from beginning of file to data. */

	/* Info header is right after first header, so it is merged here. */
	uint32_t infoSize;		/** Size of this info part of the header (=40). */
	uint32_t w;				/** Width */
	uint32_t h;				/** Height */
	uint16_t planes;		/** Number of planes (=1) */
	uint16_t pxBits;		/** Number of bits per pixel.This also indicates
								palette method:
									- 1-bit: Monochrome
									- 4-bit: 16-color palette
									- 8-bit: 256-color palette
									- 16-bit: 65565 colors
									- 24-bit: 16M colors */
	uint32_t compression;	/** Type of compression:
									- 0: BI_BGR	: no-compression
									- 1: BI_RLE8 : 8-bit RLE encoding
									- 2: BI_RLE4 : 4-bit RLE encoding */
	uint32_t imgSize;		/** Size of image if compressed
								(=0 if not compressed). */
	uint32_t xPxPerM;		/** Horizontal resolution, pixels per meter. */
	uint32_t yPxPerM;		/** Vertical resolution, pixels per meter. */
	uint32_t colors;		/** Number of used colors
								(ie. 256 for 8-bit color). */
	uint32_t importantColors;	/**	Number of important colors
									(= 0 for all). */
} SGLBMPHeader;

#pragma pack(pop)

/** Loads a BMP image into a texture. Supports standard uncompressed 24-bit
	color BMP images. */
SGL_EXPORT void sglTex2DLoadBMP(const uint8_t* pFname, SGLTex2D* pTex);

/** Saves a texture to a BMP image. Creates standard uncompressed 24-bit
	color BMP images. */
SGL_EXPORT void sglTex2DSaveBMP(const SGLTex2D* pTex, uint8_t* pFname);