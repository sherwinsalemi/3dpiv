#pragma once

#include "cv.hh"
#include "../core.hh"


float lineMap(float x1, float y1, float x2, float y2, float a);
// ^ Linearly interpolates between the two points out to infinity

vector2 thetaToCoord(float cameraYawAngle, float laserPointRight, float laserPointForward, float laserAngle);

float degToRad(float deg);
float radToDeg(float rad);
