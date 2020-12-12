//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
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
#include "walls.hpp"
#include "http.hpp"	// httpOpen
#include "cache.hpp"	// openCache
#include "file.hpp"	// closeFile


const OClass Walls::oclass(WALLS_TYPE, "Walls", Walls::creator);

// local
static Walls *wallsList;      // walls list
static uint16_t numwall;      // number of walls

void Walls::funcs() {}


WObject * Walls::creator(char *l)
{
  return new Walls(l);
}

/** Walls loader */
void Walls::httpReader(void *_walls, Http *http)
{
  Walls *walls = (class Walls *) _walls;
  if (! walls) return;

  Walls *pw = NULL, *prev = NULL;
  float xs, xe, ys, ye, zs, ze, r, g, b;
  char line[BUFSIZ];

  FILE *f = Cache::openCache(walls->getUrl(), http);
  if (! f) { error("can't open %s", walls->getUrl()); return; }

  while (fgets(line, sizeof(line), f)) {
    if (*line == '#') continue;
    line[strlen(line) -1] = '\0';
    if (! strlen(line)) continue;
    if (sscanf(line, "%f %f %f %f %f %f  %f %f %f",
	             &xs, &xe, &ys, &ye, &zs, &ze, &r, &g, &b) <= 0) {
      break;	// EOF
    }

    pw = new Walls(xs, xe, ys, ye, zs, ze, r, g, b);

    if (! prev) wallsList = pw;
    else prev->next = pw;
    prev = pw;
  }
  if (pw) pw->next = NULL;
  File::closeFile(f);
}

void Walls::defaults()
{
  wallsList = NULL;
  numwall = 0;
}

const char * Walls::getUrl() const
{
  return (const char *) names.url;
}

void Walls::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (!stringcmp(l, "url")) l = parse()->parseUrl(l, names.url);
  }
  end_while_parse(l);
}

/* Constructors */
Walls::Walls(char *l)
{
  parser(l);

  enableBehavior(NO_BBABLE);
  enableBehavior(UNSELECTABLE);
  enableBehavior(SPECIFIC_RENDER);
  setRenderPrior(PRIOR_MEDIUM);

  initStillObject();

  Http::httpOpen(names.url, httpReader, this, 0);

  draw();	// draws the walls
}

Walls::Walls(float _xs, float _xe, float _ys, float _ye, float _zs, float _ze, float _r, float _g, float _b)
{
  xs = _xs;
  xe = _xe;
  ys = _ys;
  ye = _ye;
  zs = _zs;
  ze = _ze;
  r  = _r;
  g  = _g;
  b  = _b;
  next = NULL;
  numwall++;
}

static float cmpnormal(V3 *a, V3 *b)
{
  float rx = a->v[0] - b->v[0];
  float ry = a->v[1] - b->v[1];
  float rz = a->v[2] - b->v[2];

  return rx*rx + ry*ry + rz*rz;
}

static float dotprod(float ax, float ay, float bx, float by)
{
  return ax*bx + ay*by;
}

/* Intersection with an object */
int Walls::whenIntersect(const V3& center, const V3& size, V3& normal)
{
  Walls *walls = wallsList;
  V3 cur_normal;
  bool first = true;

  float oxmin = center.v[0] - size.v[0];
  float oxmax = center.v[0] + size.v[0];
  float oymin = center.v[1] - size.v[1];
  float oymax = center.v[1] + size.v[1];
  float ozmin = center.v[2] - size.v[2];
  float ozmax = center.v[2] + size.v[2];

  // we test the collision with all the walls
  while (walls) {
    float wxmin = (walls->xs < walls->xe) ? walls->xs : walls->xe;
    float wxmax = (walls->xs > walls->xe) ? walls->xs : walls->xe;
    float wymin = (walls->ys < walls->ye) ? walls->ys : walls->ye;
    float wymax = (walls->ys > walls->ye) ? walls->ys : walls->ye;
    float wzmin = (walls->zs < walls->ze) ? walls->zs : walls->ze;
    float wzmax = (walls->zs > walls->ze) ? walls->zs : walls->ze;

    if ((((oxmin < wxmin) && (oxmax > wxmax)) ||
	 ((oxmin > wxmin) && (oxmax < wxmax)) ||
	 ((oxmax > wxmin) && (oxmax < wxmax)) ||
	 ((oxmin > wxmin) && (oxmin < wxmax)))
	&&
	(((oymin < wymin) && (oymax > wymax)) ||
	 ((oymin > wymin) && (oymax < wymax)) ||
	 ((oymax > wymin) && (oymax < wymax)) ||
	 ((oymin > wymin) && (oymin < wymax)))
	&&
	(((ozmin < wzmin) && (ozmax > wzmax)) ||
	 ((ozmin > wzmin) && (ozmax < wzmax)) ||
	 ((ozmax > wzmin) && (ozmax < wzmax)) ||
	 ((ozmin > wzmin) && (ozmin < wzmax)))) {
      cur_normal.v[2] = 0.;
      if (wxmin == wxmax) {
	cur_normal.v[0] = 1.;
	cur_normal.v[1] = 0.;
      }
      else {
	cur_normal.v[1] = 1.;
	cur_normal.v[0] = 0.;
      }

      // determine which normal we take
      if (dotprod(center.v[0] - wxmin, center.v[1] - wymin,
		  cur_normal.v[0], cur_normal.v[1]) < 0.) {
	cur_normal.v[0] = -cur_normal.v[0];
	cur_normal.v[1] = -cur_normal.v[1];
      }
      if (first) {
	normal = cur_normal;
	first = false;
      }
      else {
	if (cmpnormal(&cur_normal, &normal) > 0.01) return 2;
      }
    }
    walls = walls->next;
  }
  if (first) return 0; // no collision
  else return 1;
}

/* Draw walls */
void Walls::draw()
{
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  for (Walls *walls = wallsList; walls; walls = walls->next) {
    glColor3f(walls->r, walls->g, walls->b);
    glBegin(GL_QUADS);
     glVertex3f(walls->xs, walls->ys, walls->zs);
     glVertex3f(walls->xs, walls->ys, walls->ze);
     glVertex3f(walls->xe, walls->ye, walls->ze);
     glVertex3f(walls->xe, walls->ye, walls->zs);
    glEnd();
  }
  glEndList();
}

/* Render walls */
void Walls::render()
{
  glPushMatrix();
  glDisable(GL_LIGHTING);	// no lighting on walls
  glDisable(GL_CULL_FACE);	// two facets => no culling

  glCallList(dlist);

  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

/* Free the walls list */
void Walls::quit()
{
  while (wallsList) {
    Walls *nextwall = wallsList->next;

    delete wallsList;
    wallsList = nextwall;
  }
  wallsList = NULL;
  numwall = 0;
  glDeleteLists(dlist, 1);
}
