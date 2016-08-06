#include <GL/glut.h>

#include "vred.hpp"
#include "box.hpp"

Box::Box(char* _name,
	 const Vect& _center, const Vect& _orientation, const Vect& _size,
	 const int _renderStyle, const Color& _color,
	 const Tex& _tex, const App& _app):
  Solid(_name, _center, _orientation, _size, _renderStyle, _color, _tex, _app)
{

  myBoundingBox.Set(_center,_size[0],_size[1],_size[2],_orientation[2]);
  myBoundingSphere.Set(_center,sqrt(_size.Norm()));
}

void Box::Render() {
//   color.Apply();

  // TODO: to be tested ...or removed !!!
  glMaterialfv(GL_FRONT, GL_AMBIENT, app.GetAmbient());
  glMaterialfv(GL_FRONT, GL_DIFFUSE, app.GetDiffuse());
  glMaterialfv(GL_FRONT, GL_SPECULAR, app.GetSpecular());
  glMaterialfv(GL_FRONT, GL_SHININESS, app.GetShininess());

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMultMatrixd(modelView);
  
  const GLvoid* texData[6];
  
  switch(renderStyle) {
  case WIRED:
    glutWireCube(1.0);
    break;
  case COLORED:
    glutSolidCube(1.0);
    break;
  case TEXTURED:

    // texture init stuff
    glColor3f(1.0,1.0,1.0);
    GLuint texName;
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // XP
    if ((texData[0] = tex.GetMap_xp()) != NULL) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
		   Map::defaultTexSize, Map::defaultTexSize,
		   0, GL_RGB, GL_UNSIGNED_BYTE, texData[0]);
      glEnable(GL_TEXTURE_2D);
    }
    glBegin(GL_POLYGON);
      glTexCoord2f(1.0, 0.0); glVertex3f(+0.5, +0.5, +0.5); // #4
      glTexCoord2f(0.0, 0.0); glVertex3f(+0.5, -0.5, +0.5); // #7
      glTexCoord2f(0.0, 1.0); glVertex3f(+0.5, -0.5, -0.5); // #1
      glTexCoord2f(1.0, 1.0); glVertex3f(+0.5, +0.5, -0.5); // #0
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // XN
    if ((texData[1] = tex.GetMap_xn()) != NULL) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
		   Map::defaultTexSize, Map::defaultTexSize,
		   0, GL_RGB, GL_UNSIGNED_BYTE, texData[1]);
      glEnable(GL_TEXTURE_2D);
    }
    glBegin(GL_POLYGON);
      glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, +0.5, +0.5); // #5
      glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, +0.5, -0.5); // #3
      glTexCoord2f(1.0, 1.0); glVertex3f(-0.5, -0.5, -0.5); // #2
      glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, -0.5, +0.5); // #6
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // YP
    if ((texData[2] = tex.GetMap_yp()) != NULL) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
		   Map::defaultTexSize, Map::defaultTexSize,
		   0, GL_RGB, GL_UNSIGNED_BYTE, texData[2]);
      glEnable(GL_TEXTURE_2D);
    }      
    glBegin(GL_POLYGON);
      glTexCoord2f(0.0, 0.0); glVertex3f(+0.5, +0.5, +0.5); // #4
      glTexCoord2f(0.0, 1.0); glVertex3f(+0.5, +0.5, -0.5); // #0
      glTexCoord2f(1.0, 1.0); glVertex3f(-0.5, +0.5, -0.5); // #3
      glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, +0.5, +0.5); // #5
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // YN
    if ((texData[3] = tex.GetMap_yn()) != NULL) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
		   Map::defaultTexSize, Map::defaultTexSize,
		   0, GL_RGB, GL_UNSIGNED_BYTE, texData[3]);
      glEnable(GL_TEXTURE_2D);
    }      
    glBegin(GL_POLYGON);
      glTexCoord2f(1.0, 0.0); glVertex3f(+0.5, -0.5, +0.5); // #7
      glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, +0.5); // #6
      glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, -0.5, -0.5); // #2
      glTexCoord2f(1.0, 1.0); glVertex3f(+0.5, -0.5, -0.5); // #1
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // ZP
    if ((texData[4] = tex.GetMap_zp()) != NULL) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
		   Map::defaultTexSize, Map::defaultTexSize,
		   0, GL_RGB, GL_UNSIGNED_BYTE, texData[4]);
      glEnable(GL_TEXTURE_2D);
    }      
    glBegin(GL_POLYGON);
      glTexCoord2f(0.0, 0.0); glVertex3f(+0.5, +0.5, +0.5); // #4
      glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, +0.5, +0.5); // #5
      glTexCoord2f(1.0, 1.0); glVertex3f(-0.5, -0.5, +0.5); // #6
      glTexCoord2f(1.0, 0.0); glVertex3f(+0.5, -0.5, +0.5); // #7
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // ZN
    if ((texData[5] = tex.GetMap_zn()) != NULL) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
		   Map::defaultTexSize, Map::defaultTexSize,
		   0, GL_RGB, GL_UNSIGNED_BYTE, texData[5]);
      glEnable(GL_TEXTURE_2D);
    }
    glBegin(GL_POLYGON);
      glTexCoord2f(0.0, 1.0); glVertex3f(+0.5, +0.5, -0.5); // #0
      glTexCoord2f(1.0, 1.0); glVertex3f(+0.5, -0.5, -0.5); // #1
      glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, -0.5, -0.5); // #2
      glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, +0.5, -0.5); // #3
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glDeleteTextures(1, &texName);

    break;
  default:
    break;
  }
  glPopMatrix();
}

