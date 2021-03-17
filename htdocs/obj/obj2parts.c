/*
 * obj2parts.c
 * OBJ Mesh transformation for Vreng models
 *
 * Author: Philippe Dax
 * Date: 07/05/2007
 *
 * Copyright (c) Philippe Dax - 2007
 *
 * main changes :
 *	- faces triangles -> faces quads
 *	- spliting body in several body-parts identified by 'g '
 *	- scaling
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SCALE 3.2

main(int argc, char **argv)
{
  char buf[512];
  int v, vt, f, vi, ti, ni, vc, tc, nf, ns;
  int begin = 1;
  int r;
  float x, y, z, s, t;
  char *p;
  char file[32];
  static int first = 1;
  char * temp;
  static char template[] = "temp.XXXXXX";
  static char prefix[] = "avatar";
  FILE *fp = NULL;

  v = vt = f = 0;
  vc = tc = 0;
  nf = 0;

  temp = mktemp(template);
  if (! temp) {
    fprintf(stderr, "mktemp null");
    exit(1);
  }
  fp = fopen(temp, "w");

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

    // part name
    else if (! strncmp(buf, "g ", 2)) {
      if (buf[strlen(buf) -2] == '\r')
        buf[strlen(buf) -2] = 0;
      else
        buf[strlen(buf) -1] = 0;
      sprintf(file, "%s_%s.obj", prefix, buf+2);
      printf("g %s\n", buf+2);
      if (fp)
        fprintf(fp, "g %s\n", buf+2);
    }

    // vertices
    else if (! strncmp(buf, "v ", 2)) {
      if (! begin) {
        // end
        printf("#\n# v : %d\n# f : %d\n", v - vc, nf);
        if (fp) {
          fprintf(fp, "#\n# v : %d\n# f : %d\n", v - vc, nf);
          fclose(fp);

          rename(temp, file);

          temp = mktemp(template);	/* new file */
          if (! temp) {
            fprintf(stderr, "mktemp null");
            exit(1);
          }
          fp = fopen(temp, "w");
        }
        begin = 1;	/* new part */
        vc = v + 0;
        tc = vt + 0;
        nf = 0;
      }
      v++;
      sscanf(buf, "v %f %f %f", &x, &y, &z);
      // scaling
      x *= SCALE;
      y *= SCALE;
      z *= SCALE;
      printf("v %.3f %.3f %.3f\n", -z, -x, y);
      if (fp)
        fprintf(fp, "v %.3f %.3f %.3f\n", -z, -x, y);
    }

    // texture coords
    else if (! strncmp(buf, "vt ", 3)) {
      vt++;
      sscanf(buf, "vt %f %f", &s, &t);
      printf("vt %.2f %.2f\n", s, t);
      if (fp)
        fprintf(fp, "vt %.2f %.2f\n", s, t);
   }

    // normals
    else if (! strncmp(buf, "vn ", 3)) {
      ;	// discard
    }

    // faces
    else if (! strncmp(buf, "f ", 2)) {
      f++;
      nf++;
      if (begin) {
        begin = 0;
      }
      printf("f");
      if (fp)
        fprintf(fp, "f");
      p = buf;
      ns = 0;	//sides
      while ((p = strchr(p, ' '))) {
        ns++;
        sscanf(p, "%d/%d", &vi, &ti);
        vi -= vc;
        if (vi > v) {
          p++;
          continue;
        }
        ti -= tc;
        printf(" %d/%d", vi, ti);
        if (fp)
          fprintf(fp, " %d/%d", vi, ti);
        p++;
      }
      if (ns == 3) {	// triangle
        printf(" %d/%d", vi, ti);	// quad
        if (fp)
          fprintf(fp, " %d/%d", vi, ti);
      }
      printf("\n");
      if (fp)
        fprintf(fp, "\n");
    }

    // other stuff
    else {
      printf("%s", buf);
      if (fp)
        fprintf(fp, "%s", buf);
    }
  }

  // stats
  printf("#\n# v : %d\n# f : %d\n", v, f);
  if (fp)
    fprintf(fp, "#\n# v : %d\n# f : %d\n", v, f);

  // close
  fclose(stdout);
  if (fp) {
    fclose(fp);
    r = rename(temp, file);
    if (r < 0)
      perror("rename");
  }
}
