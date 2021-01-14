#ifndef __BBOX_CPP
#define __BBOX_CPP

#include "vred.hpp"
#include "vect.hpp"
#include "bbox.hpp"
#include "ray.hpp"

void Bbox::print()
{
#ifdef VERBOSE
  printf("  BBOX: center=<%g %g %g> size=<%g %g %g> alpha=%g\n",center[0],center[1],center[2],depth,width,height,alpha);
#endif
}

#endif
