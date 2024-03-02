#include "cv.hh"

#include "core.hh"
#include <cstdlib>
#include <stdio.h>
#include <math.h>

#define STD_DEVIATION 5
#define KERNEL_RADIUS 2*STD_DEVIATION // radius doesn't include center point

float kernel[KERNEL_RADIUS * 2 + 1][KERNEL_RADIUS * 2 + 1];

float sign(float a)
{
	if (a > 0.0f)
	{
		return 1.0f;
	}
	if (a < 0.0f)
	{
		return -1.0f;
	}
	return 0.0f;
}

void GaussianKernel(float* a, float dev, int radius)
{
	float sum = 0;

	for (int x = 0; x < radius * 2 + 1; x++)
	{
		for (int y = 0; y < radius * 2 + 1; y++)
		{
			double h = x - (radius * 2) / 2.0;
            double v = y - (radius * 2) / 2.0;
            a[x * (2 * radius + 1) + y] = exp(((pow(h, 2) + pow(v, 2)) / ((2 * pow(dev, 2)))) * (-1));
            sum += a[x * (2 * radius + 1) + y];
		}
	}

	for (int x = 0; x < radius * 2 + 1; x++)
	{
		for (int y = 0; y < radius * 2 + 1; y++)
		{
			a[x * (2 * radius + 1) + y] /= sum;
			//printf("%f, ", a[x * (2 * radius + 1) + y]);
			
		}
		//printf("\n");
	}
}

int derivativef0[1080][1920];
int derivativef1[1080][1920];
int derivativef2[1080][1920];

void initPipeline()
{
	GaussianKernel(&kernel[0][0], (float)STD_DEVIATION, KERNEL_RADIUS);
}

void processFrame(Image t0f0, Image t0f1, Image t0f2, Image t1f0, Image t1f1, Image t1f2, Image* out)
{
	for (u32 y = 0; y < t0f0.height; y++)
	{
		for (u32 x = 0; x < t0f0.width; x++)
		{
			derivativef0[y][x] = (int)pix(t1f0, x, y, RED) - (int)pix(t0f0, x, y, RED);
			derivativef1[y][x] = (int)pix(t1f1, x, y, RED) - (int)pix(t0f1, x, y, RED);
			derivativef2[y][x] = (int)pix(t1f2, x, y, RED) - (int)pix(t0f2, x, y, RED);

			//pix((*out), x, y, BLU) = pix((in), x, y, RED) / 3;
		}
	}

	for (u32 y = KERNEL_RADIUS + 1; y < (t0f0.height - KERNEL_RADIUS - 1); y++)
	{
		for (u32 x = KERNEL_RADIUS + 1; x < (t0f0.width - KERNEL_RADIUS - 1); x++)
		{
			float xTotal = 0.0f;
			float yTotal = 0.0f;
			float zTotal = 0.0f;

			for (int kY = -KERNEL_RADIUS; kY <= KERNEL_RADIUS; kY++)
			{
				for (int kX = -KERNEL_RADIUS; kX <= KERNEL_RADIUS; kX++)
				{
					float kernelValue = kernel[kX + KERNEL_RADIUS][kY + KERNEL_RADIUS];
					int pixelDiff = (derivativef1[y + kY][x + kX] - derivativef1[y][x]);

					xTotal += (float)pixelDiff * kernelValue * sign(kX);// * ((float)kX / float(kX + kY));
					yTotal += (float)pixelDiff * kernelValue * sign(kY);// * ((float)kY / float(kX + kY));
				}
			}

			// i32 difL = pix(derivative, x, y, GRN) - pix(derivative, x-1, y, GRN);
			// i32 difU = pix(derivative, x, y, GRN) - pix(derivative, x, y - 1, GRN);
			// i32 difR = pix(derivative, x + 1, y, GRN) - pix(derivative, x, y, GRN);
			// i32 difD = pix(derivative, x, y + 1, GRN) - pix(derivative, x, y, GRN);

			// pix((*out), x, y, RED) = (abs(difL) + abs(difR)) * 30;
			// pix((*out), x, y, GRN) = (abs(difU) + abs(difD)) * 30;


			//printf("xTotal: %f, yTotal: %f\n", xTotal, yTotal);

			pix((*out), x, y, RED) = abs(xTotal)*10;
			pix((*out), x, y, GRN) = abs(yTotal)*10;
			pix((*out), x, y, BLU) = abs((derivativef2[y][x] - derivativef1[y][x])+(derivativef1[y][x] - derivativef0[y][x]));
		}
	}

	printf("Copy\n");


}

void freePipeline()
{
}
