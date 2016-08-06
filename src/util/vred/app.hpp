#ifndef APP_H
#define APP_H

#include "color.hpp"

class App {

public:
  App(const Color& ambient = Color::white,
      const Color& diffuse = Color::black,
      const Color& specular = Color::black,
      const Color& emission = Color::black,
      const Color& shininess = Color::black);

  const Color& GetDiffuse() const {return diffuse;}
  void SetDiffuse(const Color& d) {diffuse = d;}
  const Color& GetAmbient() const {return ambient;}
  void SetAmbient(const Color& a) {ambient = a;}
  const Color& GetSpecular() const {return specular;}
  void SetSpecular(const Color& s) {specular = s;}
  const Color& GetEmission() const {return emission;}
  void SetEmission(const Color& s) {emission = s;}
  const Color& GetShininess() const {return shininess;}
  void SetShininess(const Color& s) {shininess = s;}

  void Print() const;

private:
  Color	ambient, diffuse, specular, emission, shininess;

};

#endif
