#ifndef SCENE_H
#define SCENE_H

#include <math.h>
#include "linmath.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

static vec3 UP = {0.0f, 1.0f, 0.0f};
static vec3 roofPos = {0.0f, 10.5f, -2.5f};
static vec3 roofSize = {6.0f, 0.5f, 5.0f};
static vec3 wallPos = {0.0f, 5.0f, 0.0f};
static vec3 wallSize = {5.0f, 5.0f, 0.4f};
static vec3 spherePos = {-0.7f, 5.0f, 0.0f};
static vec3 doorPos = {-0.7f, 2.5f, 0.0f};
static vec3 doorSize = {3.0f, 2.5f, 1.0f};


float dot(vec3 a, vec3 b){
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

float fPlane(vec3 p, vec3 n, float distanceFromOrigin) {
	return dot(p, n) + distanceFromOrigin;
}

float fBoxCheap(vec3 p, vec3 b) { //cheap box
    float x = abs(p[0]) - b[0];
    float y = abs(p[1]) - b[1];
    float z = abs(p[2]) - b[2];
	return MIN(x, MIN(y, z));
}

float fOpUnionChamfer(float a, float b, float r) {
	return MIN(MIN(a, b), (a - r + b)*sqrt(0.5));
}

float fCylinder(vec3 p, float r, float height) {
	float d = sqrt(pow(p[0], 2.0) + pow(p[2], 2.0)) - r;
	return MAX(d, abs(p[1]) - height);
}

float fOpIntersectionChamfer(float a, float b, float r) {
	return MAX(MAX(a, b), (a + r + b) * sqrt(0.5));
}

float scene(vec3 camPos){
    vec3 p = {camPos[0], camPos[1], camPos[2]};
    float minDist = 10000.0;
    vec3 out;
	
	float ground = fPlane(p, UP, 0);
	float lastY = p[1];
	p[1] += p[2] * .7;
    vec3_sub(out, p, roofPos);
	float roof = fBoxCheap(out, roofSize);
	p[1] = lastY;

    vec3_sub(out, p, wallPos);
	float box = fOpUnionChamfer(fBoxCheap(out, wallSize), roof, .1);

	p[0] = abs(p[0]);

    vec3_sub(out, p, spherePos);
    float y = out[1];
    out[1] = out[2];
    out[2] = y;
	float sphere = fCylinder(out, 2.5, 1.1);
    vec3_sub(out, p, doorPos);
	float doorway = fOpUnionChamfer(fBoxCheap(out, doorSize), 
									sphere, .1);

	float wall = fOpIntersectionChamfer(box, -doorway, .1);

	minDist = MIN(minDist, ground);
	minDist = MIN(minDist, wall);
    
    return minDist;
}

#endif