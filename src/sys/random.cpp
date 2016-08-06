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
/*
 * Generate a random 32-bit quantity
 *
 * RFC 1889
 */
#include "sysdep.hpp"
#include "md5.hpp"	// from RFC 1321


static uint32_t md_32(char *string, int length)
{
  MD5_CTX context;
  union {
    char  c[16];
    uint32_t  x[4];
  } digest;
  uint32_t r;
  int i;

  MD5Init(&context);
  MD5Update(&context, (uint8_t *) string, length);
  MD5Final((unsigned char *)&digest, &context);
  r = 0;
  for (i = 0; i < 3; i++) {
    r ^= digest.x[i];
  }
  return r;
}

/*
 * Return random unsigned 32-bit quantity. Use 'type' argument if you
 * need to generate several different values in close succession.
 */
unsigned int random32(int type)
{
  struct {
    int  type;
    struct  timeval tv;
    clock_t  cpu;
    pid_t  pid;
    uint32_t  hid;
#if HAVE_GETUID
    uid_t  uid;
    gid_t  gid;
#endif
#if HAVE_UNAME
    struct  utsname name;
#endif
  } s;

  gettimeofday(&s.tv, 0);
#if HAVE_UNAME
  uname(&s.name);
#endif
  s.type = type;
  s.cpu  = clock();
  s.pid  = getpid();
#if HAVE_GETHOSTID
  s.hid  = gethostid();
#else
  s.hid  = 1;
#endif
#if HAVE_GETUID
  s.uid  = getuid();
  s.gid  = getgid();
#endif

  return md_32((char *)&s, sizeof(s));
}
