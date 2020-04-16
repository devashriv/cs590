/*********************************/
/* CS 590CGS Lab framework        */
/* (C) Bedrich Benes 2020        */
/* bbenes ~ at ~ purdue.edu      */
/*********************************/

/* Edits by Devashri                                                  */
/* Press                                                              */
/*       r to render random points                                    */
/*       b to render 3 segments of a bezier curve                     */
/*       +,- to add/remove points                                     */
/*       space to render a point passing from any two random points   */
/*       >, < to add/remove segments passing through random points    */


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


GLuint points=0;    //number of points to display the object
int steps=20;       //# of subdivisions for curves
int seg = 1;        //no. of segments of approximate curve at start
int countSpace = 1; //counts no of times space is pressed
bool needRedisplay=false;
GLfloat  sign=+1;   //diretcion of rotation
const GLfloat defaultIncrement=0.7f; //speed of rotation
GLfloat  angleIncrement=defaultIncrement;

vector <Vect3d> r;    //all random points will be stored here
vector <Vect3d> bez;  //stores bezier control points
vector <Vect3d> bez1; //stores bezier1 smoothening points
vector <Vect3d> bez2; //stores bezier2 smoothening points
vector <Vect3d> bez3; //stores bezier3 smoothening points
vector <Vect3d> cp;   //stores all sontrol points of approximate curve
vector <Vect3d> app;  //stores points of approximate curve


float c11 = rand() % 10 * 0.2f; // random vector to ensure c1 at first knot
Vect3d addC11(c11, c11, c11);

float c12 = rand() % 10 * 0.2f; // random vector to ensure c1 at second knot
Vect3d addC12(c12, c12, c12);

//window size
GLint wWindow=1200;
GLint hWindow=800;

bool RandomPointsFlag = false; //random points default off
bool BezierCPFlag = false;  //bezier control points off
bool BezierPointFlag = false;  //bezier points default off
bool BezierCurveFlag = false;  //bezier curve default off
bool CPFlag = false;           //control points for approximate curve default hidden
bool AppPointFlag = false;     //approximate curve points default hide
bool AppCurveFlag = false; //approximate curve default off


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

/**********************
LAB related MODIFY
***********************/


//part1
//creates a random points
inline Vect3d RandomVector(void) {
	return Vect3d(rand()%10*0.2f,rand()%10*0.2f, rand()%10 * 0.2f);
}

//fills a vector array with random vectors
void Randomize(vector <Vect3d>* a, int n)
{	for (int i =0; i <n;i++)
	a ->push_back(RandomVector());
}

//initialize random point array
void InitRandomPoints(int n)
{
	r.clear();
	Randomize(&r, n);
}



//part 2
inline Vect3d C(int i, vector <Vect3d>* a)
{
	float t = (float)i / steps;
	float t1 = 1 - (float)i / steps;
	return (t1 * t1 * t1 * a->at(0) + 3 * t * t1 * t1 * a->at(1) + 3 * t * t * t1 * a->at(2) + t * t * t * a->at(3));
}

inline Vect3d D(int i, vector <Vect3d>* a)
{
	float t = (float)i / steps;
	float t1 = 1 - (float)i / steps;
	return (t1 * t1 * t1 * a->at(3) + 3 * t * t1 * t1 * a->at(4) + 3 * t * t * t1 * a->at(5) + t * t * t * a->at(6));
}

inline Vect3d E(int i, vector <Vect3d>* a)
{
	float t = (float)i / steps;
	float t1 = 1 - (float)i / steps;
	return (t1 * t1 * t1 * a->at(6) + 3 * t * t1 * t1 * a->at(7) + 3 * t * t * t1 * a->at(8) + t * t * t * a->at(9));
}

void CreateBezierCurve(vector <Vect3d>* a, vector <Vect3d>* b, vector <Vect3d>* c, vector <Vect3d>* d, int n)
{

	for (int i = 0; i < steps; i++)
	{
		b->push_back(C(i, a));
	}

	for (int i = 0; i < steps; i++)
	{
		c->push_back(D(i, a));
	}
	for (int i = 0; i < steps; i++)
	{
		d->push_back(E(i, a));
	}
}

inline Vect3d Bezier(int i, vector <Vect3d>* a)
{
	switch (i)
	{
	case 3: {
		return Vect3d(a->at(2) + addC11);
		break;
	}
	case 4: {
		return Vect3d(a->at(2) + 2 * addC11);
		break;
	}
	case 6: {
		return Vect3d(a->at(5) + addC12);
		break;
	}
	case 7: {
		return Vect3d(a->at(5) + 2 * addC12);
		break;
	}
		  break;
	}

	return RandomVector();
}

void CreateBezierPoints(vector <Vect3d>* a, int n)
{
	for (int i = 0; i < n; i++)
	{
		a->push_back(Bezier(i, a));
	}
}

void InitBezierCurve(int n)
{
	bez1.clear();
	bez2.clear();
	bez3.clear();
	CreateBezierCurve(&bez, &bez1, &bez2, &bez3, n);
}

void InitBezier(int n)
{
	bez.clear();
	CreateBezierPoints(&bez, n);
	InitBezierCurve(n);
}


//part3
inline Vect3d ControlPoints(int i,vector <Vect3d> *b, vector <Vect3d> *a)
{
	if (i%3 == 0)
		return Vect3d(b->at(i/3));
	else if (i==1)
		return Vect3d(RandomVector());
	else if (i % 3 == 1)
		return Vect3d(a->at(i - 1) + (a->at(i - 1) - a->at(i - 2)));
		return Vect3d(RandomVector());
}

