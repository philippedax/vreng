#include "vred.hpp"
#include "safe.hpp"
#include "gate.hpp"


Gate::Gate(char* _name, const Vect& _center, const Vect& _orientation, const Vect& _size,
	   int _renderStyle, const Color& _color,
	   const Tex& _tex, const App& _appli, char* _toWhere, char* _ip_multi) :
  Box(_name, _center, _orientation, _size, _renderStyle, _color, _tex, _appli)
{
  toWhere = NULL;
  setUrl(_toWhere);
}

Gate::~Gate()
{
  free(toWhere);
}

void Gate::setUrl(const char *_toWhere)
{
  Safe::safe_strdup(&toWhere, _toWhere);  
}

void Gate::setIpmc(const char *_ip_multi)
{
}
