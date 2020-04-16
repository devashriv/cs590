/*********************************/
/* CS 590CGS Lab framework        */
/* (C) Bedrich Benes 2020        */
/* bbenes ~ at ~ purdue.edu      */
/* Press +,- to add/remove points*/
/*       r to randomize          */
/*       s to change rotation    */
/*       c to render curve       */
/*       t to render tangents    */
/*       p to render points      */
/*       s to change rotation    */
/*                               */
/*******************************************/
/* Edits by Devashri                       */
/* Press 1,2,3 to change curves            */
/*       f to render frenet frames         */
/*       o to render osculating circle     */
/*       >,< to move osculating circle     */
/* Changed formula for tangent calculation */
/* 's to change rotation' bug fixed        */
/*                                         */
/*******************************************/

#include <stdio.h>
#include <iostream>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <string>
#include <vector>			//Standard template library class
#include <GL/freeglut.h>


//in house created libraries
#include "math/vect3d.h"    //for vector manipulation
#include "trackball.h"

#pragma warning(disable : 4996)
#pragma comment(lib, "freeglut.lib")

using namespace std;

//some trackball variables -> used in mouse feedback
TrackBallC trackball;
bool mouseLeft, mouseMid, mouseRight;


GLuint points=0;  //number of points to display the object
int steps=30;     //# of subdivisions
int j = 2;        //oscullating circle location tracker
bool needRedisplay=false;
GLfloat  sign=-1; //diretcion of rotation
const GLfloat defaultIncrement=50.f; //speed of rotation
GLfloat  angleIncrement=defaultIncrement;

vector <Vect3d> v;   //all the points will be stored here
vector <Vect3d> x;   //oscullating circle points will be stored here

//window size
GLint wWindow=1200;
GLint hWindow=800;

//this defines what will be rendered
//see Key() how is it controlled
bool tangentsFlag = false;
bool pointsFlag = false;
bool curveFlag = true;
bool frenetFlag = false; //do not display frenet frames as default 
bool oscFlag = false;    //do not show oscullating circle as default
int curveIndex = 1;      //setting curve 1 as default

