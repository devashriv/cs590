#include <memory.h>
#include <math.h>
#include "math/vect3d.h"
#include "math/triangle.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


//constructors

TriangleC::TriangleC(void){
}

TriangleC::TriangleC(const TriangleC *newv){
	a = newv->a;
	b = newv->b;
	c = newv->c;
}

TriangleC::TriangleC(const TriangleC & newv){
	a = newv.a;
	b = newv.b;
	c = newv.c;

}

CRect::CRect(void) {
}

CRect::CRect(const CRect *newv) {
	a = newv->a;
	b = newv->b;
	c = newv->c;
	d = newv->d;
}

CRect::CRect(const CRect & newv) {
	a = newv.a;
	b = newv.b;
	c = newv.c;
	d = newv.d;
}
