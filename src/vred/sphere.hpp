#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "solid.hpp"

class Sphere : public Solid {

public:
  Sphere(char* _name,
	 const Vect& _center = Vect::null,
	 const Vect& _orientation = Vect::null,
	 const Vect& _size = Vect::unit,
	 const int _renderStyle = WIRED, const Color& _color = Color::white,
	 const Tex& _tex = Tex(), const App& _app = App(),
	 const int _slices = 10, const int _stacks = 10
        );
  virtual ~Sphere();

  virtual int getClassId() const { return SPHERE; };
  virtual const char* getClassName() const { return "sphere"; } ;

  virtual void updateBB(int recalc);
  virtual void render();
  virtual void print() const {
    cout << "sphere ";
    Solid::print();
  }
  virtual void setSize(const Vect&);

private:
  int slices, stacks;
  GLfloat* vertices;

};

#endif
