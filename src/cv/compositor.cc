#include "compositor.hh"

#include <math.h>
#include <stdio.h>

/*
Notes:

Camera points in the positive X direction
0 radians theta is in the positive X direction
Positive is counterclockwise, so angle increases from right to left in the image
Forward and right are from the camera's perspective
*/

float lineMap(float x1, float y1, float x2, float y2, float a)
{
	return ((y2 - y1) / (x2 - x1)) * (a - x1) + y1;
}

vector2 thetaToCoord(float cameraYawAngle, float laserPointRight, float laserPointForward, float laserAngle)
{
	vector2 vec {0};

	if (tanf(laserAngle) - tanf(cameraYawAngle) == 0.0f)
	{
		printf("Warning: laser and camera planes parralel, clipping to zero\n");
		vec.x = 0;
		vec.y = 0;
		return vec;
	}

	vec.x = (laserPointForward * tanf(laserAngle) + laserPointRight) / (tanf(laserAngle) - tanf(cameraYawAngle));
	vec.y = tanf(cameraYawAngle) * vec.x;
	return vec;
}

float degToRad(float deg)
{
	return deg * PI / 180.0;
}

float radToDeg(float rad)
{
	return rad * 180.0 / PI;
}
