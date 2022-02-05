// Used noise: https://github.com/stegu/perlin-noise
#include "glwnd.h"
#include <vector>
#include "world_generator.h"
#include "camera.h"
//#include"noise/noise1234.h"
#include "SimplexNoise.h"

INT Width, Height;

std::vector<vec3> vertices;
std::vector<unsigned int> indices;
bool buttons[2048];

CCamera camera(45.f, vec3(0.f, 10.f, 0.f));

void fn_draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	camera.UpdateCameraState(GetStruct()->h_window);
	camera.Look();
	camera.Move(buttons, 0.001f);

	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);
	glEnd();

	glVertexPointer(3, GL_FLOAT, 0, vertices.data());
	//glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
}

void fn_window_resize(HWND hWnd, int width, int height)
{
	if (!height)
		height = 1;

	Width = width;
	Height = height;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, width / (double)height, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void fn_mousemove(HWND hWnd, int x, int y)
{
}

void fn_mouseclick(HWND hWnd, int x, int y, int button, int state)
{
}

void fn_charinput(HWND hWnd, char symbol)
{
}

//https://docs.microsoft.com/ru-ru/windows/win32/inputdev/wm-keydown
void fn_keydown(HWND hWnd, INT state, WPARAM wparam, LPARAM lparam)
{
	INT key = (INT)wparam;
	buttons[key] = state;
	if (state == KEY_DOWN) {
		switch (key) {
		case 27:
			exit(0); //close program
			break;

		case VK_F1:
			camera.SetActive(!camera.IsActive());
			break;
		}
	}
}

void generate_triangles_plane_no_eb(std::vector<vec3> &dest_verts, float xpos, float ypos, float zpos, float width_x, float width_y, float step)
{
	float half_width_x = width_x / 2.f;
	float half_width_y = width_y / 2.f;
	for (int x = xpos - half_width_x; x < xpos + half_width_x; x += step) {
		for (int z = zpos - half_width_y; z < zpos + half_width_y; z += step) {
			//создаем первый треугольник
			dest_verts.push_back(vec3((float)x, ypos, (float)z));
			dest_verts.push_back(vec3((float)x + step, ypos, (float)z));
			dest_verts.push_back(vec3((float)x, ypos, (float)z + step));

			//создаем второй треугольник
			dest_verts.push_back(vec3((float)x, ypos, (float)z + step));
			dest_verts.push_back(vec3((float)x + step, ypos, (float)z + step));
			dest_verts.push_back(vec3((float)x + step, ypos, (float)z));
		}
	}
}

void generate_triangles_plane(std::vector<vec3> &dest_verts, std::vector<unsigned int> &dest_indices,
	float xpos, float ypos, float zpos,
	float width_x, float width_y, float step)
{
	vec3 p1, p2, p3;
	int indice1, indice2;
	float half_width_x = width_x / 2.f;
	float half_width_y = width_y / 2.f;
	for (int x = xpos - half_width_x; x < xpos + half_width_x; x += step) {
		for (int z = zpos - half_width_y; z < zpos + half_width_y; z += step) {
			
			//создаем первый треугольник
			p1 = vec3((float)x, ypos, (float)z);
			dest_verts.push_back(p1);
			dest_indices.push_back(dest_verts.size() - 1);

			p2 = vec3((float)x + step, ypos, (float)z);
			dest_verts.push_back(p2);
			indice1 = dest_verts.size() - 1;
			dest_indices.push_back(indice1);

			p3 = vec3((float)x, ypos, (float)z + step);
			dest_verts.push_back(p3);
			indice2 = dest_verts.size() - 1;
			dest_indices.push_back(indice2);

			//создаем второй треугольник
			dest_indices.push_back(indice1);
			dest_indices.push_back(indice2);
                                                                                                                                                  
			p2 = vec3((float)x + step, ypos, (float)z + step);
			dest_verts.push_back(p2);
			dest_indices.push_back(dest_verts.size() - 1);
		}
	}
}

//Add this GL functions
void fn_windowcreate(HWND hWnd)
{
	RECT rct;
	GetClientRect(hWnd, &rct);
	glViewport(0, 0, (GLsizei)rct.right, (GLsizei)rct.bottom);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, rct.right / (double)rct.bottom, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnableClientState(GL_VERTEX_ARRAY);
	camera.SetActive(true);

	//generate_triangles_plane_no_eb(vertices, 0.f, 0.f, 0.f, 1000.f, 1000.f, 1.f);
	generate_triangles_plane(vertices, indices, 0.f, 0.f, 0.f, 100.f, 100.f, 1.f);

	//SimplexNoise perlin;
	//float y = 0.f;
	//for (int x = 0; x < 200; x++) {
	//	for (int z = 0; z < 200; z++) {
	//		vec3 p1, p2, p3;

	//		y = perlin.noise((float)x / 10.f, (float)z / 10.f);

	//		//создаем первый треугольник
	//		p1 = vec3((float)x, y, (float)z);
	//		p1.y = perlin.noise((float)p1.x / 10.f, (float)p1.z / 10.f);

	//		p2 = vec3((float)x + 1, y, (float)z);
	//		p2.y = perlin.noise((float)p2.x / 10.f, (float)p2.z / 10.f);

	//		p3 = vec3((float)x, y, (float)z + 1);
	//		p3.y = perlin.noise((float)p3.x / 10.f, (float)p3.z / 10.f);

	//		vertices.push_back(p1);
	//		vertices.push_back(p2);
	//		vertices.push_back(p3);
	//		//printf("Tri1( %f %f ) ( %f %f ) ( %f %f ) ", p1.x, p1.z, p2.x, p2.z, p3.x, p3.z);

	//		//создаем второй треугольник
	//		p1 = vec3((float)x, y, (float)z + 1);
	//		p1.y = perlin.noise((float)p1.x / 10.f, (float)p1.z / 10.f);

	//		p2 = vec3((float)x + 1, y, (float)z + 1);
	//		p2.y = perlin.noise((float)p2.x / 10.f, (float)p2.z / 10.f);

	//		p3 = vec3((float)x + 1, y, (float)z);
	//		p3.y = perlin.noise((float)p3.x / 10.f, (float)p3.z / 10.f);

	//		vertices.push_back(p1);
	//		vertices.push_back(p2);
	//		vertices.push_back(p3);
	//		//printf("Tri2( %f %f ) ( %f %f ) ( %f %f )\n", p1.x, p1.z, p2.x, p2.z, p3.x, p3.z);
	//	}
	//}
}

void fn_windowclose(HWND hWnd)
{
	glDisableClientState(GL_VERTEX_ARRAY);
	exit(0);
}

int main()
{
	create_window("Terrain generation with Perlin noise", __DATE__ __TIME__,
		24,					  //Colors bits
		32,					  //Depth bits
		fn_draw,			  //Draw function
		fn_window_resize,	  //Window resize function
		fn_mousemove,		  //Mouse move function
		fn_mouseclick,		  //Mouse click function
		fn_charinput,		  //Char input function
		fn_keydown,			  //Keydown function
		fn_windowcreate,	  //Window create function
		fn_windowclose,		  //Window close function
		0, 0,
		800,				  //Window width
		600);				  //Window height
	return 0;
}