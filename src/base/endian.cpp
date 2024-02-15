//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
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
#include "vreng.hpp"
#include "endian.hpp"


/**
 * check whether this machine is big endian
 * return 1 if big endian else 0 if little endian
 * static
 */
bool Endian::bigEndian()
{
#if 0 //dax
  uint16_t x=0x0001;
  auto p = reinterpret_cast<uint8_t*>(&x);
  return *p == 0;
#else
  union {
    int word;
    char byte[4];
  } endian;

  endian.word = 1;
  return (endian.byte[0] == 1) ? 0 : 1;
#endif
}

/**
 * check whether this machine is little endian
 * return 1 if little endian else 0 if big endian
 * static
 */
bool Endian::littleEndian()
{
  return (! bigEndian());
}

void Endian::swapShort(uint16_t *array, int len)
{
  uint32_t b1, b2;
  char *p;

  p = reinterpret_cast<char *>(array);
  while (len--) {
    b1 = *p++;
    b2 = *p++;
    *array++ = (b1 << 8) | (b2);
  }
}

void Endian::swapLong(uint32_t *array, int len)
{
  uint32_t b1, b2, b3, b4;
  char *p;

  p = reinterpret_cast<char *>(array);
  while (len--) {
    b1 = *p++;
    b2 = *p++;
    b3 = *p++;
    b4 = *p++;
    *array++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
  }
}

#if 0 //notused
void * Endian::swapEndian(void* data, int n)
{
  static char swapped[16];

  switch (n) {
    case 2: swapped[0] = *((char*)data+1);
            swapped[1] = *((char*)data  );
            break;
    case 4: swapped[0] = *((char*)data+3);
            swapped[1] = *((char*)data+2);
            swapped[2] = *((char*)data+1);
            swapped[3] = *((char*)data  );
            break;
#if 0 //notused
    case 6: swapped[0] = *((char*)data+5);
            swapped[1] = *((char*)data+4);
            swapped[2] = *((char*)data+3);
            swapped[3] = *((char*)data+2);
            swapped[4] = *((char*)data+1);
            swapped[5] = *((char*)data  );
            break;
    case 8: swapped[0] = *((char*)data+7);
            swapped[1] = *((char*)data+6);
            swapped[2] = *((char*)data+5);
            swapped[3] = *((char*)data+4);
            swapped[4] = *((char*)data+3);
            swapped[5] = *((char*)data+2);
            swapped[6] = *((char*)data+1);
            swapped[7] = *((char*)data  );
            break;
     case 16:swapped[0] = *((char*)data+15);
            swapped[1] = *((char*)data+14);
            swapped[2] = *((char*)data+13);
            swapped[3] = *((char*)data+12);
            swapped[4] = *((char*)data+11);
            swapped[5] = *((char*)data+10);
            swapped[6] = *((char*)data+9);
            swapped[7] = *((char*)data+8);
            swapped[8] = *((char*)data+7);
            swapped[9] = *((char*)data+6);
            swapped[10] = *((char*)data+5);
            swapped[11] = *((char*)data+4);
            swapped[12] = *((char*)data+3);
            swapped[13] = *((char*)data+2);
            swapped[14] = *((char*)data+1);
            swapped[15] = *((char*)data  );
            break;
#endif //notused
      default:error("Unsupported length for swapEndian");
  }
  return static_cast<void *>(swapped);
}

void Endian::localEndian(void *data, int n)
{           
  if (bigEndian()) {
    char *tmp = reinterpret_cast<char *>(swapEndian(data, n));
    memcpy(data, tmp, n);
  }         
}
#endif //notused
