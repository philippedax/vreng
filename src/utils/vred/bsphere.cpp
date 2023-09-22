#include "vred.hpp"
#include "vect.hpp"
#include "bsphere.hpp"
#include "sphere.hpp"
#include "ray.hpp"

void Bsphere::print()
{
#ifdef VERBOSE
  printf("  BSPHERE: c=<%g %g %g> r=%g\n",center[0],center[1],center[2],radius);
#endif
}
