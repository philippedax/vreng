#ifndef BSPHERE_H
#define BSPHERE_H

#include "vect.hpp"

class Solid;
class Ray;
class Box;
class Sphere;
class Camera;

class Bsphere {

public:
  Bsphere() {}
  Bsphere(Vect& _center, double _radius): center(_center), radius(_radius) {};

  virtual ~Bsphere() {}

  Vect*  RayIntersect(Ray& _ray) const;
  int     Collide(const Bsphere& sph) const;
  void    Set(const Vect& _center, double r);

  Vect  Center() const {return center;}
  void  SetCenter(const Vect& _center) {center = _center;}
  double  Radius() const {return radius;}
  void    SetRadius(double _radius) {radius = _radius;}
  void    CalcGroup(int n,Solid** table);
  void Print();

private:
  Vect  center;
  double  radius;

};

#endif
