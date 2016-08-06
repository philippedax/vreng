#include <GL/glut.h>

#include "vred.hpp"
#include "ui.hpp"


Group * Vred::treeRoot;

int main(int argc, char* argv[])
{ 
  UI::SetupUI(argc, argv);
  glutMainLoop();
}
