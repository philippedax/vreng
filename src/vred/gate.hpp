#ifndef GATE_HPP
#define GATE_HPP

#include "box.hpp"

class Gate : public Box {

public:
  Gate(char* _name,
       const Vect& _center = Vect::null,
       const Vect& _orientation = Vect::null,
       const Vect& _size = Vect::unit,
       int _renderStyle = WIRED, const Color& _color = Color::white,
       const Tex& _tex = Tex(), const App& _app = App(),
       char* _toWhere = NULL, char* _ip_multi = NULL
      );
  virtual ~Gate();

  virtual int getClassId() const { return GATE; }
  virtual const char* getClassName() const { return "gate"; }

  friend ostream& operator<<(ostream& out, const Gate& gate);

  void setUrl(const char *_toWhere);
  void setIpmc(const char *_ip_multi);
  const char* const getUrl() const { return toWhere; }
  const char* const getIpmc() const { return ip_multi; }

private:
  char* toWhere;
  char* ip_multi;

};

#endif
