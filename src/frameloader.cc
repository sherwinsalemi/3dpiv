#include "frameloader.hh"

#include "settings.hh"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

char fileName[32];

Image loadImage(int frameNumber)
{
	sprintf(fileName, PATH, frameNumber);
	Image i;
	i.data = stbi_load([path], &i.width, &i.height, nullptr, 3);
	return i;
}

void freeImage(Image* image)
{
	stbi_image_free(image->data);
}
