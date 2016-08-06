#ifndef CAMERA_H
#define CAMERA_H

#include "solid.hpp"

class Camera : public Solid {

public:
  Camera(const char* _name, const Vect& position, const Vect& aim);

  int GetClassId() const {return CAMERA;}
  const char* GetClassName() const {return "Camera";}

  Vect GetCenter() const {return center;}
  Vect GetOrientation() const {return orientation;}
  Vect GetSize() const {return 0;}

  virtual void UpdateBoundingVolumes(int recalc);
  void Render() {}
  void Look(const Vect& position, const Vect& aim) const;
  void Move(const Vect& translation, const Vect& rotation);
  Vect GetWatch() const;

};

#endif
