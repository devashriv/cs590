#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <memory.h>

class CRect {
public:

	CRect(void);
	
	CRect(Vect3d v1, Vect3d v2, Vect3d v3, Vect3d v4)
	{
		a = v1; b = v2; c = v3; d = v4;
	};

	CRect(const CRect *newv);

	CRect(const CRect &newv);

	inline void Set(Vect3d v1, Vect3d v2, Vect3d v3, Vect3d v4) {
		a = v1;
		b = v2;
		c = v3;
		d = v4;
	}
	Vect3d a, b, c, d;
};


class TriangleC
{
public:
	//constructors
	TriangleC(void);

	TriangleC(Vect3d aa, Vect3d bb, Vect3d cc)
	{
		a = aa; b = bb; c = cc;
	};

	TriangleC(const TriangleC *newv);

	TriangleC(const TriangleC &newv);

	//destructor
	~TriangleC() {}	//empty

	inline void Set(Vect3d aa, Vect3d bb, Vect3d cc){
		a=aa;
		b=bb;
		c=cc;
	}

/*
	//Accessors kept for compatibility
	inline void SetX(float x) {v[0]=x;}
	inline void SetY(float y) {v[1]=y;}
	inline void SetZ(float z) {v[2]=z;}

	float GetX() const {return v[0];}	//public accessor functions
	float GetY() const {return v[1];}	//inline, const
	float GetZ() const {return v[2];}

	float x() const {return v[0];}	//public accessor functions
	float y() const {return v[1];}	//inline, const
	float z() const {return v[2];}


	void Zero(void);

	void One(void);
*/
//protected:
	//member variables
	Vect3d a,b,c;
};

#endif	//TriangleC_H