#ifndef BBOX_H
#define BBOX_H

#include "vect.hpp"

class Solid;
class Ray;
class Box;
class Sphere;
class Camera;

class Bbox {

public:
  Bbox() {}
  Bbox(Vect& _p, double _depth, double _width, double _height, double _alpha) : center(_p), depth(_depth), width(_width), height(_height), alpha(_alpha) {}
  virtual ~Bbox() {}

  Vect* RayIntersect(Ray& _ray) const;
  int  Collide(const Bbox& box) const;
  void Set(const Vect& _center, double d, double h, double w, double a);

  Vect Center() const {return center;}
  void SetCenter(const Vect& _center) {center = _center;}
  double Height() const {return height;}
  void SetHeight(const double _height) {height = _height;}
  double Width() const {return width;}
  void SetWidth(const double _width) {width = _width;}
  double Depth() const {return depth;}
  void SetDepth(double _depth) {depth = _depth;}
  void CalcGroup(int n,Solid** table);
  int  Inner(Vect pt) const;
  void Print();

private:
  Vect center;
  double depth, width, height, alpha;

};

#endif
