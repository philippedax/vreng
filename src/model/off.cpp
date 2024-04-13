//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
// Telecom-Paris (Ecole Nationale Superieure des Telecommunications)
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
// off.cpp
//
// Model .off
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "off.hpp"
#include "http.hpp"	// Http::httpOpen
#include "wobject.hpp"	// Pos
#include "cache.hpp"	// open, close


void Off::defaults()
{
  vn = nn = pn = 0;
  v = n = NULL;
  p = NULL;
  url = NULL;
  dlist = 0;
}

/** Constructor */
Off::Off(float _scale) : scale(_scale)
{
  defaults();
}

Off::Off(const char *_url) : scale(1)
{
  defaults();
  url = new char[strlen(_url) + 1];
  strcpy(url, _url);
  Http::httpOpen(url, reader, this, 0);
}

/** Destructor */
Off::~Off()
{
  if (v) delete[] v;
  if (n) delete[] n;
  if (p) delete[] p;
  if (dlist > 0) glDeleteLists(dlist, 1);
  if (url) delete[] url;
}

/** Sets scale */
void Off::setScale(float _scale)
{
  scale = _scale;
  for (int i=0; i < vn; i++) {
    v[i*3+0] *= scale;
    v[i*3+1] *= scale;
    v[i*3+2] *= scale;
  }
  for (int i=0; i < nn; i++) {
    n[i*3+0] *= scale;
    n[i*3+1] *= scale;
    n[i*3+2] *= scale;
  }
}

/** Off Reader - static */
void Off::reader(void *_off, Http *http)
{
  Off *off = static_cast<Off *>(_off);
  if (! off) return;

  Cache *cache = new Cache();
  FILE *f = cache->open(http->url, http);
  if (! f) {
    error("can't read %s", http->url);
    delete cache;
    return;
  }

  char line[80];
  /* Get info header: vertices_number normals_number polygons_number */
  fgets(line, sizeof(line), f);
  getOff3i(line, &off->vn, &off->nn, &off->pn);
  fgets(line, sizeof(line), f);  // skip empty line

  /* Gets vertices */
  off->v = new float[3 * off->vn];
  for (int i=0; i < off->vn; i++) {
    fgets(line, sizeof(line), f);
    getOff3f(line, &off->v[i*3+0], &off->v[i*3+1], &off->v[i*3+2], off->scale);
  }
  fgets(line, sizeof(line), f);  // skip empty line

  /* Gets normals */
  off->n = new float[3 * off->nn];
  for (int i=0; i < off->nn; i++) {
    fgets(line, sizeof(line), f);
    getOff3f(line, &off->n[i*3+0], &off->n[i*3+1], &off->n[i*3+2], off->scale);
  }
  fgets(line, sizeof(line), f);  // skip empty line

  /* Gets polygons */
  off->p = new int[6 * off->pn];
  for (int i=0; i < off->pn; i++) {
    fgets(line, sizeof(line), f);
    int k = strlen(line);
    for (int j=0; j < k; j++) {
      int a = 0;
      if (line[j] == 'v')
        a = j+2;
      if (line[j] == 'n') {
        line[j-1] = 0;
        getOff3i(&line[a+2], &off->p[i*6+0], &off->p[i*6+1], &off->p[i*6+2]);
        getOff3i(&line[j+2], &off->p[i*6+3], &off->p[i*6+4], &off->p[i*6+5]);
        break;
      }
    }
  }
  cache->close();
  delete cache;
}

/** Draws in the display list */
GLint Off::displaylist()
{
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  draw();
  glEndList();
  return dlist;
}

/** Draw off */
void Off::draw()
{
  glBegin(GL_TRIANGLES);
  for (int i=0; i < pn; i++) {
    int32_t v1 = p[i*6+0];
    int32_t v2 = p[i*6+1];
    int32_t v3 = p[i*6+2];
    int32_t n1 = p[i*6+3];
    int32_t n2 = p[i*6+4];
    int32_t n3 = p[i*6+5];

    glNormal3f(n[n1*3+0], n[n1*3+1], n[n1*3+2]);
    glVertex3f(v[v1*3+0], v[v1*3+1], v[v1*3+2]);
    glNormal3f(n[n2*3+0], n[n2*3+1], n[n2*3+2]);
    glVertex3f(v[v2*3+0], v[v2*3+1], v[v2*3+2]);
    glNormal3f(n[n3*3+0], n[n3*3+1], n[n3*3+2]);
    glVertex3f(v[v3*3+0], v[v3*3+1], v[v3*3+2]);
  }
  glEnd();
}

/** Renders with color */
void Off::render(float *color)
{
  glPushMatrix();
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glRotatef(-90, 1,0,0);
  glColor3fv(color);

  glCallList(dlist);

  glDisable(GL_COLOR_MATERIAL);
  glPopMatrix();
}

/** Renders with pos and color */
void Off::render(const Pos &pos, float *color)
{
  glPushMatrix();
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glTranslatef(pos.x, pos.y, pos.z);
  glRotatef(RAD2DEG(pos.az), 0,0,1);
  glRotatef(RAD2DEG(pos.ax), 1,0,0);
  glRotatef(RAD2DEG(pos.ay), 0,1,0);
  glColor3fv(color);

  glCallList(dlist);

  glDisable(GL_COLOR_MATERIAL);
  glPopMatrix();
}

/** Returns a b c floats */
void Off::getOff3i(char *s, int *a, int *b, int *c)
{
  int k = strlen(s);
  int x=0, y=-1, z=-1;

  for (int i=0; i < k; i++) {
    if ((s[i] == ' ') || (s[i] == '\n')) {
      s[i] = 0;
      if (y == -1)
        y = i+1;
      else if (z == -1)
        z = i+1;
    }
  }
  *a = atoi(&s[x]);
  *b = atoi(&s[y]);
  *c = atoi(&s[z]);
}

/** Returns a b c floats */
void Off::getOff3f(char *s, float *a, float *b, float *c, float scale)
{
  int k = strlen(s);
  int x=0, y=-1, z=-1;

  for (int i=0; i < k; i++) {
    if ((s[i] == ' ') || (s[i] == '\n')) {
      s[i] = 0;
      if (y == -1)
        y = i+1;
      else if (z == -1)
        z = i+1;
    }
  }
  *a = static_cast<float>(atof(&s[x]) / scale);
  *b = static_cast<float>(atof(&s[y]) / scale);
  *c = static_cast<float>(atof(&s[z]) / scale);
}
