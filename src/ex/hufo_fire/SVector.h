//-------------------------------------------------------------------
// SVector.h - A vector math library.
// By: Aaron Hilton (c) 1/21/98
//-------------------------------------------------------------------
#ifndef __SVECTOR_H_
#define __SVECTOR_H_

//-------------------------------------------------------------------
// Pseudo-constantes:
#define EPSILON  	1e-8
#define ZERO  	 	EPSILON
#define M_1_MAXINT 	4.65661287524579692410E-10	//--->  1/(2^31-1) 1/meu maxint 32bits
#define M_1_255		(1.0 / 255.0)
#define M_1_65535   (1.0 / 65535.0)
#define M_1_65500   (1.0 / 65500.0)

// 3DFX Glide requires triangles to snap its coordinates to this precision.
#define SNAP_BIAS	(Float)(1<<19)

//-------------------------------------------
// Forward declaration.

// Actual Vector struct.
class SVector3D
{
public:
	SVector3D() {};
	SVector3D( Float tx, Float ty, Float tz )
	{ x = tx; y = ty; z = tz; w = 1.f; }

	SVector3D( Float tx, Float ty, Float tz, Float tw )
	{ x = tx; y = ty; z = tz; w = tw; }

	union {
		Float x;	// Coorinate
		Float nx;	// Normal
	};

	union {
		Float y;
		Float ny;
	};

	union {
		Float z;
		Float nz;
	};

	Float w;  // This coordinate is only used for the final transformation process. Not used by any of the vector methods.

	//-------------------------------------------
	// General utility functions.
	void Print(char *s = Null );	// Prints the vector's coordinates to the "stderr" stream.
	void Rand();					// Randomly generates a new vector.
	inline Float DotProduct( const SVector3D &other ) const;	// DotProduct of this vector into the other vector.
	inline void	 CrossProduct( const SVector3D &a, const SVector3D &b );  // Calculates the cross product of "a" and "b", then storing it in this vector.
	inline Float Length();	// Returns the length of this vector.

	//-------------------------------------------
	// Results of these opperations are contained in "this" vector by default.
	inline Float Normalize(); // Normalizes this vector, and returns the scalar value used to normalize the vector.
	inline void	Zero( ); 		// Reset variables to a normal zero constant. (w = 1.f)
	inline void	Scale( Float s );  // Scale this vector by "s".
	inline void	Add( const SVector3D &other );	// Add the other vector to this vector.
	inline void	Subtract( const SVector3D &other );	// Subtract the other vector from this vector. (result is contained in this vector.)
	inline void	Combine( const SVector3D &other, Float s=1.f ); // Combine vectors with a scalar quantity.
	inline void	Lerp( const SVector3D &a, const SVector3D &b, Float fPercent );	// Linear Interpolate
	inline void	LerpW( const SVector3D &a, const SVector3D &b, Float fPercent );// Linear Interpolate with W
	inline void	RotV( const SVector3D &e, const SVector3D &p ); // Rotate this vector to the normal of e.
	inline void GetAngles( Float &fPan, Float &fPitch );		// Retrieve the angles of this vector.

	// Assignment opperator.
	const SVector3D& operator=( const SVector3D &point )
	{ x = point.x; y = point.y; z = point.z; w = point.w; return *this; }

	// Math opperations.
	// Unary.
	friend SVector3D operator+( const SVector3D& ptSrc1, const SVector3D& ptSrc2 );
	friend SVector3D operator-( const SVector3D& ptSrc1, const SVector3D& ptSrc2 );

	// Binary.
	const SVector3D& operator+=( const SVector3D& ptSrc )
	{ Add( ptSrc ); return *this; }

	const SVector3D& operator-=( const SVector3D& ptSrc )
	{ Subtract( ptSrc ); return *this; }

	//-------------------------------------------
	// Matrix transformation process.
//	void	Transform( const SVector3D &v, const SMatrix &a );
};

//------------------------------------------------------------------------
// Inline implementation.

//-------------------------------------------
// General utility functions.

// DotProduct of this vector into the other vector.
Float SVector3D::DotProduct( const SVector3D &other ) const
{
	return(x * other.x + y * other.y + z * other.z);
}

