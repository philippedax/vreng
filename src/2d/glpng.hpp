//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
// Telecom-Paris (Ecole Nationale Superieure des Telecommunications)
//
// VREng is a free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public Licence as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// VREng is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//---------------------------------------------------------------------------
// glpng.hpp
//---------------------------------------------------------------------------
#ifndef GLPNG_HPP
#define GLPNG_HPP

#include <stdio.h>	// FILE

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
	#ifdef _DEBUG
		#pragma comment (lib, "glpngd.lib")
	#else
		#pragma comment (lib, "glpng.lib")
	#endif
#endif

/* XXX This is from Win32's <windef.h> */
#ifndef APIENTRY
	#if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
		#define APIENTRY    __stdcall
	#else
		#define APIENTRY
	#endif
#endif

/* Mipmapping parameters */
#define PNG_NOMIPMAPS      0 /* No mipmapping                        */
#define PNG_BUILDMIPMAPS  -1 /* Calls a clone of gluBuild2DMipmaps() */
#define PNG_SIMPLEMIPMAPS -2 /* Generates mipmaps without filtering  */

/* Who needs an "S" anyway? */
#define PNG_NOMIPMAP     PNG_NOMIPMAPS
#define PNG_BUILDMIPMAP  PNG_BUILDMIPMAPS
#define PNG_SIMPLEMIPMAP PNG_SIMPLEMIPMAPS

/* Transparency parameters */
#define PNG_CALL_BACK -3 /* Call the callback function to generate alpha */
#define PNG_ALPHA     -2 /* Use alpha bpp in PNG file, if there is one   */
#define PNG_SOLID     -1 /* No transparency                              */
#define PNG_STENCIL    0 /* Sets alpha to 0 for r=g=b=0, 1 otherwise     */
#define PNG_BLEND1     1 /* a = r+g+b                                    */
#define PNG_BLEND2     2 /* a = (r+g+b)/2                                */
#define PNG_BLEND3     3 /* a = (r+g+b)/3                                */
#define PNG_BLEND4     4 /* a = r*r+g*g+b*b                              */
#define PNG_BLEND5     5 /* a = (r*r+g*g+b*b)/2                          */
#define PNG_BLEND6     6 /* a = (r*r+g*g+b*b)/3                          */
#define PNG_BLEND7     7 /* a = (r*r+g*g+b*b)/4                          */
#define PNG_BLEND8     8 /* a = sqrt(r*r+g*g+b*b)                        */

typedef unsigned char * png_bytep;

typedef struct {
	unsigned int Width;
	unsigned int Height;
	unsigned int Depth;
	unsigned int Alpha;
} pngInfo;

typedef struct {
	unsigned int Width;
	unsigned int Height;
	unsigned int Depth;
	unsigned int Alpha;

	unsigned int Components;
	unsigned char *Data;
	unsigned char *Palette;
} pngRawInfo;

extern int APIENTRY pngLoadF(FILE *file, int mipmap, int trans, pngRawInfo *info);

#ifdef __cplusplus
}
#endif

#endif
