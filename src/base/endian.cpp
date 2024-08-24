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
// endian.cpp
//
// Endian handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "endian.hpp"


/**
 * Checks whether this machine is big endian
 * return 1 if big endian else 0 if little endian
 * static
 */
bool Endian::bigEndian()
{
  union {
    int word;
    char byte[4];
  } endian;

  endian.word = 1;
  return (endian.byte[0] == 1) ? 0 : 1;
}

/**
 * Checks whether this machine is little endian
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
