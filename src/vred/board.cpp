#include "vred.hpp"
#include "board.hpp"

Board::Board(char* _name, const Vect& _center, const Vect& _orientation, const Vect& _size,
  int _renderStyle, const Color& _color, const Tex& _tex, const App& _appli):
  Box(_name, _center, _orientation, _size, _renderStyle, _color, _tex, _appli)
{}
