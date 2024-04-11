//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
#ifndef MACROS_HPP
#define MACROS_HPP


#ifndef MIN
#define MIN(a,b)	((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b)	((a)<(b)?(b):(a))
#endif
#define ABSF(f)		((f)>0?(f):(-(f)))
#define SQR(x)		((x)*(x))		// Square
#define SGN(a)		((a)<0?-1:((a)?1:0))	// Force sign clamping to (-1;0;1)
#define NN(s)		((s)?(s):"")
#define CROP(a,x,b)	MIN(MAX(a,x),b)
#define SWAP(a,b,t)	{ t=a;a=b;b=t; }

/* byte swap a 32-bit value */
#define SWAPL(x, n)	{ \
                 	n = ((char *) (x))[0];\
                 	((char *) (x))[0] = ((char *) (x))[3];\
                 	((char *) (x))[3] = n;\
                 	n = ((char *) (x))[1];\
                 	((char *) (x))[1] = ((char *) (x))[2];\
                 	((char *) (x))[2] = n; }

/* byte swap a short */
#define SWAPS(x, n)	{ \
                 	n = ((char *) (x))[0];\
                 	((char *) (x))[0] = ((char *) (x))[1];\
                 	((char *) (x))[1] = n; }

#define FREE(x)		{ if (x != NULL) { free(x); x = NULL; } }
#define DELETE(x)	{ if (x != NULL) { delete x; x = NULL; } }
#define DELETE2(x)	{ if (x != NULL) { delete[] x; x = NULL; } }

#endif
