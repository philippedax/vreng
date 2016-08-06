//-------------------------------------------------------------------
//	Created:	13/06/00 19:30:PM
//	Author:		Allard Jeremie (Hufo / N.A.A.)
//-------------------------------------------------------------------
#ifndef HAIR_HPP
#define HAIR_HPP

#include "WObject.hpp"

#define HAIRSIZE 8
#define NBHAIR 1000
#define HAIRLENGTH 4.0
#define HAIRRANDOM 1.0
#define HAIRNBLINE (NBHAIR*(HAIRSIZE-2))

class Node {
public:
	float p[3];
	float v[3];
	void Put(int pdata);
};

class Line {
public:
	float n[3];
	Node pt[HAIRSIZE];
	void Init(Point3D p0, Point3D dp);
	void Aff(int pdata);
};

class Hair {
public:
	Line ln[NBHAIR];
	Object *o;
	Surface *s;
	Material m;
	GLfloat color[4];
	void Init(Object *o, Surface *s);
	void Aff();
	void Update(float dt, Point3D Depl);
};

#endif
