/********************************/
/* recursive sphere             */
/* (C) Bedrich Benes 2020       */
/* bbenes ~ at ~ purdue.edu     */
/********************************/

#include <stdio.h>
#include <iostream>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <string>
#include <vector>			//Standard template library class
#include <map>
#include <GL/glew.h>
#include <GL/freeglut.h>
//in house created libraries
#include "math/vect3d.h"    //for vector manipulation
#include "helper.h"         
#include "trackball.h"      //virtual trackball

#include "math/triangle.h"  //triangles
#include "objGen.h"         //to save OBJ file format for 3D printing

#pragma warning(disable : 4996)
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")

using namespace std;

TrackBallC trackball;
bool mouseLeft, mouseMid, mouseRight;


GLuint points = 0; //number of points to display the object
int steps = 0;//# of recursive subdivisions
bool needRedisplay = false;
GLfloat  sign = +1;
const GLfloat defaultIncrement = 0.3;
GLfloat  angleIncrement = defaultIncrement;


//the main window size
GLint wWindow = 800;
GLint hWindow = 600;

map <pair<int, int>, Vect3d> a;
vector <Vect3d> p;
vector <GLfloat> xyz;

inline void AddVertex(vector <GLfloat>* a, const GLfloat* A)
{
	a->push_back(A[0]); a->push_back(A[1]); a->push_back(A[2]);
}


vector <TriangleC> tri;   //all the quads will be stored here

