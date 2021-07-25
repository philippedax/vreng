#ifndef MIRAGE_HPP
#define MIRAGE_HPP

#include "vred.hpp"
#include "box.hpp"
#include "tex.hpp"
#include "app.hpp"
#include "safe.hpp"

class Mirage : public Box {

public:
  Mirage(char* _name,
         const Vect& _center = Vect::null,
         const Vect& _orientation = Vect::null,
         const Vect& _size = Vect::unit,
         int _renderStyle = WIRED,
         const Color& _color = Color::white,
         const Tex& _tex = Tex(),
         const App& _app = App()
        );

  virtual int getClassId() const {return MIRAGE;}
  virtual const char* getClassName() const {return "mirage";}

  friend ostream& operator<<(ostream& out, const Mirage& d);
};

#endif
