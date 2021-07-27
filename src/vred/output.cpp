#include "vred.hpp"
#include "output.hpp"

float rad(float d);


void printHeader(const Item& i, ostream& out)
{
  out << "<" << i.getClassName() << " ";
}

void printTailer(const Item& i, ostream& out)
{
  out << "</" << i.getClassName() << ">\n";
}


// TODO : implementer le Valpha
void pos_ang(const Solid& b, ostream& out)
{
  const Vect& c = b.getCenter();
  const Vect& o = b.getOrient();

  out << "pos=\"" << c[0] << " " << c[1] << " " << c[2] << " " << rad(o[2]) << "\"> ";
}

void pos_ang_sphere(const Solid& b, ostream& out)
{
  const Vect& c = b.getCenter();

  out << "pos=\"" << c[0] << " " << c[1] << " " << c[2] << "\"> ";
}

void box_size(const Box& b, ostream& out)
{
  const Vect& dim = b.getSize();

  out << "<solid shape=\"box\" dim=\"";
  out << dim[0] << " " << dim[1] << " " << dim[2];
  out << "\"";
}

void printVect(const Vect& v, ostream& out)
{
  out << v[0] << " " << v[1] << " " << v[2];
}

void printColor(const Color& c, ostream& out) {
  out << c[0] << " " << c[1] << " " << c[2];
}

void printApp(const App& a, ostream& out)
{
  App aref;
  if (!(a.getDiffuse() == aref.getDiffuse())) {
    out << " dif=\"";
    printColor(a.getDiffuse(), out);
    out << "\"";
  }
  if (!(a.getAmbient() == aref.getAmbient())) {
    out << " amb=\"";
    printColor(a.getAmbient(), out);
    out << "\"";
  }
  if (!(a.getShininess() == aref.getShininess())) {
    out << " shi=\"";
    printColor(a.getShininess(), out);
    out << "\"";
  }
  if (!(a.getSpecular() == aref.getSpecular())) {
    out << " spe=\"";
    printColor(a.getSpecular(), out);
    out << "\"";
  }
}

void box_props(const Box& b, ostream& out)
{
  // BOX_SIZE
  box_size(b, out);

  // BOX_TEX
  Tex t=b.getTexture();
  if (t.getTex_xp() != NULL) {
    out << " xp=\"" << t.getTex_xp() << "\"";
  }
  if (t.getTex_xn() != NULL) {
    out << " xn=\"" << t.getTex_xn() << "\"";
  }
  if (t.getTex_yp() != NULL) {
    out << " yp=\"" << t.getTex_yp() << "\"";
  }
  if (t.getTex_yn() != NULL) {
    out << " yn=\"" << t.getTex_yn() << "\"";
  }
  if (t.getTex_zp() != NULL) {
    out << " zp=\"" << t.getTex_zp() << "\"";
  }
  if (t.getTex_zn() != NULL) {
    out << " zn=\"" << t.getTex_zn() << "\"";
  }

  // APP
  printApp(b.getApp(), out);
  out << " />";
}

// void outWall(Box& b, ostream& out)
// {
//   pos_ang(b, out);
//   box_props(b, out);
// }
void spher_size(const Sphere& s, ostream& out)
{
  const Vect& dim = s.getSize();

  out << "<solid shape=\"sphere\" r=\"";
  out << dim[0];
  out << "\"";
}

void spher_props (const Sphere &s, ostream& out)
{
  // SPHER_SIZE
  spher_size(s, out);
  
  // SPHER_TEX
  if ( s.getTexture().getTex_xp() != NULL ) {
    out << " tx=\"";
    out << s.getTexture().getTex_xp();
    out << "\"";
  }
  // APP
  printApp(s.getApp(), out);
  out << " />";
}

// degre -> rad
float rad(float d)
{
  return d * M_PI / 180. ;
}