void CreateTree(map <pair<int, int>, Vect3d> &a, int e, vector <Vect3d>* c, vector <GLfloat>* d)
{

	map <pair<int, int>, Vect3d> b;
	//int x, y;
	Vect3d v;
	int slices = 9;


	Vect3d v00 = Vect3d(0, 0, 0.2);
	Vect3d v10 = Vect3d(0.2, 0, 0.2);
	Vect3d v20 = Vect3d(0.2, 0, 0);
	Vect3d v30 = Vect3d(0, 0, 0);

	Vect3d v01 = Vect3d(0, 0.25, 0.2);
	Vect3d v11 = Vect3d(0.2, 0.25, 0.2);
	Vect3d v21 = Vect3d(0.2, 0.25, 0);
	Vect3d v31 = Vect3d(0, 0.25, 0);

	Vect3d v02 = Vect3d(0, 0.35, 0.2);
	Vect3d v12 = Vect3d(0.5, 0.35, 0.2);
	Vect3d v22 = Vect3d(0.5, 0.35, 0.0);
	Vect3d v32 = Vect3d(0, 0.35, 0);

	Vect3d v03 = Vect3d(0.0, 0.45, 0.2);
	Vect3d v13 = Vect3d(0.5, 0.45, 0.2);
	Vect3d v23 = Vect3d(0.5, 0.45, 0.0);
	Vect3d v33 = Vect3d(0, 0.45, 0);

	Vect3d v04 = Vect3d(0, 0.55, 0.2);
	Vect3d v14 = Vect3d(0.2, 0.55, 0.2);
	Vect3d v24 = Vect3d(0.2, 0.55, 0);
	Vect3d v34 = Vect3d(0, 0.55, 0);

	Vect3d v05 = Vect3d(0, 0.9, 0.2);
	Vect3d v15 = Vect3d(0.2, 0.9, 0.2);
	Vect3d v25 = Vect3d(0.2, 0.9, 0);
	Vect3d v35 = Vect3d(0, 0.9, 0);

	Vect3d v06 = Vect3d(-0.5, 1, 0.2);
	Vect3d v16 = Vect3d(0.2, 1, 0.2);
	Vect3d v26 = Vect3d(0.2, 1, 0);
	Vect3d v36 = Vect3d(-0.5, 1, 0);

	Vect3d v07 = Vect3d(-0.5, 1.10, 0.2);
	Vect3d v17 = Vect3d(0.2, 1.10, 0.2);
	Vect3d v27 = Vect3d(0.2, 1.10, 0);
	Vect3d v37 = Vect3d(-0.5, 1.10, 0);

	Vect3d v08 = Vect3d(0, 1.2, 0.2);
	Vect3d v18 = Vect3d(0.2, 1.2, 0.2);
	Vect3d v28 = Vect3d(0.2, 1.2, 0);
	Vect3d v38 = Vect3d(0, 1.2, 0);

	Vect3d v09 = Vect3d(0.05, 1.6, 0.15);
	Vect3d v19 = Vect3d(0.15, 1.6, 0.15);
	Vect3d v29 = Vect3d(0.15, 1.6, 0.05);
	Vect3d v39 = Vect3d(0.05, 1.6, 0.05);

	if (e == 0)
	{
		int m = slices;
		a = {
		{make_pair(0,0),v00}, {make_pair(1, 0), v10}, { make_pair(2,0),v20 }, { make_pair(3,0),v30 },
		{make_pair(0,1),v01},	{make_pair(1,1),v11},	{make_pair(2,1),v21},	{make_pair(3,1),v31},
		{make_pair(0,2),v02},	{make_pair(1,2),v12},	{make_pair(2,2),v22},	{make_pair(3,2),v32},
		{{0,3},v03},	{{1,3},v13},	{{2,3},v23},	{{3,3},v33},
		{{0,4},v04},	{{1,4},v14},	{{2,4},v24},	{{3,4},v34},
		{{0,5},v05},	{{1,5},v15},	{{2,5},v25},	{{3,5},v35},
		{{0,6},v06},	{{1,6},v16},	{{2,6},v26},	{{3,6},v36},
		{{0,7},v07},	{{1,7},v17},	{{2,7},v27},	{{3,7},v37},
		{{0,8},v08},	{{1,8},v18},	{{2,8},v28},	{{3,8},v38},
		{{0,9},v09},	{{1,9},v19},	{{2,9},v29},	{{3,9},v39}
		};

		for (int y = 0; y < m; y++)
			for (int x = 0; x < 4; x++)
			{
				if (x == 3)
				{
					v = a[make_pair(3, y)];
					c->push_back(v);
					v = a[make_pair(0, y)];
					c->push_back(v);
					v = a[make_pair(0, y + 1)];
					c->push_back(v);
					v = a[make_pair(3, y + 1)];
					c->push_back(v);
					break;
				}
				c->push_back(a[make_pair(x, y)]);
				c->push_back(a[make_pair(x + 1, y)]);
				c->push_back(a[make_pair(x + 1, y + 1)]);
				c->push_back(a[make_pair(x, y + 1)]);
			}

		for (int i = 0; i < c->size(); i++)
		{
			AddVertex(d, c->at(i));
		}
		return;
	}

	int m = slices * (pow(2, (e - 1)));
	Vect3d btmtemp;
	Vect3d toptemp;
	Vect3d fp;
	Vect3d epv;
	Vect3d eph;
	Vect3d vertex;
	Vect3d fpl;
	Vect3d ephl;
	Vect3d epvl;

	//right top and right bottom loop end condition
	btmtemp = (a[make_pair(pow(2, (e + 1)) - 1, 0)] + a[make_pair(0, 0)]) / 2.0f;
	b[make_pair(2 *pow(2, (e + 1)) - 1, -1)] = btmtemp;
	b[make_pair(-1, -1)] = btmtemp;
	toptemp = (a[make_pair(pow(2, (e + 1)) - 1, m)] + a[make_pair(0, m)]) / 2.0f;
	b[make_pair(2 * pow(2, (e + 1)) - 1, 2 * m + 1)] = toptemp;
	b[make_pair(-1, 2 * m + 1)] = toptemp;

	//below bottom
	for (int x = 0; x < pow(2, (e + 1)); x++)
	{
		b[make_pair(2 * x, -1)] = a[make_pair(x, -1)];
	}

	for (int x = 0; x < pow(2, (e + 1)); x++)
	{
		b[make_pair(2 * x, 2 * m + 1)] = a[make_pair(x, m)];
	}

	//top row and bottom row end condition
	for (int x = 0; x < pow(2, (e + 1)) - 1; x++)
	{
		btmtemp = (a[make_pair(x, 0)] + a[make_pair(x + 1, 0)]) / 2.0f;
		b[make_pair(2 * x + 1, -1)] = btmtemp;
		toptemp = (a[make_pair(x, m)] + a[make_pair(x + 1, m)]) / 2.0f;
		b[make_pair(2 * x + 1, 2 * m + 1)] = toptemp;
	}

	//all general map condition
	for (int y = 0; y < m; y++)
		for (int x = 0; x < pow(2, (e + 1)) - 1; x++)
		{
			fp = (a[make_pair(x, y)] + a[make_pair(x + 1, y)] + a[make_pair(x + 1, y + 1)] + a[make_pair(x, y + 1)]) / 4.0f;
			b[make_pair(2 * x + 1, 2 * y + 1)] = fp;
		}

	//right column loop condition
	for (int y = 0; y < m; y++)
	{
		fpl = (a[make_pair(pow(2, (e + 1)) - 1, y)] + a[make_pair(0, y)] + a[make_pair(pow(2, (e + 1)) - 1, y + 1)] + a[make_pair(0, y + 1)]) / 4.0f;
		b[make_pair(2 * pow(2, (e + 1)) - 1, 2 * y + 1)] = fpl;
		b[make_pair(-1, 2 * y + 1)] = fpl;
	}

	for (int y = 0; y < m + 1; y++)
		for (int x = 0; x < pow(2, (e + 1)) - 1; x++)
		{
			eph = (b[make_pair(2 * x + 1, 2 * y + 1)] + b[make_pair(2 * x + 1, 2 * y - 1)] + a[make_pair(x, y)] + a[make_pair(x + 1, y)]) / 4.0f;
			b[make_pair(2 * x + 1, 2 * y)] = eph;

		}

	for (int y = 0; y < m + 1; y++)
	{
		ephl = (a[make_pair(pow(2, (e + 1)) - 1, y)] + a[make_pair(0, y)] + b[make_pair(2 * pow(2, (e + 1)) - 1, 2 * y + 1)] + b[make_pair(2 * pow(2, (e + 1)) - 1, 2 * y - 1)]) / 4.0f;
		b[make_pair(2 * pow(2, (e + 1)) - 1, 2 * y)] = ephl;
		b[make_pair(-1, 2 * y)] = ephl;
	}

	for (int y = 0; y < m; y++)
		for (int x = 0; x < pow(2, (e + 1)); x++)
		{
			epv = (b[make_pair(2 * x + 1, 2 * y + 1)] + b[make_pair(2 * x - 1, 2 * y + 1)] + a[make_pair(x, y)] + a[make_pair(x, y + 1)]) / 4.0f;
			b[make_pair(2 * x, 2 * y + 1)] = epv;
		}
	/*
	for (int y = 0; y < m*pow(2, (e - 1)); y++)
	{
		epvl = (b[make_pair(2 * (pow(2, (e + 1)) - 1) + 1, 2 * y + 1)] + b[make_pair(2 * (pow(2, (e + 1)) - 1) + 1, 2 * y -1)] + a[make_pair(pow(2, (e + 1)) - 1, y)] + a[make_pair(pow(2, (e + 1)) - 1, y + 1)]) / 4.0f;
		b[make_pair(2 * (pow(2, (e + 1)) - 1) , 2 * y + 1)] = epv;
	}*/

	for (int y = 0; y < m + 1; y++)
		for (int x = 0; x < pow(2, (e + 1)); x++)
		{
			vertex = a[make_pair(x, y)] / 4.0f + (b[make_pair(2 * x + 1, 2 * y + 1)] + b[make_pair(2 * x + 1, 2 * y - 1)] + b[make_pair(2 * x - 1, 2 * y + 1)] + b[make_pair(2 * x - 1, 2 * y - 1)]) / 16.0f + (b[make_pair(2 * x + 1, 2 * y)] + b[make_pair(2 * x - 1, 2 * y)] + b[make_pair(2 * x, 2 * y + 1)] + b[make_pair(2 * x, 2 * y - 1)]) / 8.0f;
			b[make_pair(2 * x, 2 * y)] = vertex;
		}

	for (int y = 0; y < 2 * m; y++)
		for (int x = 0; x < 2 * pow(2, (e + 1)); x++)
		{
			if (x == 2 * pow(2, (e + 1)) - 1)
			{
				v = b[make_pair(x, y)];
				c->push_back(v);
				v = b[make_pair(0, y)];
				c->push_back(v);
				v = b[make_pair(0, y + 1)];
				c->push_back(v);
				v = b[make_pair(x, y + 1)];
				c->push_back(v);
				break;
			}

			c->push_back(b[make_pair(x, y)]);
			c->push_back(b[make_pair(x + 1, y)]);
			c->push_back(b[make_pair(x + 1, y + 1)]);
			c->push_back(b[make_pair(x, y + 1)]);
		}

	for (int i = 0; i < c->size(); i++)
	{
		AddVertex(d, c->at(i));
	}

	for (int y = 0; y < 2 * m + 1; y++)
		for (int x = 0; x < 2 * pow(2, (e + 1)); x++)
		{
			a[make_pair(x, y)] = b[make_pair(x, y)];
		}
}

