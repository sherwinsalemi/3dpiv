#include "cv/cv.hh"

#include "core.hh"

#include "settings.hh"

Image last;

void InitPipeline()
{
	last.width = WIDTH;
	last.height = HEIGHT;
	InitFrame(&last);
	ZeroFrame(&last);
}

void ProcessPipeline(Image in, Image* out)
{
	for (u32 x = 0; x < WIDTH; x++)
	{
		for (u32 y = 0; y < HEIGHT; y++)
		{
			pix((*out), x, y, RED) = pix(in, x, y, RED);
		}
	}
}

void FreePipeline()
{
	FreeFrame(&last);
}
