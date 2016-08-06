#ifndef SPHERE_H
#define SPHERE_H

#include "solid.hpp"

class Sphere : public Solid {

public:
  Sphere(char* _name,
	 const Vect& _center = Vect::null,
	 const Vect& _orientation = Vect::null,
	 const Vect& _size = Vect::unit,
	 const int _renderStyle = WIRED, const Color& _color = Color::white,
	 const Tex& _tex = Tex(), const App& _app = App(),
	 const int _slices = 10, const int _stacks = 10);
  virtual ~Sphere();

  virtual int GetClassId() const { return SPHERE; };
  virtual const char* GetClassName() const { return "sphere"; } ;

  virtual void UpdateBoundingVolumes(int recalc);
  virtual void Render();
  virtual void Print() const {
    cout << "sphere ";
    Solid::Print();
  }
  virtual void SetSize(const Vect&);

private:
  int slices, stacks;
  GLfloat* vertices;

};

#endif