void InitArray(int n)
{
	p.clear();
	xyz.clear();
	GLuint vaID;
	CreateTree(a, n, &p, &xyz);
	//to create obj
	tri.clear();
	for (unsigned int i = 0; i < xyz.size(); i +=12) { //stride 3 - 3 vertices per triangle
		TriangleC tmp;
		Vect3d a, b, c;
		a.Set(xyz[i], xyz[i + 1], xyz[i + 2]);
		b.Set(xyz[i + 3], xyz[i + 4], xyz[i + 5]);
		c.Set(xyz[i + 6], xyz[i + 7], xyz[i + 8]);
		tmp.Set(a, b, c); //store them for 3D printing
		tri.push_back(tmp);
		a.Set(xyz[i], xyz[i + 1], xyz[i + 2]);
		b.Set(xyz[i + 6], xyz[i + 7], xyz[i + 8]);
		c.Set(xyz[i + 9], xyz[i + 10], xyz[i + 11]);
		tmp.Set(a, b, c); //store them for 3D printing
		tri.push_back(tmp);
	}
	glGenVertexArrays(1, &vaID);
	glBindVertexArray(vaID);
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	points = xyz.size();
	glBufferData(GL_ARRAY_BUFFER, points * sizeof(GLfloat), &xyz[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//p.clear();
	//xyz.clear(); //no need for the data, it is on the GPU now
}



//the main rendering function
void RenderObjects()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0, 0, 1);
	glPointSize(2);
	glLineWidth(1);
	trackball.Set3DViewCamera();
	CoordSyst();
	glDrawArrays(GL_QUADS, 0, 4 * points);
}


