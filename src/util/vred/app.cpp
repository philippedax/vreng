#include "vred.hpp"
#include "app.hpp"

App::App(const Color& ambient, const Color& diffuse, const Color& specular,
	 const Color& emission, const Color& shininess):
  ambient(ambient), diffuse(diffuse), specular(specular), emission(emission), shininess(shininess) {
}

void App::print() const 
{
  printf("diffuse : ");
  diffuse.print();
  printf("\nambient : ");
  ambient.print();
  printf("\nspecular : ");
  specular.print();
  printf("\nemission : ");
  emission.print();
  printf("\nshininess : ");
  shininess.print();
  printf("\n");
}
