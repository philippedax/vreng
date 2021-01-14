#ifndef BOARD_HPP
#define BOARD_HPP

#include "box.hpp"

class Board : public Box {

public:
  Board(char* _name,
        const Vect& _center = Vect::null,
        const Vect& _orientation = Vect::null,
        const Vect& _size = Vect::unit,
        int _renderStyle = WIRED,
        const Color& _color = Color::white,
        const Tex& _tex = Tex(), const App& _app = App());

  virtual int getClassId() const { return BOARD; }
  virtual const char* getClassName() const { return "board"; }

  friend ostream& operator<<(ostream& out, const Board& b);

};

#endif
