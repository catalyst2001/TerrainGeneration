#pragma once
#include <math.h>

typedef struct {
	float x, y;
} vector2;

/* Function to linearly interpolate between a0 and a1
 * Weight w should be in the range [0.0, 1.0]
 */
float interpolate(float a0, float a1, float w);

/* Create pseudorandom direction vector
 */
vector2 randomGradient(int ix, int iy);

// Computes the dot product of the distance and gradient vectors.
float dotGridGradient(int ix, int iy, float x, float y);

// Compute Perlin noise at coordinates x, y
float perlin(float x, float y);