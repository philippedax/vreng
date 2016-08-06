#include "vred.hpp"
#include "output.hpp"

double rad(double d);


void printHeader(const Item& i, ostream& out)
{
  out << "<" << i.GetClassName() << " ";
}

void printTailer(const Item& i, ostream& out)
{
  out << "</" << i.GetClassName() << ">\n";
}


// TODO : implementer le Valpha
void pos_ang(const Solid& b, ostream& out)
{
  const Vect& c = b.GetCenter();
  const Vect& o = b.GetOrientation();

  out << "pos=\"" << c[0] << " " << c[1] << " " << c[2] << " " << rad(o[2]) << "\"> ";
}

void pos_ang_sphere(const Solid& b, ostream& out)
{
  const Vect& c = b.GetCenter();

  out << "pos=\"" << c[0] << " " << c[1] << " " << c[2] << "\"> ";
}

void box_size(const Box& b, ostream& out)
{
  const Vect& dim = b.GetSize();

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
  if (!(a.GetDiffuse() == aref.GetDiffuse())) {
    out << " dif=\"";
    printColor(a.GetDiffuse(), out);
    out << "\"";
  }
  if (!(a.GetAmbient() == aref.GetAmbient())) {
    out << " amb=\"";
    printColor(a.GetAmbient(), out);
    out << "\"";
  }
  if (!(a.GetShininess() == aref.GetShininess())) {
    out << " shi=\"";
    printColor(a.GetShininess(), out);
    out << "\"";
  }
  if (!(a.GetSpecular() == aref.GetSpecular())) {
    out << " spe=\"";
    printColor(a.GetSpecular(), out);
    out << "\"";
  }
}

void box_props(const Box& b, ostream& out)
{
  // BOX_SIZE
  box_size(b, out);

  // BOX_TEX
  Tex t=b.GetTexture();
  if (t.GetTex_xp() != NULL) {
    out << " xp=\"" << t.GetTex_xp() << "\"";
  }
  if (t.GetTex_xn() != NULL) {
    out << " xn=\"" << t.GetTex_xn() << "\"";
  }
  if (t.GetTex_yp() != NULL) {
    out << " yp=\"" << t.GetTex_yp() << "\"";
  }
  if (t.GetTex_yn() != NULL) {
    out << " yn=\"" << t.GetTex_yn() << "\"";
  }
  if (t.GetTex_zp() != NULL) {
    out << " zp=\"" << t.GetTex_zp() << "\"";
  }
  if (t.GetTex_zn() != NULL) {
    out << " zn=\"" << t.GetTex_zn() << "\"";
  }

  // APP
  printApp(b.GetApp(), out);
  out << " />";
}

// void outWall(Box& b, ostream& out)
// {
//   pos_ang(b, out);
//   box_props(b, out);
// }
void spher_size(const Sphere& s, ostream& out)
{
  const Vect& dim = s.GetSize();

  out << "<solid shape=\"sphere\" r=\"";
  out << dim[0];
  out << "\"";
}

void spher_props (const Sphere &s, ostream& out)
{
  // SPHER_SIZE
  spher_size(s, out);
  
  // SPHER_TEX
  if ( s.GetTexture().GetTex_xp() != NULL ) {
    out << " tx=\"";
    out << s.GetTexture().GetTex_xp();
    out << "\"";
  }
  // APP
  printApp(s.GetApp(), out);
  out << " />";
}

// degre -> rad
double rad(double d) {
  return d * M_PI / 180. ;
}
