#ifndef CAMERA_H
#define CAMERA_H


#include "linmath.h"

struct Camera
{
	float pitch;
	float yaw;
	vec3 pos;
	vec3 vel;
};

#endif