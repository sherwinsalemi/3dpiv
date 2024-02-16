#include "cv.hh"

#include "core.hh"
#include <cstdlib>
#include <stdio.h>
#include <math.h>

#include "settings.hh"

Image last;
Image derivative;

Image persistentOut;

void initPipeline()
{
	last.width = WIDTH;
	last.height = HEIGHT;
	initFrame(&last);
	zeroFrame(&last);
	derivative.width = WIDTH;
	derivative.height = HEIGHT;
	initFrame(&derivative);
}

void processFrame(Image in, Image* out)
{

	zeroFrame(&derivative);
	copyFrame(&in, out, false);



	copyFrame(&in, &last, false);
}

void freePipeline()
{
	freeFrame(&last);
	freeFrame(&derivative);
}
