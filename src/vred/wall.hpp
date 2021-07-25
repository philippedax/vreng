#ifndef WALL_HPP
#define WALL_HPP

#include "box.hpp"

class Wall : public Box {

public:
  Wall(char* _name,
       const Vect& _center = Vect::null,
       const Vect& _orientation = Vect::null,
       const Vect& _size = Vect::unit,
       int _renderStyle = WIRED, const Color& _color = Color::white,
       const Tex& _tex = Tex(), const App& _app = App()
      );

  virtual int getClassId() const {return WALL;}
  virtual const char* getClassName() const {return "wall";}

  friend ostream& operator<<(ostream& out, const Wall& w);
};

#endif



