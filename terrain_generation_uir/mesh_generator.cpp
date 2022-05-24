#include "mesh_generator.h"

void generate_triangles_plane_noise(std::vector<vec3>& dest_verts, std::vector<unsigned int>& dest_indices, float xpos, float ypos, float zpos, float width_x, float width_y, float step, void *p_parameter, void(*vertex_filter_fn)(vec3 &vertex, void *p_parameter))
{
	vec3 p1, p2, p3;
	int indice1, indice2;
	float half_width_x = width_x / 2.f;
	float half_width_y = width_y / 2.f;
	for (int x = xpos - half_width_x; x < xpos + half_width_x; x += step) {
		for (int z = zpos - half_width_y; z < zpos + half_width_y; z += step) {

			//создаем первый треугольник
			p1 = vec3((float)x, ypos, (float)z);
			vertex_filter_fn(p1, p_parameter);
			dest_verts.push_back(p1);
			dest_indices.push_back(dest_verts.size() - 1);

			p2 = vec3((float)x + step, ypos, (float)z);
			vertex_filter_fn(p2, p_parameter);
			dest_verts.push_back(p2);
			indice1 = dest_verts.size() - 1;
			dest_indices.push_back(indice1);

			p3 = vec3((float)x, ypos, (float)z + step);
			vertex_filter_fn(p3, p_parameter);
			dest_verts.push_back(p3);
			indice2 = dest_verts.size() - 1;
			dest_indices.push_back(indice2);

			//создаем второй треугольник
			dest_indices.push_back(indice1);
			dest_indices.push_back(indice2);

			p2 = vec3((float)x + step, ypos, (float)z + step);
			vertex_filter_fn(p2, p_parameter);
			dest_verts.push_back(p2);
			dest_indices.push_back(dest_verts.size() - 1);
		}
	}
}

void generation_vertex_filter(vec3 &vertex, void *p_parameter)
{
	terrain_generation_properties *p_genproperties = (terrain_generation_properties *)p_parameter;
	SimplexNoise pnoise;
	float x = p_genproperties->seed + vertex.x;
	float z = p_genproperties->seed + vertex.z;
	float xpos = (x / p_genproperties->world_width) * p_genproperties->frequency;
	float zpos = (z / p_genproperties->world_width) * p_genproperties->frequency;
	float ysmall_noise = pnoise.noise(x, z) * p_genproperties->small_noiseK;
	float ypos = (pnoise.noise(xpos, zpos) * p_genproperties->amplitude) + ysmall_noise;
	vertex.y = ypos * p_genproperties->multiplier;
}

void generate_terrain_mesh(terrain_generation_properties *p_genproperties, std::vector<vec3>& dest_verts, std::vector<unsigned int>& dest_indices)
{
	dest_verts.clear();
	dest_indices.clear();
	generate_triangles_plane_noise(dest_verts, dest_indices, 0.f, 0.f, 0.f,
		p_genproperties->world_width, p_genproperties->world_width,
		p_genproperties->verts_step, p_genproperties, generation_vertex_filter);
}
