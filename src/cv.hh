#pragma once

#include "core.hh"

struct Image
{
	u32 width, height;
	unsigned char* data; // RGB format
};

void processFrame(Image in, Image* out);