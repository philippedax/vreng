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
#include "vreng.hpp"
#include "nsl.hpp"

#ifndef AF_INET6
#define AF_INET6 26
#endif

extern "C" {	// MacOSX doesn't declare these functions
extern int inet_pton(int, const char *, void *);
}


struct hostent * my_getipnodebyname(const char *hostname, int af)
{
#if HAVE_GETIPNODEBYNAME
  int err;

  struct hostent *hp = getipnodebyname(hostname, af, AI_DEFAULT, &err);
  return hp;
#else //!HAVE_GETIPNODEBYNAME
  return NULL;
#endif
}

struct hostent * my_gethostbyname(const char *hostname, int af)
{
#if HAVE_GETIPNODEBYNAME
  return my_getipnodebyname(hostname, af);
#else //!HAVE_GETIPNODEBYNAME

  struct hostent *hptmp, *hp;

  if ((hptmp = gethostbyname(hostname)) == NULL) return NULL;

  if ((hp =  static_cast<struct hostent *>)malloc(sizeof(struct hostent)))) != NULL)
    memcpy(hp, hptmp, sizeof(struct hostent));
  return hp;
#endif
}

struct hostent * my_gethostbyname_r(const char *hostname, int af)
{
#if HAVE_GETIPNODEBYNAME
  struct hostent *hp = NULL;
  hp = my_getipnodebyname(hostname, af);
  return hp;
#else //!HAVE_GETIPNODEBYNAME

  struct hostent *hptmp = NULL, *hp;

#if HAVE_GETHOSTBYNAME_R
  struct hostent result;
  int err;
  char buf[512];

#if defined(LINUX) || defined(HPUX)
  gethostbyname_r(hostname, (struct hostent *) &result, buf, sizeof(buf), (struct hostent **) &hptmp, &err);
#else
  hptmp = gethostbyname_r(hostname, &result, buf, sizeof(buf), &err);
#endif

#else //!HAVE_GETHOSTBYADDR_R
  hptmp = gethostbyname(hostname);
#endif

  if (! hptmp) return NULL;
  if ((hp =  static_cast<struct hostent *>(malloc(sizeof(struct hostent)))) != NULL)
    memcpy(hp, hptmp, sizeof(struct hostent));
  return hp;
#endif
}

struct hostent * my_getipnodebyaddr(const char *addr, int af)
{
#if HAVE_GETIPNODEBYADDR
  int err;

  unsigned int len = (af == AF_INET) ? 4 : 16;
  struct hostent *hp = getipnodebyaddr(addr, len, af, &err);
  return hp;
#else //!HAVE_GETIPNODEBYADDR
  return NULL;
#endif
}

struct hostent * my_gethostbyaddr(const char *addr, int af)
{
#if HAVE_GETIPNODEBYADDR
  return my_getipnodebyaddr(addr, af);
#else //!HAVE_GETIPNODEBYADDR

  struct hostent *hptmp, *hp;

  if ((hptmp = gethostbyaddr(addr, sizeof(struct in_addr), af)) == NULL) return NULL;
  if ((hp =  static_cast<struct hostent *>(malloc(sizeof(struct hostent)))) != NULL)
    memcpy(hp, hptmp, sizeof(struct hostent));
  return hp;
#endif
}

struct hostent * my_gethostbyaddr_r(const char *addr, int af)
{
#if HAVE_GETIPNODEBYADDR
  return my_getipnodebyaddr(addr, af);
#else //!HAVE_GETIPNODEBYADDR

  struct hostent *hptmp = NULL, *hp;

#if HAVE_GETHOSTBYADDR_R
  struct hostent result;
  int err;
  char buf[512];

#if defined(LINUX) || defined(HPUX)
  gethostbyaddr_r(addr, sizeof(struct in_addr), af, (struct hostent *) &result, buf, sizeof(buf), (struct hostent **) &hptmp, &err);
#else
  hptmp = gethostbyaddr_r(addr, sizeof(struct in_addr), af, &result, buf, sizeof(buf), &err);
#endif

#else //!HAVE_GETHOSTBYADDR_R
  hptmp = gethostbyaddr(addr, sizeof(struct in_addr), af);
#endif

  if (! hptmp) return NULL;
  if ((hp =  static_cast<struct hostent *>(malloc(sizeof(struct hostent)))) != NULL)
    memcpy(hp, hptmp, sizeof(struct hostent));
  return hp;
#endif
}

#if 0 //notused
struct servent * my_getservbyname(const char *service)
{
  struct servent *sptmp, *sp;

  if ((sptmp = getservbyname(service, NULL)) == NULL) return NULL;
  if ((sp =  static_cast<struct servent *>(malloc(sizeof(struct servent)))) != NULL)
    memcpy(sp, sptmp, sizeof(struct servent));
  return sp;
}

struct servent * my_getservbyname_r(const char *service)
{
  struct servent *sptmp = NULL, *sp;

#if HAVE_GETSERVBYNAME_R
  struct servent s_r;
  char buf[512];
#if defined(LINUX) || defined(HPUX)
  getservbyname_r(service, NULL, &s_r, buf, sizeof(buf), (struct servent **) &sptmp);
#else
  sptmp = getservbyname_r(service, NULL, &s_r, buf, sizeof(buf));
#endif

#else //!HAVE_GETSERVBYNAME_R
  sptmp = getservbyname(service, NULL);
#endif

  if (! sptmp) return NULL;
  if ((sp =  static_cast<struct servent *>(malloc(sizeof(struct servent)))) != NULL)
    memcpy(sp, sptmp, sizeof(struct servent));
  return sp;
}
#endif //notused

void my_free_hostent(struct hostent *hp)
{
#if HAVE_FREEHOSTENT
  if (hp) freehostent(hp);
#else
    free(hp);
#endif
}

/** inet_ntop, inet_ntoa */
const char * my_inet_ntop(int af, const void *addr)
{
#if HAVE_INET_NTOP
  char dst[MAXHOSTNAMELEN];

  return inet_ntop(af, addr, dst, sizeof(dst));
#else
  return inet_ntoa((struct in_addr &) addr);
#endif
}

/** inet_pton, inet_addr */
int my_inet_pton(int af, const char *name, void *addr)
{
#if HAVE_INET_PTON
  return inet_pton(af, name, addr);
#else
  *((int *)addr) = inet_addr(name);
  return (int) addr;
#endif
}

const char * inet4_ntop(const void *addr)
{
  return my_inet_ntop(AF_INET, addr);
}

int inet4_pton(const char *name, void *addr)
{
  return my_inet_pton(AF_INET, name, addr);
}

const char * inet6_ntop(const void *addr)
{
  return my_inet_ntop(AF_INET6, addr);
}

int inet6_pton(const char *name, void *addr)
{
  return my_inet_pton(AF_INET6, name, addr);
}
