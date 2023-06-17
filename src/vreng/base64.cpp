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
 * decoder base64
 */
#include "base64.hpp"


static int numbits = 0;
static int size = 0;
static const char *pin;
static char *pout;


static
int readchar()
{
  int c;

  do {
    c = *pin++;
    if (c == 0) return EOF;
  } while ((((c+1) & 0x7f) < 33)); /* skip control char */
  return c;
}

static
void writebits(uint32_t bits, int n)
{
  static uint32_t bitstorage = 0;

  bitstorage = (bitstorage << n) | bits;
  numbits += n;
  while (numbits > 7) {
    uint32_t scratch = bitstorage >> (numbits - 8);
    *pout++ = scratch & 0xff;
    size++;
    numbits -= 8;
  }
}

int decodeBase64(const char *str, char *out)
{
  int c, digit;
  int decode[256];
  int i;
  const char *digits = base64digits;

  numbits = 0;
  size = 0;
  pin = str;
  pout = out;
  /* build decode table */
  for (i = 0; i < 256; i++)
    decode[i] = -2; /* illegal digit */
  for (i = 0; i < 64; i++) {
     decode[(int)digits[i]] = i;
     decode[(int)digits[i] | 0x80] = -1; /* ignore parity when decoding */
  }
  decode[(int) '='] = -1;
  decode[(int) '=' | 0x80] = -1;  /* ignore '=' for mime */

  for (c = readchar(); c != EOF; c = readchar()) {
    digit = decode[c & 0x7f];
    if (digit < -1) {
      fprintf(stderr, "decodeBase64: illegal char: %c\n", c);
      return -1;
    } else if (digit >= 0)
      writebits(digit & 0x3f, 6);
  }
  return size;
}
