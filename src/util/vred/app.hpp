#ifndef APP_HPP
#define APP_HPP

#include "color.hpp"

class App {

public:
  App(const Color& ambient = Color::white,
      const Color& diffuse = Color::black,
      const Color& specular = Color::black,
      const Color& emission = Color::black,
      const Color& shininess = Color::black);

  const Color& getDiffuse() const {return diffuse;}
  void setDiffuse(const Color& d) {diffuse = d;}
  const Color& getAmbient() const {return ambient;}
  void setAmbient(const Color& a) {ambient = a;}
  const Color& getSpecular() const {return specular;}
  void setSpecular(const Color& s) {specular = s;}
  const Color& getEmission() const {return emission;}
  void setEmission(const Color& s) {emission = s;}
  const Color& getShininess() const {return shininess;}
  void setShininess(const Color& s) {shininess = s;}

  void print() const;

private:
  Color	ambient, diffuse, specular, emission, shininess;

};

#endif
