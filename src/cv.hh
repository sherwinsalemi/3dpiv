#pragma once

#include "core.hh"

struct Image
{
	u32 width, height;
	unsigned char* data; // RGB format
};

void initFrame(Image* img);
void initFrame(Image* img, u32 width, u32 height);
void zeroFrame(Image* img);
void copyFrame(Image* src, Image* dst, bool allocate = true);
void copyFrame(unsigned char* src, Image* dst, bool allocate = true);
void freeFrame(Image* img);

void initPipeline();
void processFrame(Image in, Image* out);
void freePipeline();

#define RED 0
#define GRN 1
#define BLU 2 
#define pix(img, x, y, offset) img.data[(y) * img.width * 3 + (x)* 3 + offset]

unsigned char CLAMP(unsigned char in, unsigned char low, unsigned char high);
i32 CLAMP(i32 in, i32 low, i32 high);
u32 CLAMP(u32 in, u32 low, u32 high);

unsigned char MAX(unsigned char a, unsigned char b);
unsigned char MIN(unsigned char a, unsigned char b);
i32 MAX(i32 a, i32 b);
i32 MIN(i32 a, i32 b);
u32 MAX(u32 a, u32 b);
u32 MIN(u32 a, u32 b);

