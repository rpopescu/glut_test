#ifndef THREED_H
#define THREED_H

#include <stdint.h>

struct vertex
{
	vertex(int32_t vx, int32_t vy, int32_t vz) : x(vx), y(vy), z(vz) {}
	vertex() : x(0), y(0), z(0) {}
	int32_t x,y,z;
};

#endif

