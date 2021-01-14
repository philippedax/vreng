#ifndef RAY_HPP
#define RAY_HPP

#include "vect.hpp"

class Ray {

public: 
  Ray() {}
  Ray(Vect& _start, Vect& _dir) { start = _start; dir = _dir; }
  virtual ~Ray() {}

  Vect&  Start() { return start; }
  Vect&  Dir() { return dir; }

private:
  Vect   start;
  Vect   dir;

};

#endif
