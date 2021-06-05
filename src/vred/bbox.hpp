#ifndef BBOX_HPP
#define BBOX_HPP

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

  Vect* rayIntersect(Ray& _ray) const;
  int  collide(const Bbox& box) const;
  void set(const Vect& _center, double d, double h, double w, double a);

  Vect getCenter() const {return center;}
  void setCenter(const Vect& _center) {center = _center;}
  double getHeight() const {return height;}
  void setHeight(const double _height) {height = _height;}
  double getWidth() const {return width;}
  void setWidth(const double _width) {width = _width;}
  double getDepth() const {return depth;}
  void setDepth(double _depth) {depth = _depth;}
  void calcGroup(int n, Solid** table);
  int  inner(Vect pt) const;
  void print();

private:
  Vect center;
  double depth, width, height, alpha;

};

#endif
