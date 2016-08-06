#ifndef HOST_H
#define HOST_H

#include "vred.hpp"
#include "box.hpp"
#include "tex.hpp"
#include "app.hpp"
#include "safe.hpp"


class Host : public Box {

public:
  Host(char* _name,
       const Vect& _center = Vect::null,
       const Vect& _orientation = Vect::null,
       const Vect& _size = Vect::unit,
       int _renderStyle = WIRED, const Color& _color = Color::white,
       const Tex& _tex = Tex(), const App& _app = App(),
       const char* _hostname = NULL) :
       Box(_name, _center, _orientation, _size, _renderStyle, _color, _tex, _app)
       {
         hostname = NULL;
         SetHostname(_hostname);
       }

  virtual int GetClassId() const {return HOST;}
  virtual const char* GetClassName() const {return "host";}

  friend ostream& operator<<(ostream& out, const Host& h);
  
  inline const char* const GetHostname() const {return hostname;}
  void SetHostname(const char* _hostname)
  {
    Safe::safe_strdup(&hostname, _hostname);
  }

private:
  char* hostname;

};

#endif
