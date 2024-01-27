#include "cv.hh"

#include "core.hh"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initFrame(Image* img)
{
	img->data = (unsigned char*)malloc(img->width*img->height*sizeof(char)*3);

	if (img->width == 0)
		img->width = 1920;

	if (img->height == 0)
		img->height = 1080;
}

void initFrame(Image* img, u32 width, u32 height)
{
	img->width = width;
	img->height = height;

	img->data = (unsigned char*)malloc(img->width*img->height*sizeof(char)*3);
}

void zeroFrame(Image* img)
{
	if (img->width == 0)
		img->width = 1920;

	if (img->height == 0)
		img->height = 1080;

	memset(img->data, 0, img->width*img->height*sizeof(char)*3);
}

void copyFrame(Image* src, Image* dst, bool allocate)
{
	dst->width = src->width;
	dst->height = src->height;

	if (allocate)
	{dst->data = (unsigned char*)malloc(src->width*src->height*sizeof(char)*3);}

	memcpy(dst->data, src->data, src->width*src->height*sizeof(char)*3);
}

void copyFrame(unsigned char* src, Image* dst, bool allocate)
{
	dst->width = 1920;
	dst->height = 1080;

	if (allocate)
	{dst->data = (unsigned char*)malloc(dst->width*dst->height*sizeof(char)*3);}

	memcpy(dst->data, src, dst->width*dst->height*sizeof(char)*3);
}

void freeFrame(Image* img)
{
	free(img->data);
}