/*********************************
Some OpenGL-related functions DO NOT TOUCH
**********************************/
//displays the text message in the GL window
void GLMessage(char *message)
{
	static int i;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.f, 100.f, 0.f, 100.f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3ub(0, 0, 255);
	glRasterPos2i(10, 10);
	for (i = 0; i<(int)strlen(message); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

//called when a window is reshaped
void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glEnable(GL_DEPTH_TEST);
	//remember the settings for the camera
	wWindow = w;
	hWindow = h;
}

//Some simple rendering routines using old fixed-pipeline OpenGL
//draws line from a to b with color 
void DrawLine(Vect3d a, Vect3d b, Vect3d color) {

	glColor3fv(color);
	glBegin(GL_LINES);
		glVertex3fv(a);
		glVertex3fv(b);
	glEnd();
}

//draws point at a with color 
void DrawPoint(Vect3d a, Vect3d color) {
	 
	glColor3fv(color);
	glPointSize(5);
	glBegin(GL_POINTS);
	 glVertex3fv(a);
	glEnd();
}


inline static Vect3d Cross(const Vect3d& a, const Vect3d& b)
{
	return Vect3d(a.v[1] * b.v[2] - a.v[2] * b.v[1], a.v[2] * b.v[0] - a.v[0] * b.v[2], a.v[0] * b.v[1] - a.v[1] * b.v[0]);
}

/**********************
LAB related MODIFY
***********************/

//This defines the actual curve 1
inline Vect3d P(GLfloat t)
{
	const float rad = 0.5f;
	const float height = 1.f;
	const float rot = 5.f;
	return Vect3d(sign*rad*(float)sin(rot*M_PI*t),height*t,sign*rad*(float)cos(rot*M_PI*t)); //spiral with radius rad, height, and rotations rot
}

//This defines the curve 2
inline Vect3d Q(GLfloat t)
{
	const float a = 0.5f;
	const float b = 1.f;
	const float rot = 5.f;
	return Vect3d(sign*a *t* (float)sin(rot * M_PI * t)* (float)sin(rot * M_PI * t)* (float)sin(rot * M_PI * t), -t,t+ b * (float)cos(rot * M_PI * t)* (float)cos(rot * M_PI * t)* (float)cos(rot * M_PI * t)); //asteroid helix
}

//This defines curve 3
inline Vect3d R(GLfloat t)
{
	const float rad = 0.2f;
	const float R = 1.f;
	const float rot = 20.f;
	return Vect3d((R + sign*rad * (float)cos(rot * M_PI * t)) * (float)sin(M_PI * t), rad * t * (float)sin(rot * M_PI * t), (R + sign*rad * (float)cos(rot * M_PI * t)) * (float)cos(M_PI * t)); //toroifdal helix
}

//This defines the osculating circle
inline Vect3d C(GLfloat u, Vect3d xaxis, Vect3d yaxis, Vect3d cent)
{
	return Vect3d(xaxis * sin(u*M_PI/180) + yaxis * cos(u*M_PI/180)+cent); 
}

//This fills the <vector> *a with data. 
void CreateCurve(vector <Vect3d> *a, int n)
{
	GLfloat step=1.f/n;
	switch (curveIndex)
	{
	case(1):
	{
		for (int i = 0; i < n; i++)
		{
			a->push_back(P(i * step));
		}
		break;
	}
	case(2): 
	{
		for (int i = 0; i < n; i++)
		{
			a->push_back(Q(i * step));
		}
		break;
	}
	case(3):
	{
		for (int i = 0; i < n; i++)
		{
			a->push_back(R(i * step));
		}
		break;
	}
	}
}

//This will fill vector * b with data
void CreateCircle(vector <Vect3d>* b, Vect3d xaxis, Vect3d yaxis, Vect3d cent)
{
	//no. segments in osculating circle
	for (int i = 0; i<=360;i+=10)
	{ 
		b->push_back(C(i,xaxis, yaxis, cent));
	}
}

//Call THIS for a new curve. It clears the old one first
void InitArray(int n)
{
	v.clear();
	CreateCurve(&v,n); 
}

//Call this for osculating circle
void InitOsc()
{
	x.clear();
	Vect3d tan = v[j + 1] - v[j - 1];//tangent vector at desired point j
	//radius of the oscullating circle at a given point j on the curve
	float r = ((v[j - 1] - v[j]).Length() * (v[j] - v[j + 1]).Length() * tan.Length()) / (2 * (Cross(v[j - 1] - v[j], v[j] - v[j + 1]).Length()));
	Vect3d nor = Cross(v[j - 1] - v[j], v[j + 1] - v[j]); //tangent vector at point j
	Vect3d rad = Cross(tan, nor); //radius vector at point j
	rad.Normalize();
	rad *= r;
	Vect3d cent = v[j] + rad; //center of the circle at point j
	tan.Normalize();
	tan *= r;
	CreateCircle(&x, tan, rad, cent);
}

//returns random number from <-1,1>
inline float random11() { 
	return 2.f*rand() / (float)RAND_MAX - 1.f;
}

//randomizes an existing curve by adding random number to each coordinate
void Randomize(vector <Vect3d> *a) {
	const float intensity = 0.01f;
	for (unsigned int i = 0; i < a->size(); i++) {
		Vect3d r(random11(), random11(), random11());
		a->at(i) = a->at(i) + intensity*r;
	}
}

//display coordinate system
void CoordSyst() {
	Vect3d a, b, c;
	Vect3d origin(0, 0, 0);
	Vect3d red(1, 0, 0), green(0, 1, 0), blue(0, 0, 1), almostBlack(0.1f, 0.1f, 0.1f), yellow(1, 1, 0);

	//draw the coordinate system 
	a.Set(1, 0, 0);
	b.Set(0, 1, 0);
	c.Set(Vect3d::Cross(a, b)); //use cross product to find the last vector
	glLineWidth(4);
	DrawLine(origin, a, red);
	DrawLine(origin, b, green);
	DrawLine(origin, c, blue);
	glLineWidth(1);

}

//this is the actual code for the lab
void Lab01() {
	Vect3d a,b,c;
	Vect3d origin(0, 0, 0);
	Vect3d red(1, 0, 0), green(0, 1, 0), blue(0, 0, 1), almostBlack(0.1f, 0.1f, 0.1f), yellow(1, 1, 0);


	CoordSyst();
	//draw the curve
	if (curveFlag)
		for (unsigned int i = 0; i < v.size() - 1; i++) {
		DrawLine(v[i], v[i + 1], almostBlack);
	}

	//draw osculating circle
	if (oscFlag)
		for (unsigned int i = 0; i< x.size() - 1; i++) {
		DrawLine(x[i], x[i + 1], green);
	}

	//draw the points
	if (pointsFlag)
		for (unsigned int i = 0; i < v.size() - 1; i++) {
		DrawPoint(v[i], blue);
	}

    //draw the tangents
	if (tangentsFlag)
	for (unsigned int i = 1; i < v.size() - 1; i++) {
		Vect3d tan;
		tan = v[i + 1] - v[i-1]; //too simple - could be better from the point after AND before
		tan.Normalize(); 
		tan *= 0.2f;
		DrawLine(v[i], v[i]+tan, red);
	}

	//draw the frenet frames
	if (frenetFlag)
		for (unsigned int i = 1; i < v.size() - 1; i++) {
			Vect3d tan;
			Vect3d nor;
			Vect3d crs;
			tan = v[i + 1] - v[i - 1]; //too simple - could be better from the point after AND before
			nor = Cross(v[i + 1] - v[i], v[i - 1] - v[i]);
			crs = Cross(nor, tan);
			tan.Normalize();
			nor.Normalize();
			crs.Normalize();
			tan *= 0.2f;
			nor *= 0.2f;
			crs *= 0.2f;
			DrawLine(v[i], v[i] + tan, red);
			DrawLine(v[i], v[i] + nor, green);
			DrawLine(v[i], v[i] + crs, blue);
		}
}

//the main rendering function
void RenderObjects()
{
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	//set camera
	glMatrixMode(GL_MODELVIEW);
	trackball.Set3DViewCamera();
	//call the student's code from here
	Lab01();
}

//Add here if you want to control some global behavior
//see the pointFlag and how is it used
void Kbd(unsigned char a, int x, int y)//keyboard callback
{
	switch (a)
	{
	case 27: exit(0); break;
	case 't': tangentsFlag = !tangentsFlag; break;
	case 'p': pointsFlag = !pointsFlag; break;
	case 'c': curveFlag = !curveFlag; break;
	case 'f': frenetFlag = !frenetFlag; break;
	case 32: {
		if (angleIncrement == 0) angleIncrement = defaultIncrement;
		else angleIncrement = 0;
		break;
	}
	case 's': {
		sign = -sign; 
		InitArray(steps);
		InitOsc();
		break; }
	case '-': {
		steps--;
		if (steps<1) steps = 1;
		InitArray(steps);
		break;
	}
	case '+': {
		steps++;
		InitArray(steps);
		break;
	}
	case 'r': {
		Randomize(&v);
		break;
	}
	case '1': {
		curveIndex = 1;
		InitArray(steps);
		InitOsc();
		break;
	}
	case '2': {
		curveIndex = 2;
		InitArray(steps);
		InitOsc();
		break;
	}
	case '3': {
		curveIndex = 3;
		InitArray(steps);
		InitOsc();
		break;
	}

	case 'o': { 
		oscFlag = !oscFlag;
		break;
	}
	case '<': {
		if (j > 1) j--;
		else j = steps-2;
		InitOsc();
		break;
	}
	case '>': {
		if (j < steps-2) j++;
		else j = 1;
		InitOsc();
		break;
	}
			
	}
	cout << "[points]=[" << steps << "]" << endl;
	glutPostRedisplay();
}


/*******************
OpenGL code. Do not touch.
******************/
void Idle(void)
{
  glClearColor(0.5f,0.5f,0.5f,1); //background color
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  GLMessage("Lab 1 - CS 590CGS");
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40,(GLfloat)wWindow/(GLfloat)hWindow,0.01,100); //set the camera
  glMatrixMode(GL_MODELVIEW); //set the scene
  glLoadIdentity();
  gluLookAt(0,10,10,0,0,0,0,1,0); //set where the camera is looking at and from. 
  static GLfloat angle=0;
  angle+=angleIncrement;
  if (angle>=360.f) angle=0.f;
  glRotatef(sign*angle,0,1,0);
  RenderObjects();
  glutSwapBuffers();  
}

void Display(void)
{

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


int main(int argc, char **argv)
{ 
  glutInitDisplayString("stencil>=2 rgb double depth samples");
  glutInit(&argc, argv);
  glutInitWindowSize(wWindow,hWindow);
  glutInitWindowPosition(500,100);
  glutCreateWindow("Surface of Revolution");
  //GLenum err = glewInit();
  // if (GLEW_OK != err){
  // fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  //}
  glutDisplayFunc(Display);
  glutIdleFunc(Idle);
  glutReshapeFunc(Reshape);
  glutKeyboardFunc(Kbd); //+ and -
  glutSpecialUpFunc(NULL); 
  glutSpecialFunc(NULL);
  glutMouseFunc(Mouse);
  glutMotionFunc(MouseMotion);
  InitArray(steps);
  InitOsc();
  glutMainLoop();
  return 0;        
}