void Idle(void)
{
	glClearColor(1.9, 1.9, 1.9, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLMessage("Tree");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, (GLfloat)wWindow / (GLfloat)hWindow, 0.01, 100); //set the camera
	glMatrixMode(GL_MODELVIEW); //set the scene
	glLoadIdentity();
	gluLookAt(0, 5, 5, 0, 0, 0, 0, 1, 0);
	static GLfloat angle = 0;
	angle += angleIncrement;
	if (angle >= 360.f) angle = 0.f;
	glRotatef(sign * angle, 0, 1, 0);
	RenderObjects();
	glutSwapBuffers();
}


void Kbd(unsigned char a, int x, int y)//keyboard callback
{
	switch (a)
	{
	case 27: exit(0); break;
	case 32: {
		if (angleIncrement == 0) angleIncrement = defaultIncrement;
		else angleIncrement = 0;
		break;
	}
	case 's': {sign = -sign; break; }
	case '-': {
		steps--;
		if (steps < 0) steps = 0;
		InitArray(steps);
		break;
	}
	case '+': {
		steps++;
		InitArray(steps);
		break;
	}
	case 'w': {
		SaveOBJ(&tri, "geometry.obj"); break;
	}
	}
	cout << "[steps]=[" << steps << "]" << endl;
	glutPostRedisplay();
}


void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		trackball.Set(true, x, y);
		mouseLeft = true;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		trackball.Set(false, x, y);
		mouseLeft = false;
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		trackball.Set(true, x, y);
		mouseMid = true;
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
	{
		trackball.Set(true, x, y);
		mouseMid = false;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		trackball.Set(true, x, y);
		mouseRight = true;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		trackball.Set(true, x, y);
		mouseRight = false;
	}
}

void MouseMotion(int x, int y) {
	if (mouseLeft)  trackball.Rotate(x, y);
	if (mouseMid)   trackball.Translate(x, y);
	if (mouseRight) trackball.Zoom(x, y);
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInitDisplayString("stencil>=2 rgb double depth samples");
	glutInit(&argc, argv);
	glutInitWindowSize(wWindow, hWindow);
	glutInitWindowPosition(500, 100);
	glutCreateWindow("Vertex Array Example");
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Kbd); //+ and -
	glutSpecialUpFunc(NULL);
	glutSpecialFunc(NULL);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	InitArray(steps);
	glutMainLoop();
	return 0;
}