void CreateCP(vector <Vect3d>* a)
{
	for (int i = 0; i <(r.size() - 1) * 4 - (r.size() - 2)+1 ; i++)
		a->push_back(ControlPoints(i,&r,a));
}

void InitCP()
{
	cp.clear();
	CreateCP(&cp);
}

inline Vect3d Poly(int i,int j,vector <Vect3d> *b)
{
	float t = (float)i / steps;
	float t1 = 1 - (float)i / steps;
	return (t1 * t1 * t1 * b->at(j) + 3 * t * t1 * t1 * b->at(j + 1) + 3 * t * t * t1 * b->at(j + 2) + t * t * t * b->at(j + 3));
}

void CreateAppCurve(vector <Vect3d>* a,int seg)
{
	for (int j = 0; j < seg*4-(seg-1)-1; j += 3)
		for (int i = 0; i < steps+1; i++)
			a->push_back(Poly(i,j,&cp));
}

void InitAppCurve(int seg)
{
	app.clear();
	CreateAppCurve(&app,seg);
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
void Lab02() {
	Vect3d a,b,c;
	Vect3d origin(0, 0, 0);
	Vect3d red(1, 0, 0), green(0, 1, 0), blue(0, 0, 1), almostBlack(0.1f, 0.1f, 0.1f), yellow(1, 1, 0);

	CoordSyst();
	//draw the curve
	if (RandomPointsFlag)
		for (unsigned int i = 0; i < r.size(); i++) {
			DrawPoint(r[i], yellow);
	}

	//draw Bezier control points
	if (BezierCPFlag)
	{
		for (unsigned int i = 0; i < bez.size(); i++) {
			DrawPoint(bez[i], green);
		}
	}
	//draw Bezier curve points
	if (BezierPointFlag)
	{
		for (unsigned int i = 0; i < bez1.size(); i++)
			DrawPoint(bez1[i], blue);
		for (unsigned int i = 0; i < bez2.size(); i++)
			DrawPoint(bez2[i], blue);
		for (unsigned int i = 0; i < bez3.size(); i++)
			DrawPoint(bez3[i], blue);
	}
	//draw Bezier curve
	if (BezierCurveFlag)
	{
		for (unsigned int i = 1; i < bez1.size(); i++) {
			DrawLine(bez1[i], bez1[i - 1], red);
		}
		DrawLine(bez1[bez1.size() - 1], bez2[0], red);
		for (unsigned int i = 1; i < bez2.size(); i++) {
			DrawLine(bez2[i], bez2[i - 1], green);
		}
		DrawLine(bez2[bez2.size() - 1], bez3[0], green);
		for (unsigned int i = 1; i < bez3.size(); i++) {
			DrawLine(bez3[i], bez3[i - 1], blue);
		}
	}
	//draw approximate curve control points
	if (CPFlag)
	{
		for (unsigned int i = 0; i < cp.size(); i++) {
			DrawPoint(cp[i], green);
		}
	}
	//draw approximate curve points
	if (AppPointFlag)
	{
		for (unsigned int i = 0; i < app.size(); i++) {
			DrawPoint(app[i], red);
		}
	}
	//draw approximate curve
	if (AppCurveFlag)
	{
		for (unsigned int i = 0; i < app.size()-1; i++) {
			DrawLine(app[i], app[i + 1], red);
		}
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
	Lab02();
}

//Add here if you want to control some global behavior
//see the pointFlag and how is it used
void Kbd(unsigned char a, int x, int y)//keyboard  callback
{
	switch (a)
	{
	case 27: exit(0); break;	
	case 'x': {
		if (angleIncrement == 0) angleIncrement = defaultIncrement;
		else angleIncrement = 0;
		break;
	}
	case 's': {sign = -sign; break; }
	
	case 'r': {
		RandomPointsFlag = !RandomPointsFlag;
		break;
	}
	case 'p': BezierCPFlag = !BezierCPFlag; break;

	case 'b': {
		BezierPointFlag = !BezierPointFlag;
		break;
	}
	case 'c': {
		BezierCurveFlag = !BezierCurveFlag;
		break;
	}
	case '+': {
		steps = steps++;
		InitBezierCurve(10);
		break;
	}
	case '-': {
		steps = steps--;
		InitBezierCurve(10);
		break;
	}
	case ' ': {
		AppCurveFlag = !AppCurveFlag;
		break;
	}
	case 'q': {
		CPFlag = !CPFlag;
		break;
	}
	case '>': {
		if (seg == 9)
			seg = 1;
		seg++;
		InitAppCurve(seg);
		break;
	}
	case '<': {
		seg--;
		InitAppCurve(seg);
		break;
	}
	case 'a': {
		AppPointFlag = !AppPointFlag;
		break;
	}

	}
	//cout << "[points]=[" << steps << "]" << endl;
	glutPostRedisplay();
}


/*******************
OpenGL code. Do not touch.
******************/
void Idle(void)
{
  glClearColor(0.5f,0.5f,0.5f,1); //background color 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  GLMessage("Lab 2 - CS 590CGS");
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
  InitRandomPoints(10);
  InitBezier(10);
  InitCP();
  InitAppCurve(seg);
  glutMainLoop();
  return 0;        
}
