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
  Bbox(Vect& _p, float _depth, float _width, float _height, float _alpha) : center(_p), depth(_depth), width(_width), height(_height), alpha(_alpha) {}
  virtual ~Bbox() {}

  Vect* rayIntersect(Ray& _ray) const;
  int  collide(const Bbox& box) const;
  void set(const Vect& _center, float d, float h, float w, float a);

  Vect getCenter() const {return center;}
  void setCenter(const Vect& _center) {center = _center;}
  float getHeight() const {return height;}
  void setHeight(const float _height) {height = _height;}
  float getWidth() const {return width;}
  void setWidth(const float _width) {width = _width;}
  float getDepth() const {return depth;}
  void setDepth(float _depth) {depth = _depth;}
  void calcGroup(int n, Solid** table);
  int  inner(Vect pt) const;
  void print();

private:
  Vect center;
  float depth, width, height, alpha;

};

#endif
