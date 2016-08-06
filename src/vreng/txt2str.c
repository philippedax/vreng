//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)
//
// Copyright (C) 1997-2007 Ecole Nationale Superieure des Telecommunications
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

int main(int argc, char **argv)
{
  int c;

  if (argc == 1) {
    fprintf(stderr, "usage: %s string < text > string.h\n", argv[0]);
    exit(1);
  }

  printf("const char %s[] = \"\\\n", argv[1]);
  while ((c = getchar()) != EOF) {
    if (c == '\n')
      printf("\\n\\\n");
    else if (c == '"')
      printf("\\\"");
    else
      putchar(c);
  }
  printf("\\n\\\n\";\n");
  fclose(stdout);
  return 0;
}
