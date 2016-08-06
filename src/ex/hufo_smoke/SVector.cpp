//-------------------------------------------------------------------
// SVector.cpp - A vector math library.
// By: Aaron Hilton (c) 1/21/98
//-------------------------------------------------------------------
#include "stdafx.h"
#include "SVector.h"

//-------------------------------------------
// General utility functions.

//-------------------------------------------
// Prints the vector's coordinates to the "stderr" stream.
void SVector3D::Print(char *s )
{
	// If we have an informative string, then print it.
	if(s)
		fprintf(stderr, "%s\n", s);

	// Print the vector info.
	fprintf(stderr,">%12g\t%12g\t%12g\t%12g\n", (double)x, (double)y, (double)z, (double)w );
}

// Randomly generates a new vector.
void SVector3D::Rand()
{
	x = 1.f - 2.f * rnd();
	y = 1.f - 2.f * rnd();
	z = 1.f - 2.f * rnd();
}

//-------------------------------------------
// Calculates the equation of "b" to the power of "e".
Int	IPower( Int b, Int e )
{
	Int result = 1 ;

	// Loop the multiplication process for each exponent.
	while( e-- )
		result *= b ;

	return result;
}
