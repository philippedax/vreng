#ifndef BOX_H
#define BOX_H

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

  virtual int GetClassId() const {return BOX;}
  virtual const char* GetClassName() const {return "box";}

  virtual void UpdateBoundingVolumes(int recalc);
  virtual void Render();
  virtual void Print() const {
    cout << "box ";
    Solid::Print();
  }

};

#endif
