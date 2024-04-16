//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
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
// windoze.hpp
//---------------------------------------------------------------------------
#ifndef WINDOZE_HPP
#define WINDOZE_HPP


#include <winsock2.h>
#include <time.h>
#include <sys\timeb.h>
#include <resolv.h>
#include <io.h>
#include <arpa\nameser.h>

#define	pid_t	int
#define open _open
#define close _close
#define read _read
#define write _write
#define access _access
#define lseek _lseek
#define	getpid	_getpid
#define srandom srand
#define random rand
#define strncasecmp _strnicmp
#define strcasecmp _stricmp

#ifndef drand48()
#define drand48() ((float)rand()/RAND_MAX)
#endif

#define F_OK 0
#ifdef _O_RDONLY
#ifndef O_RDONLY
#define O_RDONLY _O_RDONLY
#endif
#endif
#define IOSBINARY ios::binary

#ifdef VISUAL_C
#define inline __inline
#endif

#define XtPointer int

int gettimeofday(struct timeval* tv, int* tz);
void initDll(void);


#endif
