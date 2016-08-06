//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)
// 
// Copyright (C) 1997-2008 Ecole Nationale Superieure des Telecommunications
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define base64digits \
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

static int numbits = 0;

static char *pstr = ouille;


int readchar(void)
{
  int c;
  do {
    c = *pstr++;
    if (c == 0) return EOF;
  } while ( (((c+1) & 0x7f) < 33) ); /* skip control char */
  return c;
}

void writebits(unsigned bits, int n)
{
  static unsigned long bitstorage = 0;

  bitstorage = (bitstorage << n) | bits;
  numbits += n;
  while (numbits > 7) {
    unsigned scratch = bitstorage >> (numbits - 8);
    putchar(scratch & 0xff);
    numbits -= 8;
  }
}

main(int argc, char **argv)
{
  int c, digit;
  int decode [256];
  int i;
  const char *digits = base64digits;

  if (argc != 2) {
    printf("usage: %s file\n", argv[0]);
    exit(0);
  }
  if (freopen(argv[1], "w", stdout) == NULL) {
    fprintf(stderr, "file %s exists\n", argv[1]);
    exit(1);
  }

  /* build decode table */
  for (i = 0; i < 256; i++) decode [i] = -2; /* illegal digit */
  for (i = 0; i < 64; i++) {
     decode [digits [i]] = i;
     decode [digits [i] | 0x80] = -1; /* ignore parity when decoding */
  }
  decode ['='] = -1;
  decode ['=' | 0x80] = -1;  /* ignore '=' for mime */

  c = readchar();
  while (c != EOF) {
    digit = decode [c & 0x7f];
    if (digit < -1) {
      fprintf (stderr, "illegal character: %c\n", c);
      exit (1);
    } else if (digit >= 0)
      writebits (digit & 0x3f, 6);
    c = readchar();
  }
}
