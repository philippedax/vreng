//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
// Telecom-ParisTech (Ecole Nationale Superieure des Telecommunications)
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
// windoze.cpp
//
// Windows specific
//---------------------------------------------------------------------------
#include "vreng.hpp"

#if defined(WIN32) && !defined(CYGWIN32)


static WORD wVersionRequested;
static WSADATA wsaData;


/** For Windoze, we need to implement our own gettimeofday() */
int gettimeofday(struct timeval* tv, int* tz)
{
  struct _timeb tb;

  _ftime(&tb);
  tv->tv_sec = tb.time;
  tv->tv_usec = ((long) tb.millitm) * 1000;
  return 0;
}

/** init dll for the sockets */
void initDll()
{
  wVersionRequested = MAKEWORD(2, 0);
  if (WSAStartup(wVersionRequested, &wsaData) != 0) {
    fprintf(stderr, "Can't find a usable WinSock DLL\n");
    exit(1);
  }
}

#endif
