//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
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
// thread.hpp
//---------------------------------------------------------------------------
#ifndef THREAD_HPP
#define THREAD_HPP

#if HAVE_LIBPTHREAD
typedef pthread_t Vpthread_t;
typedef pthread_cond_t Vpthread_cond_t;
typedef pthread_mutex_t Vpthread_mutex_t;
#else
typedef int Vpthread_t;
typedef int Vpthread_cond_t;
typedef int Vpthread_mutex_t;
#endif

void initMutex(Vpthread_mutex_t *pmutex);
void lockMutex(Vpthread_mutex_t *pmutex);
void unlockMutex(Vpthread_mutex_t *pmutex);

#endif
