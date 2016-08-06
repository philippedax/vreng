#include "vred.hpp"
#include "ray.hpp"
#include "vect.hpp"
#include "bbox.hpp"
#include "bsphere.hpp"

#ifndef M_PI
#define M_PI 3.1415926535
#endif


#if 0
Vect* Bbox::RayIntersect(Ray& _ray) const
{
  Vect start = _ray.Start();
  Vect dir = _ray.Dir();

  // upper and looking upwards => no intersection
  if ((start[2] > (center[2] + 0.5*height)) && (dir[2] > 0.0))
    return 0;
  // the same for downwards
  if ((start[2] < (center[2] - 0.5*height)) && (dir[2] < 0.0))
    return 0;

  // otherwise, we compute the position of the intersection point
  // between the infinite planes
  double lambdamin;
  double lmin, lmax, tmp;
  lmin = (((center - start)*dir) - 0.5*height * dir[2]) / (dir * dir);
  lmax = lmin + ((height * dir[2]) / (dir * dir)); 
  if (lmax < lmin) {
    tmp = lmax; lmax = lmin; lmin = tmp;
  }
  if (lmin < 0.0)
    lambdamin = lmax;
  else
    lambdamin = lmin;

  // now, we can work in a XY plane.
  Vect BboxDir(cos(alpha), sin(alpha), 0.0);
  Vect BboxOrthoDir(-sin(alpha), cos(alpha), 0.0);  

  lmin = (((center - start)*dir) - 0.5*width * (dir * BboxOrthoDir));
  lmin /= dir * dir;
  lmax = lmin + ((width * (dir * BboxOrthoDir)) / (dir * dir)); 
  if (lmax < lmin) {
    tmp = lmax; lmax = lmin; lmin = tmp;
  }
  if (lmin < 0.0) {
    if (lmax < lambdamin) {lambdamin = lmax;}
  } else {
    if (lmax < 0.0) return 0;
    if (lmin < lambdamin) {lambdamin = lmin;}
  }

  // the same for depth with BboxDir
  lmin = (((center - start)*dir) - 0.5*depth * (dir * BboxDir));
  lmin /= dir * dir;
  lmax = lmin + ((depth * (dir * BboxDir)) / (dir * dir));
  if (lmax < lmin) {
    tmp = lmax; lmax = lmin; lmin = tmp;
  }
  if (lmin < 0.0) {
    if (lmax < lambdamin) {lambdamin = lmax;}      
  } else {       
    if (lmax < 0.0) return 0;
    if (lmin < lambdamin) {lambdamin = lmin;}      
  }

  Vect* result = new Vect();
  *result = _ray.Start() + (lambdamin * _ray.Dir());
  return result;
}
#endif

Vect* Bsphere::RayIntersect(Ray& _ray) const
{
  double delta;
  double a,b,c;

  a = _ray.Dir() * _ray.Dir();
  b = _ray.Dir()*(_ray.Start() - center);
  c = ((_ray.Start() - center) * (_ray.Start() - center)) - (radius*radius);
  delta = b*b - a*c;
  if (delta < 0.0001) return 0;
  Vect* result = new Vect(0, 0, 0);
  *result = _ray.Start() + ((-b-sqrt(delta))/a) * _ray.Dir();
  return result;
}

Vect* Bbox::RayIntersect(Ray& _ray) const
{
  int i;
  Vect pos = _ray.Start();
  Vect incr = _ray.Dir();
  incr *= 0.1;

  for (i=0; i < 1000; i++) {
    pos += incr;
    if (Inner(pos)) {
      return new Vect(pos);
    }
    if (incr[3] > 0 && pos[3] > center[3]+height/2) break;
    if (incr[3] < 0 && pos[3] < center[3]-height/2) break;
  }
  //  printf("Rien %d\n",i);
  return 0;
}
