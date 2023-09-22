#include "vred.hpp"
#include "safe.hpp"
#include "gate.hpp"


Gate::Gate(char* _name, const Vect& _center, const Vect& _orientation, const Vect& _size,
	   int _renderStyle, const Color& _color,
	   const Tex& _tex, const App& _appli, char* _url, char* _ip_multi) :
  Box(_name, _center, _orientation, _size, _renderStyle, _color, _tex, _appli)
{
  url = NULL;
  setUrl(_url);
}

Gate::~Gate()
{
  free(url);
}

void Gate::setUrl(const char *_url)
{
  Safe::safe_strdup(&url, _url);  
}

void Gate::setIpmc(const char *_ip_multi)
{
}
