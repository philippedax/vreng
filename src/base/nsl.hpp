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
// nsl.hpp
//---------------------------------------------------------------------------
#ifndef NSL_HPP
#define NSL_HPP

#ifndef AF_INET6
#define AF_INET6 26
#endif


struct hostent * my_gethostbyname(const char *hostname, int af);
struct hostent * my_gethostbyname_r(const char *hostname, int af);
struct hostent * my_getipnodebyname(const char *hostname, int af);
void my_free_hostent(struct hostent *hp);

const char * my_inet_ntop(int af, const void *addr);
int my_inet_pton(int af, const char *name, void *addr);
const char * inet4_ntop(const void *addr);
int inet4_pton(const char *name, void *addr);

//notused struct hostent * my_getipnodebyaddr(const char *hostaddr, int af);
//notused const char * inet6_ntop(const void *addr);
//notused int inet6_pton(const char *name, void *addr);
//notused struct hostent * my_gethostbyaddr(const char *hostaddr, int af);
//notused struct hostent * my_gethostbyaddr_r(const char *hostaddr, int af);
//notused struct servent * my_getservbyname(const char *servname);
//notused struct servent * my_getservbyname_r(const char *servname);

#endif
