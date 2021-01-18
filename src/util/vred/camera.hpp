#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "solid.hpp"

class Camera : public Solid {

public:
  Camera(const char* _name, const Vect& position, const Vect& aim);

  int getClassId() const { return CAMERA; }
  const char* getClassName() const { return "Camera"; }

  Vect getCenter() const { return center; }
  Vect getOrientation() const { return orient; }
  Vect getSize() const { return 0; }

  virtual void updateBB(int recalc);
  void render() {}
  void look(const Vect& position, const Vect& aim) const;
  void move(const Vect& translation, const Vect& rotation);
  Vect getWatch() const;

};

#endif
