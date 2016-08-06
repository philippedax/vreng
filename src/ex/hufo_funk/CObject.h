//-------------------------------------------------------------------
//	File:		CObject.h
//	Created:	01/12/99 0:24:AM
//	Author:		Allard Jeremie
//	Comments:	Create and manage Objects.
//-------------------------------------------------------------------
#ifndef __CObject_h_
#define __CObject_h_

#include "PTypes.h"	// Added by ClassView
#include "SVector.h"

//typedef SVector3D CPoint3D;
class CPoint3D
{
public:
	Float x,y,z;
	CPoint3D() :x(0.0f),y(0.0f),z(0.0f) {}
	CPoint3D( Float tx, Float ty, Float tz )
	:x(tx),y(ty),z(tz) {}
	//-------------------------------------------
	// General utility functions.
	inline Float DotProduct( const CPoint3D &other );	// DotProduct of this vector into the other vector.
	inline void	 CrossProduct( const CPoint3D &a, const CPoint3D &b );  // Calculates the cross product of "a" and "b", then storing it in this vector.
	inline Float Length();	// Returns the length of this vector.
	inline Float Length2();	// Returns the squared length of this vector.

	//-------------------------------------------
	// Results of these opperations are contained in "this" vector by default.
	inline Float Normalize(); // Normalizes this vector, and returns the scalar value used to normalize the vector.
	inline void	Zero( ); 		// Reset variables to a normal zero constant. (w = 1.f)
	inline void	Scale( Float s );  // Scale this vector by "s".
	inline void	Add( const CPoint3D &other );	// Add the other vector to this vector.
	inline void	Subtract( const CPoint3D &other );	// Subtract the other vector from this vector. (result is contained in this vector.)

	// Assignment opperator.
	const CPoint3D& operator=( const CPoint3D &point )
	{ x = point.x; y = point.y; z = point.z; return *this; }

	// Math opperations.
	// Unary.

	// Binary.

	friend CPoint3D operator+( const CPoint3D& ptSrc1, const CPoint3D& ptSrc2 );
	friend CPoint3D operator-( const CPoint3D& ptSrc1, const CPoint3D& ptSrc2 );
	friend CPoint3D operator*( const CPoint3D& pt, Float f );
	friend CPoint3D operator*( Float f, const CPoint3D& pt );

	const CPoint3D& operator+=( const CPoint3D& ptSrc )
	{ Add( ptSrc ); return *this; }

	const CPoint3D& operator-=( const CPoint3D& ptSrc )
	{ Subtract( ptSrc ); return *this; }

	const CPoint3D& operator*=( Float f )
	{ Scale( f ); return *this; }

};

#define PUT_COORD  1
#define PUT_NORMAL 2

class CColor4f
{
public:
	void Put();
	union
	{
		struct
		{
			float r,g,b,a;
		};
		float t[4];
	};
	CColor4f()
	: r(0.0f),g(0.0f),b(0.0f),a(1.0f)
	{}
	CColor4f(float rr,float gg=0.0f,float bb=0.0f,float aa=1.0f)
	: r(rr),g(gg),b(bb),a(aa)
	{}
	CColor4f operator *(float f)
	{
		return CColor4f(r*f,g*f,b*f,a);
	}
};

class CVertex
{
public:
	CPoint3D n;
	CPoint3D p;
	void Put(int pdata);
};

class CMaterial
{
public:
	int Put();
	char name[16];
	CColor4f ambient;
	CColor4f diffuse;
	CColor4f specular;
	CColor4f emission;
	int shininess;
	int flag;
	enum
	{
		Smooth=1,
		Double=2,
		Lighting=4,
		Blend=8,
		Additive=16
	};
	CMaterial()
	: ambient (0.2f,0.2f,0.2f)
	, diffuse (0.8f,0.8f,0.8f)
	, specular(0.0f,0.0f,0.0f)
	, emission(0.0f,0.0f,0.0f)
	, shininess(0)
	, flag(Smooth)
	{}
};

class CTriFace
{
public:
	void CalcNormal();
	void Put(int pdata);
	void Aff(int pdata);
	CVertex *v[3];
	CPoint3D n;
	CMaterial *m;
};

