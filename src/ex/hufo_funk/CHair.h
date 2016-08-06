//-------------------------------------------------------------------
//	File:		CObject.h
//	Created:	13/06/00 19:30:PM
//	Author:		Allard Jeremie (Hufo / N.A.A.)
//	Comments:	Create and manage Hair.
//-------------------------------------------------------------------
#ifndef __CHair_h_
#define __CHair_h_

#include "PTypes.h"	// Added by ClassView
#include "SVector.h"
#include "CObject.h"

#define HAIRSIZE 8
#define NBHAIR 500
#define HAIRLENGTH 4.0f
#define HAIRRANDOM 1.0f
#define HAIRNCOLOR 4
#define HAIRNBLINE (NBHAIR*(HAIRSIZE-2))

class CNode
{
public:
	CPoint3D p;
	CPoint3D v;
	void Put(int pdata);
};

class CLine
{
public:
	CPoint3D n;
	CNode pt[HAIRSIZE];
	void Init(CPoint3D p0, CPoint3D dp);
	void Aff(int pdata);
};

class CHair
{
public:
	CLine ln[NBHAIR];
	CObject *o;
	CSurface *s;
	CMaterial m;
	CColor4f c[HAIRNCOLOR];
	void Init(CObject *o,CSurface *s);
	void Aff();
	void Update(float dt,CPoint3D Depl);
};

#endif