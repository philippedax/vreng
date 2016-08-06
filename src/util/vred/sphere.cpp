#include <GL/glut.h>

#include "vred.hpp"
#include "sphere.hpp"


Sphere::Sphere(char* _name,
	       const Vect& _center,
	       const Vect& _orientation,
	       const Vect& _size,
	       const int _renderStyle, const Color& _color,
	       const Tex& _tex, const App& _app,
	       const int _slices, const int _stacks):
  Solid(_name, _center, _orientation, _size, _renderStyle, _color, _tex, _app),
  slices(_slices), stacks(_stacks), vertices(NULL)
{
  myBoundingBox.Set(_center,_size[0],_size[1],_size[2],0);
  myBoundingSphere.Set(_center,_size[0]);
}

Sphere::~Sphere() {
  if (vertices != NULL)
    delete[] vertices;
}

void Sphere::Render() {
//   color.Apply();

  // TODO: to be tested ...or removed !!!
  glMaterialfv(GL_FRONT, GL_AMBIENT, app.GetAmbient());
  glMaterialfv(GL_FRONT, GL_DIFFUSE, app.GetDiffuse());
  glMaterialfv(GL_FRONT, GL_SPECULAR, app.GetSpecular());
  glMaterialfv(GL_FRONT, GL_SHININESS, app.GetShininess());

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMultMatrixd(modelView);

  const GLvoid* texData;

  switch(renderStyle) {
  case WIRED:
    glutWireSphere(1.0, slices, stacks);
    break;
  case COLORED:
    glutSolidSphere(1.0, slices, stacks);
    break;
  case TEXTURED:

    // create the vertices of the sphere
    if (vertices == NULL) {
      cout << "creating the sphere vertices" << endl;
      vertices = new GLfloat[(1 + slices * (stacks - 1) + 1) * 3];
      
      // top vertex    
      vertices[0] = 0.0; vertices[1] = 0.0; vertices[2] = 1.0;
    
      // body vertices
      for (int iz = 1; iz < stacks; ++iz) {
	float z = 1.0 - iz * (2.0 / stacks);
	for (int ia = 0; ia < slices; ++ia) {
	  float a = ia * 360 / slices, sine = sqrt(1 - z * z); 
	  vertices[(1 + ia + (iz - 1) * slices) * 3 + 0] = sine * COS(a);
	  vertices[(1 + ia + (iz - 1) * slices) * 3 + 1] = sine * SIN(a);
	  vertices[(1 + ia + (iz - 1) * slices) * 3 + 2] = z;
	}
      }

      // bottom vertex
      vertices[(1 + slices * (stacks - 1)) * 3 + 0] = 0.0;
      vertices[(1 + slices * (stacks - 1)) * 3 + 1] = 0.0;
      vertices[(1 + slices * (stacks - 1)) * 3 + 2] = -1.0;
    }

    // texture init stuff
    glColor3f(1.0,1.0,1.0);
    GLuint texName;
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    if ((texData = tex.GetMap_xp()) != NULL) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
		   Map::defaultTexSize, Map::defaultTexSize,
		   0, GL_RGB, GL_UNSIGNED_BYTE, texData);
      glEnable(GL_TEXTURE_2D);
    }

    // render the top
    for (int t = 0; t < slices; ++t) {
      float txl = ((float) t) / ((float) slices);
      float txr = ((float) (t + 1)) / ((float) slices);
      float txt = ((((float) t) + 0.5)) / ((float) slices);
      glBegin(GL_TRIANGLES);
        glTexCoord2f(txt, 0.0); 
        glVertex3f(vertices[0], vertices[1], vertices[2]);
        glTexCoord2f(txl, 1.0 / ((float) stacks)); 
	glVertex3f(vertices[(1 + t) * 3 + 0],
		   vertices[(1 + t) * 3 + 1],
		   vertices[(1 + t) * 3 + 2]);
        glTexCoord2f(txr, 1.0 / ((float) stacks)); 
	glVertex3f(vertices[(1 + (t + 1) % slices) * 3 + 0],
		   vertices[(1 + (t + 1) % slices) * 3 + 1],
		   vertices[(1 + (t + 1) % slices) * 3 + 2]);
      glEnd();
    }

    // render the body
    for (int bz = 0; bz < stacks - 2; ++bz) {
      float tyu = ((float) (bz + 1)) / ((float) stacks);
      float tyd = ((float) (bz + 2)) / ((float) stacks);
      glBegin(GL_QUAD_STRIP);
        for (int ba = 0; ba < slices + 1; ++ba) {
	  float tx = ((float) ba) / ((float) slices);
          glTexCoord2f(tx, tyu); 
	  glVertex3f(vertices[(1 + bz * slices + ba % slices) * 3 + 0],
		     vertices[(1 + bz * slices + ba % slices) * 3 + 1],
		     vertices[(1 + bz * slices + ba % slices) * 3 + 2]);
          glTexCoord2f(tx, tyd); 
	  glVertex3f(vertices[(1 + (bz + 1) * slices + ba % slices) * 3 + 0],
		     vertices[(1 + (bz + 1) * slices + ba % slices) * 3 + 1],
		     vertices[(1 + (bz + 1) * slices + ba % slices) * 3 + 2]);
	}
      glEnd();
    }

    // render the bottom
    for (int b = 0; b < slices; ++b) {
      float bxl = ((float) b) / ((float) slices);
      float bxr = ((float) (b + 1)) / ((float) slices);
      float bxb = ((((float) b) + 0.5)) / ((float) slices);
      glBegin(GL_TRIANGLES);
        glTexCoord2f(bxl, ((float)(stacks - 1)) / ((float) stacks)); 
	glVertex3f(vertices[(1 + slices * (stacks - 2) + b) * 3 + 0],
		   vertices[(1 + slices * (stacks - 2) + b) * 3 + 1],
		   vertices[(1 + slices * (stacks - 2) + b) * 3 + 2]);
        glTexCoord2f(bxb, 1.0); 
        glVertex3f(vertices[(1 + slices * (stacks - 1)) * 3 + 0],
		   vertices[(1 + slices * (stacks - 1)) * 3 + 1],
		   vertices[(1 + slices * (stacks - 1)) * 3 + 2]);
        glTexCoord2f(bxr, ((float)(stacks - 1)) / ((float) stacks)); 
	glVertex3f(vertices[(1 + slices * (stacks-2) + (b+1)%slices) * 3 + 0],
		   vertices[(1 + slices * (stacks-2) + (b+1)%slices) * 3 + 1],
		   vertices[(1 + slices * (stacks-2) + (b+1)%slices) * 3 + 2]);
      glEnd();
    }
    
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &texName);

    break;

  default:
    break;
  }
  glPopMatrix();
}

void Sphere::SetSize(const Vect& size) {
  // constraint the sphere to be a sphere...
  double radius = (size[0]+size[1]+size[2])/3;
  Solid::SetSize(Vect(radius, radius, radius));
}
