//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)
// 
// Copyright (C) 1997-2005 Ecole Nationale Superieure des Telecommunications
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

static unsigned long bitstorage = 0;
static int numbits = 0;
static int mask [] = {0, 1, 3, 7, 15, 31, 63, 127, 255};
static int count = 0;


void writechar(char c)
{
  putchar(c);
  count ++;
  if (count >= 72) {
    printf("\\\n");
    count = 0;
  }
}

void padoutput(void)
{
  while ((count % 4) != 0) {
    putchar('=');
    count++;
  }
}

unsigned int readbits(int n, int *pbitsread)
{
  static int eof = 0;
  unsigned long scratch;

  while ((numbits < n) && (!eof)) {
    int c = getchar();
    if (c == EOF) eof = 1;
    else {
      bitstorage <<= 8;
      bitstorage |= (c & 0xff);
      numbits += 8;
    }
  }
  if (numbits < n) {
    scratch = bitstorage << (n - numbits);
    *pbitsread = numbits;
    numbits = 0;
  } else {
    scratch = bitstorage >> (numbits -n);
    *pbitsread = n;
    numbits -= n;
  }
  return (unsigned int) (scratch & mask [n]);
}

int main(int argc, char **argv)
{
  int numbits = 6;
  int digit;
  const char *digits = base64digits;
  char *p, *pd, string[BUFSIZ];

  if (argc != 2) {
    printf("usage: %s file\n", argv[0]);
    exit(0);
  }
  if (freopen(argv[1], "r", stdin) == NULL) {
    fprintf(stderr, "file %s not found\n", argv[1]);
    exit(1);
  }
  strcpy(string, argv[1]);

  pd = p  = string;
  while ((p = strchr(p, '/')) != NULL)
    pd=p++;
  p = strchr(pd, '.');
  if (p != NULL)
    *p = '\0';

  printf("const char *%s_au = \"\\\n", pd);

  digit = readbits (numbits, &numbits);
  while (numbits > 0) {
    writechar (digits[digit]);
    digit = readbits (numbits, &numbits);
  }
  padoutput();
  printf("\";\n");

  return 0;
}
