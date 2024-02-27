#include "cv/cv.hh"

#include "core.hh"

#include "settings.hh"

Image last;

void initPipeline()
{
	last.width = WIDTH;
	last.height = HEIGHT;
	initFrame(&last);
	zeroFrame(&last);
}

void processFrame(Image in, Image* out)
{
	copyFrame(&in, out, false);
}

void freePipeline()
{
	freeFrame(&last);
}
