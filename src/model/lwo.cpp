//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
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
/*
 * Copyright (C) 1998 Janne Löf <jlof@mail.student.oulu.fi>
 */
#include "vreng.hpp"
#include "lwo.hpp"
#include "http.hpp"	// httpOpen
#include "cache.hpp"	// open, close
#include "file.hpp"	// skip


// local
static char lwourl[256];


void Lwo::readSrfs(class File *file, FILE *f, int nbytes)
{
  int cnt = nbm;
  tMaterial *lmaterial = NULL;

  while (nbytes > 0) {
    /* allocate more memory for materials if needed */
    if (cnt <= nbm) {
      cnt += cnt/2 + 4;
      material = static_cast<tMaterial*>(realloc(material,sizeof(tMaterial)*cnt));
      if (! material) { error("can't realloc material"); return; }
    }
    lmaterial = material + nbm++;

    /* read name */
    nbytes -= file->read_string(f, lmaterial->name, LW_MAX_NAME_LEN);

    /* default color */
    lmaterial->r = lmaterial->g = lmaterial->b = 0.7;
  }
  material = static_cast<tMaterial*>(realloc(material, sizeof(tMaterial) * nbm));
  if (! material) error("can't realloc material");
}

void Lwo::readSurf(class File *file, FILE *f, int nbytes)
{
  char name[LW_MAX_NAME_LEN];
  tMaterial *lmaterial = NULL;

  /* read surface name */
  nbytes -= file->read_string(f, name, LW_MAX_NAME_LEN);

  /* find material */
  for (int i=0; i < nbm; i++) {
    if (! strcmp(material[i].name, name)) {
      lmaterial = &material[i];
      break;
    }
  }

  /* read values */
  while (nbytes > 0) {
    int id = file->read_long(f);
    int len = file->read_short(f);
    nbytes -= 6 + len + len%2;

    switch (id) {
    case ID_COLR:
      lmaterial->r = file->read_char(f) / 255.;
      lmaterial->g = file->read_char(f) / 255.;
      lmaterial->b = file->read_char(f) / 255.;
      lmaterial->a = 1;
      file->read_char(f); // dummy
      break;
    default:
      file->skip(f, len+(len%2));
    }
  }
}

void Lwo::readPols(class File *file, FILE *f, int nbytes)
{
  int cnt = nbf;
  tFace *lface = NULL;

  while (nbytes > 0) {
    /* allocate more memory for polygons if necessary */
    if (cnt <= nbf) {
      cnt += cnt + 4;
      face = static_cast<tFace*>(realloc((void*) face, sizeof(tFace) * cnt));
      if (! face) { error("can't realloc face"); return; }
    }
    lface = face + nbf++;

    /* number of points in this face */
    lface->index_count = file->read_short(f);
    nbytes -= 2;

    /* allocate space for points */
    lface->index = static_cast<int*>(calloc(sizeof(int) * lface->index_count, 1));
    if (! lface->index) { error("can't alloc faceindex"); return; }

    /* read points in */
    for (int i=0; i < lface->index_count; i++) {
      lface->index[i] = file->read_short(f);
      nbytes -= 2;
    }

    /* read surface material */
    lface->material = file->read_short(f);
    nbytes -= 2;

    /* skip over detail polygons */
    if (lface->material < 0) {
      lface->material = -lface->material;
      int det_cnt = file->read_short(f);
      nbytes -= 2;
      while (det_cnt-- > 0) {
	int cnt = file->read_short(f);
	file->skip(f, cnt*2+2);
	nbytes -= cnt*2+2;
      }
    }
    lface->material -= 1;
  }

  /* readjust to true size */
  face = static_cast<tFace*>(realloc(face, sizeof(tFace) * nbf));
  if (! face) error("can't realloc face");
}

void Lwo::readPnts(class File *file, FILE *f, int nbytes)
{
  vertex_count = nbytes / 12;
  vertex = new float[vertex_count * 3];
  if (! vertex) { error("can't alloc vertex"); return; }
  for (int i=0; i < vertex_count; i++) {
    vertex[i*3+0] = file->read_float(f);
    vertex[i*3+1] = file->read_float(f);
    vertex[i*3+2] = file->read_float(f);
  }
}

void Lwo::httpReader(void *alwo, Http *http)
{
  Lwo *lwo = static_cast<Lwo *>(alwo);
  if (! lwo) return;

  Cache *cache = new Cache();
  FILE *f = cache->open(lwourl, http);
  if (! f) {
    error("lwo: can't open %s", lwourl);
    return;
  }
  File *file = new(File);

  /* check for headers */
  if (file->read_long(f) != ID_FORM) {
    error("lwoReader: error ID_FORM");
    cache->close();
    delete cache;
    return;
  }
  int32_t form_bytes = file->read_long(f);
  int32_t read_bytes = 4;
  if (file->read_long(f) != ID_LWOB) {
    error("lwoReader: not a LWOB file");
    cache->close();
    delete cache;
    return;
  }

  /* read chunks */
  while (read_bytes < form_bytes) {
    int32_t  chunk  = file->read_long(f);
    int32_t  nbytes = file->read_long(f);
    read_bytes += 8 + nbytes + nbytes%2;
    switch (chunk) {
      case ID_PNTS: lwo->readPnts(file, f, nbytes); break;
      case ID_POLS: lwo->readPols(file, f, nbytes); break;
      case ID_SRFS: lwo->readSrfs(file, f, nbytes); break;
      case ID_SURF: lwo->readSurf(file, f, nbytes); break;
      default: file->skip(f, nbytes + nbytes%2);
    }
  }
  if (file) {
    file->close();
    delete file;
    cache->close();
    delete cache;
  }
  return;
}

