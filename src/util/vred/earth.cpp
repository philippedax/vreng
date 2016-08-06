#include "vred.hpp"
#include "vre.hpp"
#include "earth.hpp"

Earth::Earth(char* _name, 
	     const Vect& _center, const Vect& _orientation, const Vect& _size,
	     int _renderStyle, const Color& _color,
	     const Tex& _tex, const App& _app,
	     const int _slices, const int _stacks)
  : Sphere(_name, _center, _orientation, _size, _renderStyle, _color, _tex, _app, _slices, _stacks)
{};

