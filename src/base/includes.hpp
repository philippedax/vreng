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
// includes.hpp
//---------------------------------------------------------------------------
#ifndef INCLUDES_HPP
#define INCLUDES_HPP


#if HAVE_CONFIG_H
# include "../../config.h"
#endif

#include <stdio.h>

#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#if STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# if HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif

#if HAVE_STRING_H
# include <string.h>
#endif

#if HAVE_STRINGS_H
# include <strings.h>
#endif

#if HAVE_INTTYPES_H
# include <inttypes.h>
#endif

#if HAVE_STDINT_H
# include <stdint.h>
#endif

#if HAVE_CTYPE_H
# include <ctype.h>
#endif

#if HAVE_STDARG_H
# include <stdarg.h>
#else
# include <varargs.h>
#endif

#if HAVE_FCNTL_H
# include <fcntl.h>
#endif

#if HAVE_SIGNAL_H
# include <signal.h>
#endif

#if HAVE_MATH_H
# include <math.h>
#endif

#if HAVE_ERRNO_H
# include <errno.h>
#endif

#if HAVE_SETJMP_H
# include <setjmp.h>
#endif

#if HAVE_LOCALE_H
# include <locale.h>
#endif

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
#endif

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#if HAVE_UTIME_H
# include <utime.h>
#endif

#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

#if HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif

#if HAVE_SYS_POLL_H
# include <sys/poll.h>
#endif

#if HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif

#if HAVE_NETDB_H
# include <netdb.h>
#endif

#if HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif

#if HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif

#if HAVE_NETINET_TCP_H
# include <netinet/tcp.h>
#else
# define TCP_NODELAY 1
#endif

#if HAVE_INET_PTON
# ifndef HAVE_INET_NTOP
#  define HAVE_INET_NTOP 1
# endif
#endif

#if HAVE_NO_SOCKLEN_T
# define socklen_t int
#endif

#if HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif

#if HAVE_PWD_H
# include <pwd.h>
#endif

#if defined(HAVE_GETOPT_H) && defined(HAVE_GETOPT_LONG)
# include <getopt.h>
#endif

#if HAVE_SYS_RESOURCE_H
# include <sys/resource.h>
#endif

#if HAVE_SYS_UTSNAME_H
# include <sys/utsname.h>
#endif

#if HAVE_PTHREAD_H
# include <pthread.h>
#endif

#define GL_SILECE_DEPRECATION	// avoid warnings on apple
#include <GL/gl.h>

#if HAVE_GL_GLU_H
# include <GL/glu.h>
#endif
#if defined(HAVE_LIBGLU) && defined(HAVE_GL_GLU_H)
# define HAVE_GLU 1
#endif

#if HAVE_GLUT_GLUT_H
# include <glut/glut.h>
#endif
#if HAVE_GL_GLUT_H
# include <GL/glut.h>
#endif

#if HAVE_CURL_CURL_H
# include <curl/curl.h>
#endif

#if HAVE_XPM_H
# include <X11/xpm.h>
#endif

#if HAVE_MALLOC_H
# include <malloc.h>
#endif

#if HAVE_FLOATINGPOINT_H
# ifdef FREEBSD
#  include <floatingpoint.h>
# define HAVE_FPSETMASK 1
# endif
#endif

#ifndef MAXHOSTNAMELEN
# define MAXHOSTNAMELEN 64
#endif

#ifndef MAXFLOAT
# ifdef WIN32
#  include <float.h>
#  define MAXFLOAT FLT_MAX
# else
#  define MAXFLOAT 1E100
# endif
#endif

#ifdef SUNOS
# define FORMATPTR	"%x"
#else
# define FORMATPTR	"%p"
#endif

#if !defined(HAVE_DRAND48)
# define drand48() ((float)rand()/RAND_MAX)
#endif

#if defined(WIN32) && !defined(CYGWIN32)
# include "windoze.h"
#endif

#endif
