/*
 * tri2quad.c
 * Mesh transformation for Vreng
 *
 * Author: Philippe Dax
 * Date: 18/05/2007
 *
 * Copyright (c) Philippe Dax - May 2007
 *
 * main changes :
 *	- faces triangles -> faces quads
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char **argv)
{
  char buf[512];
  int v, vt, f, vi, ti, ni, vc, tc, nf, ns;
  int begin = 1;
  int r;
  float x, y, z, s, t;
  char *p;

  v = vt = f = 0;
  vc = tc = 0;
  nf = 0;

  // process
  while (fgets(buf, sizeof(buf), stdin)) {
    if (! strncmp(buf, "#", 1)) {
      continue;
    }
    else if (! strncmp(buf, "\n", 1)) {
      continue;
    }
    else if (! strncmp(buf, "\r", 1)) {
      continue;
    }
    else if (! strncmp(buf, "s", 1)) {
      continue;
    }
    else if (! strncmp(buf, "use", 3)) {
      continue;
    }

    // name
    else if (! strncmp(buf, "g ", 2)) {
      printf("%s", buf);
      continue;
    }

    // vertices
    else if (! strncmp(buf, "v ", 2)) {
      v++;
      sscanf(buf, "v %f %f %f", &x, &y, &z);
      printf("v %.3f %.3f %.3f\n", x, y, z);
    }

    // texture coords
    else if (! strncmp(buf, "vt ", 3)) {
      sscanf(buf, "vt %f %f", &s, &t);
      printf("vt %.2f %.2f\n", s, t);
   }

    // normals
    else if (! strncmp(buf, "vn ", 3)) {
      ;	// ignore
    }

    // faces
    else if (! strncmp(buf, "f ", 2)) {
      f++;
      printf("f");
      p = buf;
      ns = 0;	//sides
      while ((p = strchr(p, ' ')) && (ns < 4)) {
        ns++;
        sscanf(p, "%d/%d", &vi, &ti);
        printf(" %d/%d", vi, ti);
        p++;
      }
      if (ns == 3) {	// triangle
        printf(" %d/%d", vi, ti);	// quad (ajout 4eme vertice confondu)
      }
      printf("\n");
    }

    // other stuff
    else {
      printf("%s", buf);
    }
  }

  // stats
  printf("#\n# v : %d\n# f : %d\n", v, f);

  // close
  fclose(stdout);
  return 0;
}