class CTriStrip
{
public:
	int n;
	CVertex **v;
};

class CSurface : public CMaterial
{
public:
	void AffStrip(CVertex *p0=NULL);
	void Aff(CVertex *p0=NULL);
	//CMaterial m;
	int nbf;
	CTriFace *fc;
	int nbts; // nb triangle strip
	CTriStrip *ts; // triangle strip
	CSurface() : fc(NULL), nbf(0), ts(NULL), nbts(0) {}
	Bool GetStrip(char *fname,CVertex *p0, int np);
};

class CObject
{
public:
	Bool GetStrip(char *fname);
	void Aff();
	void Free();
	CObject();
	~CObject();
	bool LoadLWO(char* szFileName);
	void AffStrip();

	CSurface * FindSurface(char *name);

	void Flip();

	int nbp,nbf,nbs;
	CVertex *pt;
	CTriFace *fc;
	CSurface *sf;
};

//------------------------------------------------------------------------
// Inline implementation.
//-------------------------------------------
// General utility functions.
// DotProduct of this vector into the other vector.
Float CPoint3D::DotProduct( const CPoint3D &other )
{
	return(x * other.x + y * other.y + z * other.z);
}

// Calculates the cross product of "a" and "b", then storing it in this vector.
void CPoint3D::CrossProduct( const CPoint3D &a, const CPoint3D &b )
{
	x = a.y * b.z - b.y * a.z;
	y = b.x * a.z - a.x * b.z;
	z = a.x * b.y - b.x * a.y;
}

// Returns the length of this vector.
Float CPoint3D::Length()
{
	return  (Float) sqrt( sqr(x) + sqr(y) + sqr(z) );
}

// Returns the squared length of this vector.
Float CPoint3D::Length2()
{
	return sqr(x) + sqr(y) + sqr(z);
}

//------------------------------------------------------------------------
// Results of these opperations are contained in "this" vector by default.

//-------------------------------------------
// Normalizes this vector, and returns the scalar value used to normalize the vector.
Float CPoint3D::Normalize()
{
	Float n, nn ;

	nn = sqr(x) + sqr(y) + sqr(z);
	if ( nn < ZERO ) return 0.0f;

	nn = (Float) sqrt(nn);
	n = 1.0f / nn;
	x *= n;
	y *= n;
	z *= n;
	return nn;
}

void CPoint3D::Zero( )
{
	x = y = z = 0.f;
}

// Scale this vector by "s".
void CPoint3D::Scale( Float s )
{
	x *= s;
	y *= s;
	z *= s;
}

// Add the other vector to this vector.
void CPoint3D::Add( const CPoint3D &other )
{
	x += other.x;
	y += other.y;
	z += other.z;
}

// Subtract the other vector from this vector.
void CPoint3D::Subtract( const CPoint3D &other )
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
}
/*
// Combine vectors with a scalar quantity.
void CPoint3D::Combine( const CPoint3D &other, Float s)
{
  x += s * other.x;
  y += s * other.y;
  z += s * other.z;
}
*/
// Math opperations.
inline CPoint3D operator+( const CPoint3D& p1, const CPoint3D& p2 )
{
	return CPoint3D(p1.x+p2.x,p1.y+p2.y,p1.z+p2.z);
	/*CPoint3D ptTmp(ptSrc1);
	ptTmp.Add( ptSrc2 );
	return ptTmp;*/
}

inline CPoint3D operator-( const CPoint3D& p1, const CPoint3D& p2 )
{
	return CPoint3D(p1.x-p2.x,p1.y-p2.y,p1.z-p2.z);
	/*CPoint3D ptTmp(p1);
	ptTmp.Subtract( p2 );
	return ptTmp;*/
}
inline CPoint3D operator*( const CPoint3D& pt, Float f )
{
	return CPoint3D(pt.x*f,pt.y*f,pt.z*f);
}
inline CPoint3D operator*( Float f, const CPoint3D& pt )
{
	return CPoint3D(pt.x*f,pt.y*f,pt.z*f);
}

#endif // __CObject_h_
//-------------------------------------------------------------------
//	History:
//	$Log:$
//-------------------------------------------------------------------
