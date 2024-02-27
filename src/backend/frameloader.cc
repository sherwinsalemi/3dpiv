#include "frameloader.hh"

#include "../settings.hh"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <io.h>
#include <stdlib.h>

char fileName[32];

Image loadImage(const char* path, int* frameNumber)
{
	sprintf(fileName, PATH, *frameNumber);

	if (_access(fileName, 04) != 0)
	{
		if (_access(fileName, 00) != 0)
		{
			fileName[31] = '\0'; // Null terminates string for printing
			printf("Failed to read frame %d, file does not exist!\nPath: %s\n", *frameNumber, fileName);
			printf("Returning to frame 1\n");
			*frameNumber = 1;
			sprintf(fileName, PATH, *frameNumber);
		}
		else {
			fileName[31] = '\0';
			printf("Failed to read frame %d, missing permissions!\nPath: %s\n", *frameNumber, fileName);
			printf("Attempting to return to frame 1\n");
			*frameNumber = 1;
			sprintf(fileName, PATH, *frameNumber);
		}
	}

	Image i;
	int w, h;
	i.data = stbi_load(fileName, &w, &h, nullptr, 3);
	i.width = (unsigned int)w;
	i.height = (unsigned int)h;
	return i;
}

void freeImage(Image* image)
{
	stbi_image_free(image->data);
}
