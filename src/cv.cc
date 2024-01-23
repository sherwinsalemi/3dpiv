#include "cv.hh"

#include "core.hh"

#include <stdio.h>

void processFrame(Image in, Image* out)
{
	printf("Size: %d, %d\n", in.width, in.height);

	for (u32 y = 0; y < in.height; y++)
	{
		for (u32 x = 0; x < in.width; x++)
		{
			out->data[y * in.width * 3 + x * 3 + 2] = in.data[y * in.width * 3 + x * 3];
			// out->data[y * in.width * 3 + x * 3] = in.data[y * in.width * 3 + x * 3];
			// out->data[y * in.width * 3 + x * 3 + 1] = in.data[y * in.width * 3 + x * 3 + 1];
			// out->data[y * in.width * 3 + x * 3 + 2] = in.data[y * in.width * 3 + x * 3 + 2];
		}
	}
}