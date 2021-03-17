#include <stdio.h>
#include <stdlib.h>

main(int argc, char **argv)
{
  char buf[512];
  float x, y, z;
  float minx = 100., miny = 100., minz = 100.;
  float maxx =-100., maxy =-100., maxz =-100.;
  float cx, cy, cz;
  float sx, sy, sz;
  int v = 0;

  gets(buf);
  v = atoi(buf);
  gets(buf);

  for (; gets(buf) && v ; v--) {
    sscanf(buf, "%f %f %f", &x, &y, &z);
    minx = (x < minx) ? x : minx;
    miny = (y < miny) ? y : miny;
    minz = (z < minz) ? z : minz;
    maxx = (x > maxx) ? x : maxx;
    maxy = (y > maxy) ? y : maxy;
    maxz = (z > maxz) ? z : maxz;
  }
  cx = (minx + maxx) / 2;
  cy = (miny + maxy) / 2;
  cz = (minz + maxz) / 2;
  sx = (maxx - minx);
  sy = (maxy - miny);
  sz = (maxz - minz);
  printf("c=%.3f %.3f %.3f  s=%.3f %.3f %.3f  min=%.3f %.3f %.3f  max=%.3f %.3f %.3f\njp=\"%.3f %.3f %.3f\"\n",
         cx,cy,cz, sx,sy,sz, minx,miny,minz, maxx,maxy,maxz, cx,cy,cz);
  fclose(stdout);
}
