//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2024 Philippe Dax
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
// nsl.cpp
//
// DNS operations
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "nsl.hpp"
#include <vector>


/** my_gethostbyname */
struct hostent * my_gethostbyname(const char *hostname, int af)
{
//#if HAVE_GETADDRINFO
//  struct addrinfo hints, *result;
//
//  memset(&hints, 0, sizeof(hints));
//  hints.ai_family = PF_UNSPEC;
//  hints.ai_socktype = SOCK_STREAM;
//  hints.ai_flags = 0;
//
//  int ret = getaddrinfo(hostname, "http", &hints, &result);
//  if (ret) {
//    error("addrinfo: %d %s - %s", ret, gai_strerror(ret), hostname);
//    //dax /*NOTREACHED*/
//  }
//
//  std::vector<in_addr*> in_addrs;
//  for (addrinfo *p_addr = result; p_addr; p_addr = p_addr->ai_next) {
//    in_addrs.push_back(&reinterpret_cast<sockaddr_in*> (p_addr->ai_addr)->sin_addr);
//  }
//  in_addrs.push_back(NULL);
//
//  struct hostent *hp = static_cast<struct hostent *> (malloc(sizeof(struct hostent)));
//  //dax hp->h_name = result->ai_canonname;	// CRASH access ai !!! FIXME!
//  hp->h_aliases = NULL;
//  hp->h_addrtype = af;
//  hp->h_length = sizeof(in_addr);
//  hp->h_addr_list = reinterpret_cast<char**> (&in_addrs[0]);
//
//  freeaddrinfo(result);
//  return hp;
//#else //!HAVE_GETADDRINFO

#if HAVE_GETIPNODEBYNAME
  return my_getipnodebyname(hostname, af);
#else //!HAVE_GETIPNODEBYNAME
  return gethostbyname(hostname);
#endif
//#endif
}

/** my_gethostbyname_r */
struct hostent * my_gethostbyname_r(const char *hostname, int af)
{
#if HAVE_GETIPNODEBYNAME
  return my_getipnodebyname(hostname, af);
#else //!HAVE_GETIPNODEBYNAME
#if HAVE_GETHOSTBYNAME_R
  static struct hostent *hp;
  struct hostent ret;
  int err;
  char buf[512];

  if (gethostbyname_r(hostname, &ret, buf, sizeof(buf), &hp, &err) == 0)
    return hp;
  else
    return NULL;
#else //!HAVE_GETHOSTBYADDR_R
  return gethostbyname(hostname);
#endif //HAVE_GETHOSTBYNAME_R
#endif
}

/** my_getipnodebyname */
struct hostent * my_getipnodebyname(const char *hostname, int af)
{
  int err;

  return getipnodebyname(hostname, af, AI_DEFAULT, &err);
}

/** my_free_hostent */
void my_free_hostent(struct hostent *_hp)
{
#if HAVE_FREEHOSTENT
  //dax if (_hp) freehostent(_hp);
#else
  free(_hp);
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
  *(static_cast<int *> (addr)) = inet_addr(name);
  return static_cast<int> (addr);	// else return 1
#endif
}

/** inet4_ntop */
const char * inet4_ntop(const void *addr)
{
  return my_inet_ntop(AF_INET, addr);
}

/** inet4_pton */
int inet4_pton(const char *name, void *addr)
{
  return my_inet_pton(AF_INET, name, addr);
}
