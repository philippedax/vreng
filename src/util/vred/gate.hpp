#ifndef GATE_H
#define GATE_H

#include "box.hpp"

class Gate : public Box {

public:
  Gate(char* _name,
       const Vect& _center = Vect::null,
       const Vect& _orientation = Vect::null,
       const Vect& _size = Vect::unit,
       int _renderStyle = WIRED, const Color& _color = Color::white,
       const Tex& _tex = Tex(), const App& _app = App(),
       char* _toWhere = NULL, char* _ip_multi = NULL);
  virtual ~Gate();

  virtual int     GetClassId() const {return GATE;}
  virtual const char*   GetClassName() const {return "gate";}

  friend ostream& operator<<(ostream& out, const Gate& gate);

  void SetToWhere(const char *_toWhere);
  void SetIp_multi(const char *_ip_multi);
  const char* const ToWhere() const {return toWhere;}
  const char* const Ip_multi() const {return ip_multi;}

private:
  char* toWhere;
  char* ip_multi;

};

#endif