Lwo::Lwo(const char *url)
{
  material = NULL;
  face = NULL;
  vertex = NULL;
  nbm = 0;
  nbf = 0;
  vertex_count = 0;
  dlist = 0;

  strcpy(lwourl, url);
  Http::httpOpen(url, httpReader, this, 0);
  return;
}

Lwo::~Lwo()
{
  if (face) free(face);
  face = NULL;
  if (material) free(material);
  material = NULL;
  if (vertex) delete[] vertex;
  if (dlist > 0) glDeleteLists(dlist, 1);
}

#define PX(p) (vertex[lface->index[p]*3+0])
#define PY(p) (vertex[lface->index[p]*3+1])
#define PZ(p) (vertex[lface->index[p]*3+2])

void Lwo::displaylist()
{
  int prev_index_cnt = -1;
  int prev_material  = -1;

  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);

  for (int i=0; i < nbf; i++) {
    const tFace *lface = face + i;

    if (lface->index_count < 3) continue; // ignore faces with less than 3 points

    /* glBegin/glEnd */
    glEnable(GL_NORMALIZE);
    if (prev_index_cnt != lface->index_count || prev_index_cnt > 4) {
      if (prev_index_cnt > 0) glEnd();
      prev_index_cnt = lface->index_count;
      switch (lface->index_count) {
        case 3:  glBegin(GL_TRIANGLES); break;
        case 4:  glBegin(GL_QUADS); break;
        default: glBegin(GL_POLYGON); break;
      }
    }

    /* update material if necessary */
    if (prev_material != lface->material) {
      prev_material = lface->material;
      GLfloat color[4];
      color[0] = material[lface->material].r;
      color[1] = material[lface->material].g;
      color[2] = material[lface->material].b;
      color[3] = material[lface->material].a;
      glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color);
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
    }

#if 0 //dax
    /* calculate normal */
    GLfloat prev_nx = 0;
    GLfloat prev_ny = 0;
    GLfloat prev_nz = 0;
    GLfloat ax = PX(1) - PX(0);
    GLfloat ay = PY(1) - PY(0);
    GLfloat az = PZ(1) - PZ(0);
    GLfloat bx = PX(lface->index_count-1) - PX(0);
    GLfloat by = PY(lface->index_count-1) - PY(0);
    GLfloat bz = PZ(lface->index_count-1) - PZ(0);
    GLfloat nx = ay*bz - az*by;
    GLfloat ny = az*bx - ax*bz;
    GLfloat nz = ax*by - ay*bx;
    GLfloat r = sqrt(nx*nx + ny*ny + nz*nz);

    if (r < 0.000001) continue;  // avoid division by zero
    nx /= r; ny /= r; nz /= r;

    /* update normal if necessary */
    if (nx != prev_nx || ny != prev_ny || nz != prev_nz) {
      prev_nx = nx;
      prev_ny = ny;
      prev_nz = nz;
      glNormal3f(nx, ny, nz);
    }
#endif

    /* draw polygon/triangle/quad */
    for (int j=0; j < lface->index_count; j++) {
      glVertex3f(vertex[lface->index[j]*3+0], vertex[lface->index[j]*3+1], vertex[lface->index[j]*3+2]);
    }
  }

  /* if glBegin was called then call glEnd */
  if (prev_index_cnt > 0) glEnd();
  glDisable(GL_NORMALIZE);
  glEndList();
}

void Lwo::render()
{
  if (dlist) {
    glPushMatrix();
    glCallList(dlist);
    glPopMatrix();
  }
}

void Lwo::render(const Pos &pos)
{
  if (dlist) {
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(RAD2DEG(pos.az), 0, 0, 1);
    glRotatef(RAD2DEG(pos.ax), 1, 0, 0);
    glCallList(dlist);
    glPopMatrix();
  }
}

float Lwo::getScale()
{
  float max_radius = 0;

  for (int i=0; i < vertex_count; i++) {
    float *v = &vertex[i*3];
    float r = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    if (r > max_radius) max_radius = r;
  }
  return sqrt(max_radius);
}

void Lwo::setScale(float _scale)
{
  for (int i=0; i < vertex_count; i++) {
    vertex[i*3+0] *= _scale;
    vertex[i*3+1] *= _scale;
    vertex[i*3+2] *= _scale;
  }
}
