#include "cv/cv.hh"

#include "core.hh"
#include <math.h>
#include "settings.hh"
#include <stdio.h>

Image last;
Image derivative;

void InitPipeline()
{
	last.width = WIDTH;
	last.height = HEIGHT;
	InitFrame(&last);
	ZeroFrame(&last);

	derivative.width = WIDTH;
	derivative.height = HEIGHT;
	InitFrame(&derivative);
}

void ProcessPipeline(Image in, Image* out)
{
	printf("Process\n");
	ZeroFrame(&derivative);

	for (u32 x = 0; x < WIDTH; x++)
	{
		for (u32 y = 0; y < HEIGHT; y++)
		{
			unsigned char pixel = abs(pix(in, x, y, RED) - pix(last, x, y, RED));
			
			pix((derivative), x, y, GRN) = pixel;
		}
	}

	printf("Derivative done\n");

	for (u32 x = 3; x < WIDTH - 3; x++)
	{
		for (u32 y = 3; y < HEIGHT - 3; y++)
		{
			//printf("neighbor\n");
			i32 difL = pix(derivative, x, y, GRN) - pix(derivative, x-1, y, GRN);
			i32 difU = pix(derivative, x, y, GRN) - pix(derivative, x, y - 1, GRN);
			i32 difR = pix(derivative, x + 1, y, GRN) - pix(derivative, x, y, GRN);
			i32 difD = pix(derivative, x, y + 1, GRN) - pix(derivative, x, y, GRN);
			//printf("Sides\n");


			//printf("Write\n");
			pix((*out), x, y, RED) = (abs(difL) + abs(difR)) * 3;
			pix((*out), x, y, GRN) = (abs(difU) + abs(difD)) * 3;
		}
	}

	printf("Calc done\n");

	CopyFrame(&in, &last, false);
}

void FreePipeline()
{
	FreeFrame(&last);
	FreeFrame(&derivative);
}
