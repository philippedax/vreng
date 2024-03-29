#ifndef BSPHERE_HPP
#define BSPHERE_HPP

#include "vect.hpp"

class Solid;
class Ray;
class Box;
class Sphere;
class Camera;

class Bsphere {

public:
  Bsphere() {}
  Bsphere(Vect& _center, float _radius): center(_center), radius(_radius) {};

  virtual ~Bsphere() {}

  Vect*   rayIntersect(Ray& _ray) const;
  int     collide(const Bsphere& sph) const;
  void    set(const Vect& _center, float r);

  Vect    getCenter() const { return center; }
  void    setCenter(const Vect& _center) { center = _center; }
  float  getRadius() const { return radius; }
  void    setRadius(float _radius) { radius = _radius; }
  void    calcGroup(int n,Solid** table);
  void    print();

private:
  Vect    center;
  float  radius;

};

#endif
