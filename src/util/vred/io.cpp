#include "vred.hpp"
#include "solid.hpp"
#include "output.hpp"
#include "vre.hpp"


ostream& operator<<(ostream& out, const Gate& g) 
{
  printHeader(g, out);

  pos_ang(g, out);
  out << (g).ToWhere();
  box_props(g, out);
  printTailer(g, out);

  return out;
}

ostream& operator<<(ostream& out, const Wall& w) 
{
  printHeader(w, out);

  pos_ang(w, out);
  box_props(w, out);
  printTailer(w, out);

  return out;
}

ostream& operator<<(ostream& out, const Board& b) 
{
  printHeader(b, out);

  pos_ang(b, out);
  box_props(b, out);
  printTailer(b, out);

  return out;
}

ostream& operator<<(ostream& out, const Step& s) 
{
  printHeader(s, out);

  pos_ang(s, out);
  box_props(s, out);
  printTailer(s, out);

  return out;
}

ostream& operator<<(ostream& out, const Web& w) 
{
  printHeader(w, out);

  pos_ang(w, out);
  out << w.GetUrl() << " ";
  box_props(w, out);
  printTailer(w, out);

  return out;
}

ostream& operator<<(ostream& out, const Earth& e)
{
  printHeader(e, out);

  pos_ang_sphere(e, out);
  spher_props(e, out);
  printTailer(e, out);

  return out;
}

ostream& operator<<(ostream& out, const Host& h) 
{
  printHeader(h, out);

  pos_ang(h, out);
  out << h.GetHostname() ;
  box_props(h, out);
  printTailer(h, out);

  return out;
}

ostream& operator<<(ostream& out, const Doc& d) 
{
  printHeader(d, out);

  pos_ang(d, out);
  out << d.GetUrl() << " ";
  box_props(d, out);
  printTailer(d, out);

  return out;
}

ostream& operator<<(ostream& out, const Group& g) 
{
  for (int i=0; i<g.card; ++i) {
    out << *(g.group[i]);
  }
  return out;
}

ostream& operator<<(ostream& out, const Solid& s)
{
  const Wall *w = dynamic_cast<const Wall*>(&s);
  if (w != NULL) {
    out << *w;
    return out;
  }
  const Gate *g = dynamic_cast<const Gate*>(&s);
  if (g != NULL) {
    out << *g;
    return out;
  }
  const Earth *e = dynamic_cast<const Earth*>(&s);
  if (e != NULL) {
    out << *e;
    return out;
  }
  const Web *www = dynamic_cast<const Web*>(&s);
  if (www != NULL) {
    out << *www;
    return out;
  }
  const Board *b = dynamic_cast<const Board*>(&s);
  if (b != NULL) {
    out << *b;
    return out;
  }
  const Step *st = dynamic_cast<const Step*>(&s);
  if (st != NULL) {
    out << *st;
    return out;
  }
  const Host *h = dynamic_cast<const Host*>(&s);
  if (h != NULL) {
    out << *h;
    return out;
  }
  const Doc *d = dynamic_cast<const Doc*>(&s);
  if (d != NULL) {
    out << *d;
    return out;
  }
  out << "solid : default case\n";
  return out;
}

