#pragma once
#include <Windows.h>
#include <gl/gl.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <vector>

#include "glmath.h"

#define CHUNK_WIDTH 30.f
#define DISTANCE 20

class world_chunk {
public:
	world_chunk() {}
	~world_chunk() {}

	void generate_mesh() {
		for (float x = position.x; x < position.x + CHUNK_WIDTH; x++) {
			for (float z = position.z; z < position.z + CHUNK_WIDTH; z++) {

			}
		}
	}

	vec3 position;
	std::vector<vec3> vertices;
};

class world_generator
{
public:
	world_generator() {}
	~world_generator() {}

	int init() {

		return 1;
	}

	void shutdown() {



	}

	void draw() {
		for (int i = 0; i < number_of_chunks; i++) {

		}
	}

	int number_of_chunks;
};

