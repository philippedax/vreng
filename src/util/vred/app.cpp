#include "vred.hpp"
#include "app.hpp"

App::App(const Color& ambient, const Color& diffuse, const Color& specular,
	 const Color& emission, const Color& shininess):
  ambient(ambient), diffuse(diffuse), specular(specular), emission(emission), shininess(shininess) {
}

void App::Print() const 
{
  printf("diffuse : ");
  diffuse.Print();
  printf("\nambient : ");
  ambient.Print();
  printf("\nspecular : ");
  specular.Print();
  printf("\nemission : ");
  emission.Print();
  printf("\nshininess : ");
  shininess.Print();
  printf("\n");
}