// Calculates the cross product of "a" and "b", then storing it in this vector.
void SVector3D::CrossProduct( const SVector3D &a, const SVector3D &b )
{
	x = a.y * b.z - b.y * a.z;
	y = b.x * a.z - a.x * b.z;
	z = a.x * b.y - b.x * a.y;
}

// Returns the length of this vector.
Float SVector3D::Length()
{
	return  (Float) sqrt( sqr(x) + sqr(y) + sqr(z) );
}

//------------------------------------------------------------------------
// Results of these opperations are contained in "this" vector by default.

//-------------------------------------------
// Normalizes this vector, and returns the scalar value used to normalize the vector.
Float SVector3D::Normalize()
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

void SVector3D::Zero( )
{
	x = y = z = 0.f;
	w = 1.f;
}

// Scale this vector by "s".
void SVector3D::Scale( Float s )
{
	x *= s;
	y *= s;
	z *= s;
}

// Add the other vector to this vector.
void SVector3D::Add( const SVector3D &other )
{
	x += other.x;
	y += other.y;
	z += other.z;
}

// Subtract the other vector from this vector.
void SVector3D::Subtract( const SVector3D &other )
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
}

// Combine vectors with a scalar quantity.
void SVector3D::Combine( const SVector3D &other, Float s)
{
  x += s * other.x;
  y += s * other.y;
  z += s * other.z;
}

// Linear Interpolate
void SVector3D::Lerp( const SVector3D &a, const SVector3D &b, Float fPercent )
{
	x = a.x*(1.f-fPercent) + b.x*fPercent;
	y = a.y*(1.f-fPercent) + b.y*fPercent;
	z = a.z*(1.f-fPercent) + b.z*fPercent;
}

// Linear Interpolate with W
void SVector3D::LerpW( const SVector3D &a, const SVector3D &b, Float fPercent )
{
	x = a.x*(1.f-fPercent) + b.x*fPercent;
	y = a.y*(1.f-fPercent) + b.y*fPercent;
	z = a.z*(1.f-fPercent) + b.z*fPercent;
	w = a.w*(1.f-fPercent) + b.w*fPercent;
}

// Rotate a quaternion vector ("p") around a normal ("e").
void SVector3D::RotV( const SVector3D &e, const SVector3D &p )
{
	Float m = (Float)sqrt( sqr( p.y ) + sqr( p.z ) ) ;
	Float im = 1.0f / m ;

	x = m * e.x + p.x * e.z ;
	y = ( -p.x * p.y * e.x  + p.z * e.y ) * im + p.y * e.z ;
	z = ( -p.x * p.z * e.x  - p.y * e.y ) * im + p.z * e.z ;
}

// Retrieve the angles of this vector.
void SVector3D::GetAngles( Float &fPan, Float &fPitch )
{
//	fPan = -(Float)atan2( x, z );
//	fPitch = 0;//(Float)atan2( sin(fPan)*z + cos(fPan)*x, y);
	double dx, dy, dz;
	dx = (double)x;
	dy = (double)y;
	dz = (double)z;
	if(dz > 0.0001f)
	{
		fPan = (Float) -atan(dx/dz);
		dz /= cos((double)fPan);
		fPitch = (Float) atan(dy/dz);
	} else if(dz < -0.0001f)
	{
		fPan = (Float) (PI-atan(dx/dz));
		dz /= cos((double)fPan-PI);
		fPitch = (Float) -atan(dy/dz);
	} else
	{
		fPan = 0.0;
		fPitch = (Float)-PI/2.f;
	}
}

// Math opperations.
inline SVector3D operator+( const SVector3D& ptSrc1, const SVector3D& ptSrc2 )
{
	SVector3D ptTmp(ptSrc1);
	ptTmp.Add( ptSrc2 );
	return ptTmp;
}

inline SVector3D operator-( const SVector3D& ptSrc1, const SVector3D& ptSrc2 )
{
	SVector3D ptTmp(ptSrc1);
	ptTmp.Subtract( ptSrc2 );
	return ptTmp;
}

//-------------------------------------------------------------------
// Extra tidbits...

//-------------------------------------------
// Calculates the equation of "b" to the power of "e".
Int	IPower( Int b, Int e );

#endif // __SVECTOR_H_
