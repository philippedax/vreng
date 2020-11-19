//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
// Telecom-ParisTech (Ecole Nationale Superieure des Telecommunications)
//
// VREng is a free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public Licence as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// VREng is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "terrain.hpp"


const OClass Terrain::oclass(TERRAIN_TYPE, "Terrain", Terrain::creator);

const uint8_t Terrain::DEF_LEVEL = 6;
const GLfloat Terrain::DEF_HEIGHT = 0.1;
const GLfloat Terrain::DEF_DIV = 1.85;
const GLfloat Terrain::DEF_SCALE = 100;
const GLfloat Terrain::DEF_COLOR[4] = {0,1,0,1};	// green


WObject * Terrain::creator(char *l)
{
  return new Terrain(l);
}

void Terrain::defaults()
{
  level = DEF_LEVEL;
  height = DEF_HEIGHT;
  div = DEF_DIV;
  scale = DEF_SCALE;
  mul = 1;
  for (int i=0; i<4; i++) color[i] = DEF_COLOR[i];
}

void Terrain::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (!stringcmp(l, "height")) l = parse()->parseFloat(l, &height, "height");
    else if (!stringcmp(l, "color"))  l = parse()->parseVector3f(l, color, "color");
    else if (!stringcmp(l, "level"))  l = parse()->parseUInt8(l, &level, "level");
    else if (!stringcmp(l, "div"))    l = parse()->parseFloat(l, &div, "div");
    else if (!stringcmp(l, "scale"))  l = parse()->parseFloat(l, &scale, "scale");
  }
  end_while_parse(l);
}

void Terrain::behavior()
{
  enableBehavior(NO_BBABLE);
  enableBehavior(SPECIFIC_RENDER);
  setRenderPrior(RENDER_LOW);

  initStillObject();
}

void Terrain::inits()
{
  if (level > 10) level = DEF_LEVEL;
  if (div <= 1 || div > 3) div = DEF_DIV;
  size = level*level;
  mesh = new GLfloat[size*size*sizeof(GLfloat)+1];
  normales = new nVect[size*size*sizeof(nVect)+1];
  srand(time(0));
  heights(level, 0, 0, height, div);
  initNormales();
  draw();
}

Terrain::Terrain(char *l)
{
  parser(l);
  behavior();
  inits();
}

void Terrain::heights(int l, int x, int y, float h, float f)
{
  if (l == 0) return;

  int w = l * l;
  if (y == 0) aux_heights(x, y, x+w, y, h);
  if (x == 0) aux_heights(x, y, x, y+w, h);
  aux_heights(x+w, y, x+w, y+w, h);
  aux_heights(x, y+w, x+w, y+w, h);
  mesh[(x+w/2)*size+y+w/2] = (mesh[x*size+y]+mesh[(x+w)*size+y]+mesh[x*size+y+w]+mesh[(x+w)*size+y+w])/4;
  float m = f * mul;
  heights(l-1, x, y, h/f, m);
  heights(l-1, x+w/2, y, h/f, m);
  heights(l-1, x, y+w/2, h/f, m);
  heights(l-1, x+w/2, y+w/2, h/f, m);
}

void Terrain::aux_heights(int x1, int y1, int x2, int y2, float h)
{
  mesh[(x1+x2)/2*size + (y1+y2)/2] = (mesh[x1*size +y1] + mesh[x2*size +y2])/2 + h*((rand()%21)-10)/10;
}

void Terrain::prodvect(float x1, float y1, float z1, float x2, float y2, float z2, float *px, float *py, float *pz)
{
  *px = y1*z2 - z1*y2;
  *py = z1*x2 - x1*z2;
  *pz = x1*y2 - y1*x2;
}

void Terrain::initNormales()
{
  for (int i=0; i<size ; i++) {
    for (int j=0; j<size ; j++) {
      float px,py,pz;
      prodvect(2./size, 0, mesh[(i+1)*size+j]-mesh[(i-1)*size+j], 0, 2./size, mesh[i*size+j+1]-mesh[i*size+j-1], &px, &py, &pz);
      normales[i*size+j].x = px;
      normales[i*size+j].y = py;
      normales[i*size+j].z = pz;
    }
  }
}

void Terrain::draw()
{
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  glBegin(GL_QUADS);
  for (int i=0; i<size ; i++) {
    for (int j=0; j<size ; j++) {
      GLfloat nx, ny, nz;

      setColor(mesh[i*size+j]);
      nx = normales[i*size+j].x;
      ny = normales[i*size+j].y;
      nz = normales[i*size+j].z;
      glNormal3f(nx, ny, nz);
      glVertex3f((GLfloat)i/size, (GLfloat)j/size, mesh[i*size+j]);
      setColor(mesh[(i+1)*size+j]);
      nx = normales[(i+1)*size+j].x;
      ny = normales[(i+1)*size+j].y;
      nz = normales[(i+1)*size+j].z;
      glNormal3f(nx, ny, nz);
      glVertex3f((GLfloat)(i+1)/size, (GLfloat)j/size, mesh[(i+1)*size+j]);
      setColor(mesh[(i+1)*size+j+1]);
      nx = normales[(i+1)*size+j+1].x;
      ny = normales[(i+1)*size+j+1].y;
      nz = normales[(i+1)*size+j+1].z;
      glNormal3f(nx, ny, nz);
      glVertex3f((GLfloat)(i+1)/size, (GLfloat)(j+1)/size, mesh[(i+1)*size+j+1]);
      setColor(mesh[i*size+j+1]);
      nx = normales[i*size+j+1].x;
      ny = normales[i*size+j+1].y;
      nz = normales[i*size+j+1].z;
      glNormal3f(nx, ny, nz);
      glVertex3f((GLfloat)i/size, (GLfloat)(j+1)/size, mesh[i*size+j+1]);
    }
  }
  glEnd();      
  glEndList();
}

void Terrain::render()
{
  //DAX glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPushMatrix();
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);

  glTranslatef(pos.x, pos.y, pos.z);
  glScalef(scale, scale, scale);

  glCallList(dlist);

  glDisable(GL_LIGHTING);
  glPopMatrix();
  //DAX glPopAttrib();
}

void Terrain::setColor(float z)
{
  GLfloat r,g,b;

  if (z > 0) {
    r = 0.0009 / (0.001+z*z) + z*z/0.05; 
    g = 0.4 + (z+0.1) * (z-0.12) * (z-0.15)*130;
    b = 0.1 + z*z*(z-0.1) / 0.018;
    glColor4f(r,g,b,1);
  }
  else {
    glColor4fv(color);
  }
}

void Terrain::quit()
{
  glDeleteLists(dlist, 1);
  if (mesh) delete[] mesh;
  if (normales) delete[] normales;
} 

void Terrain::funcs()
{
} 
