#ifndef DOC_HPP
#define DOC_HPP

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
      const char* _url = NULL
     ) :
      Box(_name, _center, _orientation, _size, _renderStyle, _color, _tex, _app)
      {
        url = NULL;
        setUrl(_url);
      }

  virtual int getClassId() const {return DOC;}
  virtual const char* getClassName() const {return "doc";}

  friend ostream& operator<<(ostream& out, const Doc& d);

  inline const char* const getUrl() const {return url;}
  void setUrl(const char* _url)
  {
    Safe::safe_strdup(&url, _url);
  }

private:
  char* url;

};

#endif
