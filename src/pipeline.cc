#include "cv.hh"

#include "core.hh"
#include <cstdlib>
#include <stdio.h>
#include <math.h>

Image last;
Image derivative;

void initPipeline()
{
	last.width = 1920;
	last.height = 1080;
	initFrame(&last);

	derivative.width = 1920;
	derivative.height = 1080;
	initFrame(&derivative);

	zeroFrame(&last);
}

void processFrame(Image in, Image* out)
{
	printf("Size: %d, %d\n", in.width, in.height);

	zeroFrame(&derivative);

	//copyFrame(&in, out, false);

	for (u32 y = 0; y < in.height; y++)
	{
		for (u32 x = 0; x < in.width; x++)
		{
			unsigned char pixel = abs(pix(in, x, y, RED) - pix(last, x, y, RED));


			pix((derivative), x, y, GRN) = pixel;


			//pix((*out), x, y, BLU) = pix((in), x, y, RED) / 3;
		}
	}

	for (u32 y = 1; y < (in.height - 1); y++)
	{
		for (u32 x = 1; x < (in.width - 1); x++)
		{
			i32 difL = pix(derivative, x, y, GRN) - pix(derivative, x-1, y, GRN);
			i32 difU = pix(derivative, x, y, GRN) - pix(derivative, x, y - 1, GRN);
			i32 difR = pix(derivative, x + 1, y, GRN) - pix(derivative, x, y, GRN);
			i32 difD = pix(derivative, x, y + 1, GRN) - pix(derivative, x, y, GRN);

			pix((*out), x, y, RED) = (abs(difL) + abs(difR)) * 3;
			pix((*out), x, y, GRN) = (abs(difU) + abs(difD)) * 3;
			
		}
	}

	printf("Copy\n");


	copyFrame(&in, &last, false);

}

void freePipeline()
{
	freeFrame(&last);
	freeFrame(&derivative);
}
