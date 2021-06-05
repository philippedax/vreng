#ifndef BOX_HPP
#define BOX_HPP

#include "solid.hpp"

class Box : public Solid {

public:
  Box(char* _name,
      const Vect& _center = Vect::null,
      const Vect& _orientation = Vect::null,
      const Vect& _size = Vect::unit,
      const int _renderStyle = WIRED, const Color& _color = Color::white,
      const Tex& _tex = Tex(), const App& _app = App());
  virtual ~Box() {}

  virtual int getClassId() const { return BOX; }
  virtual const char* getClassName() const { return "box"; }

  virtual void updateBB(int recalc);
  virtual void render();
  virtual void print() const {
    cout << "box ";
    Solid::print();
  }

};

#endif
