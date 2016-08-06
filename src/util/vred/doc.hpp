#ifndef DOC_H
#define DOC_H

#include "vred.hpp"
#include "box.hpp"
#include "tex.hpp"
#include "app.hpp"
#include "safe.hpp"

class Doc : public Box {

public:
  Doc(char* _name,
      const Vect& _center = Vect::null,
      const Vect& _orientation = Vect::null,
      const Vect& _size = Vect::unit,
      int _renderStyle = WIRED, const Color& _color = Color::white,
      const Tex& _tex = Tex(), const App& _app = App(),
      const char* _url = NULL) :
      Box(_name, _center, _orientation, _size, _renderStyle, _color, _tex, _app)
      {
        url = NULL;
        SetUrl(_url);
      }

  virtual int GetClassId() const {return DOC;}
  virtual const char* GetClassName() const {return "doc";}

  friend ostream& operator<<(ostream& out, const Doc& d);

  inline const char* const GetUrl() const {return url;}
  void SetUrl(const char* _url)
  {
    Safe::safe_strdup(&url, _url);
  }

private:
  char* url;

};

#endif
