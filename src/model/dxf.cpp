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
/*------------------------------------------------------------------------
                                /\
                             __/__\__
                            /\      /\
                           /  \    /  \
                          /    \  /    \
                         /______\/______\
                          /____________\
                          M   M OOOO X  X
                          M M M 0  0  XX
                          M   M OOOO X  X

          MOX - Centro de Computacion Avanzada en Ingenieria.
                        Facultad de Ingenieria
                       Universidad de los Andes
                         Bogota, Colombia.
 Autor: Daniel Barrero                                  Fecha: Abril-1996
------------------------------------------------------------------------*/
#include "vreng.hpp"
#include "dxf.hpp"
#include "http.hpp"	// httpOpen
#include "cache.hpp"	// open
#include "file.hpp"	// open, close


// local
File *filein;


Dxf::Dxf(const char *_url)
 : loaded(false), currentScale(1.0), desiredScale(1.0)
{
  flgcolor = false;
  filein = NULL;
  for (int i=0; i<4 ; i++) {
    mat_diffuse[i] = 1;
    mat_ambient[i] = 1;
    mat_specular[i] = 1;
  }

  url = new char[strlen(_url) + 1];
  strcpy(url, _url);
  Http::httpOpen(url, httpReader, this, 0);
}

Dxf::~Dxf()
{
  deleteDXF(dxffile);
  if (url) delete[] url;
  //dax segfault if (dlist > 0) glDeleteLists(dlist, 1);
}

const char * Dxf::getUrl() const
{
  return (const char *) url;
}

/* http loader - static */
void Dxf::httpReader(void *_dxf, Http *http)
{
  Dxf *dxf = (Dxf *) _dxf;
  if (! dxf) return;

  char filename[PATH_LEN] = {0};
  Cache::setCachePath(dxf->getUrl(), filename);
  dxf->dxffile = newDXF(filename);

  Cache *cache = new Cache();
  FILE *f = cache->open(dxf->getUrl(), http);
  if (! f) {
    error("dxf: can't open %s", dxf->getUrl());
    return;
  }
  dxf->dxffile->fp = f;
  readDXF(dxf->dxffile);
  dxf->loaded = true;
}

void Dxf::bindTexture2D(int texid)
{
  if (texid > 0) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texid);
  }
}

void Dxf::setScale(float scale)
{
  if (scale != currentScale) {
    if (! loaded) desiredScale = scale;
    else { currentScale = desiredScale = scale; }
  }
}

float Dxf::getRadius()
{
  float max_radius = 0.0;
#if 0
  for (int i=0; i < dxf->objects->num_objects; i++) {
    PolyMeshObj *pmesh = dxf->objects->objects[i];
    for (int j=0; j < pmesh->num_polygons; j++) {
      Polygon *pol = pmesh->polygons[j];
      if (pol && pol->num_points > 0) {
        for (int k=0; k < pol->num_points; k++) {
          if (k==0 || (k>0 && pol->points.refs[k] != pol->points.refs[k-1])) {
            double r= pol->points.refs[k]->x * pol->points.refs[k]->x +
                      pol->points.refs[k]->y * pol->points.refs[k]->y +
                      pol->points.refs[k]->z * pol->points.refs[k]->z ;
            if (r > max_radius)
              max_radius = r;
          }
        }
      }
    }
  }
#endif
  return sqrt(max_radius);
}

void Dxf::setColor(int type, float *color)
{
  flgcolor = true;
  switch (type) {
    case GL_DIFFUSE: for (int i=0; i<4 ; i++) mat_diffuse[i] = color[i]; break;
    case GL_AMBIENT: for (int i=0; i<4 ; i++) mat_ambient[i] = color[i]; break;
    case GL_SPECULAR: for (int i=0; i<4 ; i++) mat_specular[i] = color[i]; break;
    case GL_EMISSION: break;
  }
}

bool Dxf::draw(DXF_file *dxffile)
{
  if (! loaded) return false;
  if (! dxffile) return false;

  if (! dxffile->objects || (dxffile->objects && dxffile->objects->num_objects <= 0)) {
    error("dxf: nothing to draw !");
    return false;
  }

  if (currentScale != desiredScale) setScale(desiredScale);
  if (flgcolor) {
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  }

  Point3D *v1 = newPoint3D(1,0,0);
  Point3D *v2 = newPoint3D(0,1,0);
  glEnable(GL_NORMALIZE);

  for (int i=0 ; i < dxffile->objects->num_objects ; i++) {
    PolyMeshObj *pmesh = dxffile->objects->objects[i];
    if (pmesh) {
      for (int j=0; j < pmesh->num_polygons; j++) {
        Polygon *pol = pmesh->polygons[j];
        if (pol && pol->num_points > 0) {

          glBegin(GL_POLYGON);
          for (int k=0; k < pol->num_points; k++) {
            if (k==0 || (k>0 && pol->points.refs[k] != pol->points.refs[k-1])) {
              glVertex3d(pol->points.refs[k]->x,
                         pol->points.refs[k]->y,
                         pol->points.refs[k]->z);
              //error("i=%d j=%d k=%d %.2f %.2f %.2f", i,j,k,pol->points.refs[k]->x,pol->points.refs[k]->y,pol->points.refs[k]->z);
            }
            glTexCoord3d(pol->points.refs[k]->x,
                         pol->points.refs[k]->y,
                         pol->points.refs[k]->z);
          }
          if (pol->num_points >= 3) {
            copyPoint3D(v1, pol->points.refs[2]);
            copyPoint3D(v2, pol->points.refs[1]);
            subPoint3D(v1, pol->points.refs[0]);
            subPoint3D(v2, pol->points.refs[0]);
            glNormal3d(v1->y *(v2->z)-v1->z*(v2->y),
                      -v1->x*(v2->z)+v1->z*(v2->x),
                       v1->x*(v2->y)-v1->y*(v2->x));
          }
          glEnd();
        }
      }
    }
  }
  deletePoint3D(v1);
  deletePoint3D(v2);
  return true;
}

// Draws in the display list
GLint Dxf::displaylist()
{
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  draw(dxffile);
  glEndList();
  return dlist;
}

/* Display the wire frame described by the face list pointed to by CurObj. */
void Dxf::render()
{
  if (dlist) {
    glPushMatrix();
    glScalef(currentScale, currentScale, currentScale);
    glRotatef(-90, 1,0,0);
    glCallList(dlist);
    glPopMatrix();
  }
}

/*------------------  Object Point3D -----------------------------------*/
Point3D * newPoint3D(float x, float y, float z)
{
  Point3D *p = new Point3D[1];
  return setPoint3D(p, x, y, z);
}

Point3D * deletePoint3D (Point3D *p)
{
  if (p) delete[] p;
  return (p=NULL);
}

Point3D * setPoint3D(Point3D *p, float x, float y, float z)
{
  if (p) {
    p->x = x;
    p->y = y;
    p->z = z;
  }
  return p;
}

Point3D * copyPoint3D(Point3D *p1, Point3D *p2)
{
  if (!p1 || !p2) return NULL;
  return setPoint3D(p1, p2->x, p2->y, p2->z);
}

Point3D * addPoint3D(Point3D *p1, Point3D *p2)
{
  if (!p1 || !p2) return NULL;
  return setPoint3D(p1, p1->x+p2->x, p1->y+p2->y, p1->z+p2->z);
}

Point3D * subPoint3D(Point3D *p1, Point3D *p2)
{
  if (!p1 || !p2) return NULL;
  return setPoint3D(p1, p1->x-p2->x, p1->y-p2->y, p1->z-p2->z);
}

Point3D * addPoint3DFPOINT(Point3D *p, float s)
{
  return (p) ? setPoint3D(p, p->x+s, p->y+s, p->z+s) : p;
}

Point3D * multPoint3DFPOINT(Point3D *p, float f)
{
  return (p) ? setPoint3D(p, p->x*f, p->y*f, p->z*f) : p;
}

/*------------------  Object Surface  ----------------------------------*/
Surface * newSurface(float ambient, float diffuse, float brilliance,
                     float specular, float roughness, float reflection,
                     float refraction)
{
  Surface *surf = new Surface;
  return setSurface(surf, ambient, diffuse, brilliance, specular,
                    roughness, reflection, refraction);
}

Surface * deleteSurface(Surface *surf)
{
  if (surf)
    delete[] surf;
  return (surf = NULL);
}

Surface * setSurface(Surface *surf, float ambient, float diffuse,
                     float brilliance, float specular, float roughness,
                     float reflection, float refraction)
{
  if (! surf) return NULL;

  surf->ambient = ambient;
  surf->diffuse = diffuse;
  surf->brilliance = brilliance;
  surf->specular = specular;
  surf->roughness = roughness;
  surf->reflection = reflection;
  surf->refraction = refraction;
  return surf;
}

Surface * copySurface(Surface *surf1, Surface *surf2)
{
  if (!surf1 || !surf2) return NULL;

  return setSurface(surf1, surf2->ambient, surf2->diffuse, surf2->brilliance,
                    surf2->specular, surf2->roughness, surf2->reflection,
                    surf2->refraction);
}

/*------------------  Object Polygon -----------------------------------*/
Polygon * newPolygon(char pol_type, char points_type, SRGB *color)
{
  Polygon *pol = new Polygon;
  pol->pol_type = pol_type;
  pol->points_type = points_type;
  pol->points.pnts = NULL;
  pol->num_points = 0;
  pol->color = color;
  pol->surface = NULL;
  return pol;
}

Polygon * deletePolygon(Polygon *pol)
{
  if (!pol) return NULL;

  if (pol->points_type == POL_POINT_VALS) free(pol->points.pnts);
  else {
    for ( ; pol->num_points > 0 ; pol->num_points--)
      pol->points.refs[pol->num_points-1] = NULL;
    free(pol->points.refs);
  }
  deletePoint3D(pol->color);
  deleteSurface(pol->surface);
  delete[] pol;
  return (pol = NULL);
}

int addPolygonPoint(Polygon *pol, Point3D *p)
{
  if (!pol || !p) return -1;

  if (pol->points_type == POL_POINT_VALS) {
    pol->num_points++;
    Point3D *pnts = (Point3D *) realloc(pol->points.pnts, sizeof(Point3D)*(pol->num_points));
    if (pnts) {
       copyPoint3D(&(pnts[pol->num_points-1]), p);
       pol->points.pnts = pnts;
    }
    else pol->num_points--;
  }
  else {
    pol->num_points++;
    Point3D **refs = (Point3D **) realloc(pol->points.refs, sizeof(Point3D*)*(pol->num_points));
    if (refs) {
       //error(" adding point to polygon");
       refs[pol->num_points-1]=p;
       pol->points.refs=refs;
    }
    else pol->num_points--;
  }
  return pol->num_points;
}

int delPolygonPoint(Polygon *pol)
{
  if (!pol) return -1;

  if (pol->points_type == POL_POINT_VALS) {
    pol->num_points--;
    pol->points.pnts = (Point3D *) realloc(pol->points.pnts, sizeof(Point3D)*(pol->num_points));
  }
  else {
    pol->num_points--;
    pol->points.refs = (Point3D **) realloc(pol->points.refs, sizeof(Point3D*)*(pol->num_points));
  }
  return pol->num_points;
}

int insertPolygonPoint(Polygon *pol, Point3D *p, int pos)
{
  if (!pol || pos<0 || !p) return -1;

  Point3D tmp, n, *tmpr, *nr;

  if (pos >= pol->num_points) return addPolygonPoint(pol, p);
  if (pol->points_type == POL_POINT_VALS) {
    pol->num_points++;
    Point3D *pnts = (Point3D *) realloc(pol->points.pnts, sizeof(Point3D)*(pol->num_points));
    if (pnts) {
      for (copyPoint3D(&n,p); pos < pol->num_points ; pos++) {
        copyPoint3D(&tmp, &(pnts[pos]));
        copyPoint3D(&(pnts[pos]), &n);
        copyPoint3D(&n, &tmp);
      }
      pol->points.pnts = pnts;
    } else pol->num_points--;
  }
  else {
    pol->num_points++;
    Point3D **refs = (Point3D **) realloc(pol->points.refs, sizeof(Point3D*)*(pol->num_points));
    if (refs) {
      for (nr=p; pos < pol->num_points ; pos++) {
        tmpr = refs[pos];
        refs[pos] = nr;
        nr = tmpr;
      }
      pol->points.refs = refs;
    }
    else pol->num_points--;
  }
  return pol->num_points;
}

int removePolygonPoint(Polygon *pol,int pos)
{
  if (!pol || pos<0) return -1;

  Point3D tmp, n, *tmpr, *nr;
  int i;

  if (pos >= pol->num_points) return delPolygonPoint(pol);
  if (pol->points_type == POL_POINT_VALS) {
    Point3D *pnts = pol->points.pnts;
    for (i = pol->num_points-2, copyPoint3D(&n,&(pnts[i+1])); i >= pos ; i--) {
      copyPoint3D(&tmp, &(pnts[i]));
      copyPoint3D(&(pnts[i]), &n);
      copyPoint3D(&n, &tmp);
    }
    pol->num_points--;
    pol->points.pnts = (Point3D *) realloc(pnts, sizeof(Point3D)*(pol->num_points));
  }
  else {
    Point3D **refs = pol->points.refs;
    for (i = pol->num_points-2, nr=refs[i+1]; i >= pos ; i--) {
      tmpr = refs[i];
      refs[i] = nr;
      nr = tmpr;
    }
    pol->num_points--;
    pol->points.refs = (Point3D **) realloc(refs, sizeof(Point3D*)*(pol->num_points));
  }
  return pol->num_points;
}

bool changePolygonType(Polygon *pol,char pol_type)
{
  if (!pol) return false;

  pol->pol_type = pol_type;
  return true;
}

bool changePoligonPointsType(Polygon *pol,char points_type)
{
  if (!pol) return false;

  if (points_type == pol->points_type || pol->num_points <= 0) {
    pol->points_type = points_type;
    return true;
  }
  if (points_type == POL_POINT_VALS && pol->points_type == POL_POINT_REFS) {
    Point3D *pnts = (Point3D *) malloc(sizeof(Point3D)*(pol->num_points));
    for (int i=0; i < pol->num_points ; i++)
       copyPoint3D(&(pnts[i]), pol->points.refs[i]);
    free(pol->points.refs);
    pol->points.pnts = pnts;
    pol->points_type = points_type;
  }
  else {
    Point3D **refs = (Point3D **) malloc(sizeof(Point3D*)*(pol->num_points));
    for (int i=0; i < pol->num_points ; i++)
       refs[i] = newPoint3D(pol->points.refs[i]->x, pol->points.refs[i]->y, pol->points.refs[i]->z);
    free(pol->points.pnts);
    pol->points.refs = refs;
    pol->points_type = points_type;
  }
  return true;
}

Point3D * maxPolygon(Polygon *pol)
{
  if (!pol || pol->num_points <= 0) return NULL;

  Point3D *p = newPoint3D(0,0,0);
  if (pol->points_type == POL_POINT_VALS) {
    copyPoint3D(p, &(pol->points.pnts[0]));
    for (int i=1; i < pol->num_points ; i++) {
      p->x=(pol->points.pnts[i].x > p->x)?pol->points.pnts[i].x:p->x;
      p->y=(pol->points.pnts[i].y > p->x)?pol->points.pnts[i].y:p->y;
      p->x=(pol->points.pnts[i].z > p->x)?pol->points.pnts[i].z:p->z;
    }
  }
  else {
    copyPoint3D(p, &(pol->points.pnts[0]));
    for (int i=1; i<pol->num_points; i++) {
      p->x=(pol->points.refs[i]->x > p->x)?pol->points.refs[i]->x:p->x;
      p->y=(pol->points.refs[i]->y > p->x)?pol->points.refs[i]->y:p->y;
      p->x=(pol->points.refs[i]->z > p->x)?pol->points.refs[i]->z:p->z;
    }
  }
  return p;
}

Point3D * minPolygon(Polygon *pol)
{
  if (!pol || pol->num_points <= 0) return NULL;

  Point3D *p = newPoint3D(0, 0, 0);
  if (pol->points_type == POL_POINT_VALS) {
    copyPoint3D(p, &(pol->points.pnts[0]));
    for (int i=1; i < pol->num_points; i++) {
      p->x = (pol->points.pnts[i].x < p->x)?pol->points.pnts[i].x:p->x;
      p->y = (pol->points.pnts[i].y < p->x)?pol->points.pnts[i].y:p->y;
      p->x = (pol->points.pnts[i].z < p->x)?pol->points.pnts[i].z:p->z;
    }
  }
  else {
    copyPoint3D(p, &(pol->points.pnts[0]));
    for (int i=1; i < pol->num_points; i++) {
      p->x = (pol->points.refs[i]->x < p->x)?pol->points.refs[i]->x:p->x;
      p->y = (pol->points.refs[i]->y < p->x)?pol->points.refs[i]->y:p->y;
      p->x = (pol->points.refs[i]->z < p->x)?pol->points.refs[i]->z:p->z;
    }
  }
  return p;
}

/*------------------  Object PolyMesh ----------------------------------*/
PolyMeshObj * newPolyMesh(char *name, SRGB *color)
{
  PolyMeshObj *pmesh = (PolyMeshObj *) malloc(sizeof(PolyMeshObj));
  pmesh->name = (char *) malloc(2);
  pmesh->name[0]='\0';
  changePolyMeshObjName(pmesh,name);
  pmesh->color=color;
  pmesh->num_points=0;
  pmesh->points=NULL;
  pmesh->num_polygons=0;
  pmesh->polygons=NULL;
  pmesh->surface=NULL;
  return pmesh;
}

PolyMeshObj * deletePolyMesh(PolyMeshObj *pmesh)
{
  if (!pmesh) return NULL;

  if (pmesh->num_polygons > 0) {
    for ( ; pmesh->num_polygons>0 ; pmesh->num_polygons--)
      deletePolygon(pmesh->polygons[pmesh->num_polygons-1]);
    free(pmesh->polygons);
  }
  if (pmesh->num_points > 0) {
    for ( ; pmesh->num_points>0 ; pmesh->num_points--)
      deletePoint3D(pmesh->points[pmesh->num_points-1]);
    free(pmesh->points);
  }
  deletePoint3D(pmesh->color);
  deleteSurface(pmesh->surface);
  pmesh = NULL;
  return pmesh;
}

PolyMeshObj * changePolyMeshObjName(PolyMeshObj *pmesh, char *name)
{
  if (!pmesh) return NULL;

  if (name) {
    pmesh->name = (char *) realloc(pmesh->name, strlen(name));
    strcpy(pmesh->name, name);
    _xstrlwr(pmesh->name);
  }
  return pmesh;
}

int addPolyMeshObjPolygon(PolyMeshObj *pmesh, Polygon *pol)
{
  if (!pmesh || !pol) return -1;

  pmesh->num_polygons++;
  Polygon **polygons = (Polygon **) realloc(pmesh->polygons, sizeof(Polygon*)*(pmesh->num_polygons));
  if (polygons) {
    polygons[pmesh->num_polygons-1] = pol;
    pmesh->polygons = polygons;
  }
  else
    pmesh->num_polygons--;
  return pmesh->num_polygons;
}

int delPolyMeshObjPolygon(PolyMeshObj *pmesh)
{
  if (!pmesh) return -1;

  if (pmesh->num_polygons == 0) return 0;
  pmesh->num_polygons--;
  Polygon *pol = pmesh->polygons[pmesh->num_polygons];
  deletePolygon(pol);
  pmesh->polygons = (Polygon **) realloc(pmesh->polygons, sizeof(Polygon*)*(pmesh->num_polygons));
  return pmesh->num_polygons;
}

int insertPolyMeshObjPolygon(PolyMeshObj *pmesh, Polygon *pol, int pos)
{
  if (!pmesh || !pol || pos <0) return -1;
  if (pos >= pmesh->num_polygons) return addPolyMeshObjPolygon(pmesh, pol);
  pmesh->num_polygons++;
  Polygon **polygons = (Polygon **)realloc(pmesh->polygons, sizeof(Polygon*)*(pmesh->num_polygons));
  if (polygons) {
    Polygon *np;
    for (np=pol; pos < pmesh->num_polygons ; pos++) {
      Polygon *tmpp;
      tmpp = polygons[pos];
      polygons[pos] = np;
      np = tmpp;
    }
    pmesh->polygons=polygons;
  }
  else pmesh->num_polygons--;
  return pmesh->num_polygons;
}

int removePolyMeshObjPolygon(PolyMeshObj *pmesh, int pos)
{
  if (!pmesh || pos <0) return -1;

  if (pos >= pmesh->num_polygons) return delPolyMeshObjPolygon(pmesh);
  Polygon **polygons = pmesh->polygons;
  Polygon *pol = polygons[pos];
  Polygon *np;
  int i=0;
  for (i=pmesh->num_polygons-2, np=polygons[i+1]; i >= pos ; i--) {
    Polygon *tmpp;
    tmpp = polygons[i];
    polygons[i] = np;
    np = tmpp;
  }
  deletePolygon(pol);
  pmesh->num_polygons--;
  pmesh->polygons = (Polygon **) realloc(polygons, sizeof(Polygon*)*(pmesh->num_polygons));
  return pmesh->num_polygons;
}

int addPolyMeshObjPoint(PolyMeshObj *pmesh, Point3D *p)
{
  if (!pmesh || !p) return -1;

  pmesh->num_points++;
  Point3D **pnts = (Point3D **) realloc(pmesh->points, sizeof(Point3D*)*(pmesh->num_points));
  if (pnts) {
    pnts[pmesh->num_points-1] = p;
    pmesh->points = pnts;
  }
  else pmesh->num_points--; //error("not adding polygon");
  return pmesh->num_points;
}

int delPolyMeshObjPoint(PolyMeshObj *pmesh)
{
  if (!pmesh) return -1;

  if (pmesh->num_points == 0) return 0;
  pmesh->num_points--;
  Point3D *p = pmesh->points[pmesh->num_points];

  /* eliminar punto de los polygonos !!! */
  for (int i=0; i < pmesh->num_polygons ; i++) {
    Polygon *pol = pmesh->polygons[i];
    if (pol->pol_type == POL_POINT_REFS) {
      for (int j=0; j < pol->num_points ; j++) {
         if (pol->points.refs[j] == p) removePolygonPoint(pol, j);
      }
    }
  }
  deletePoint3D(p);
  pmesh->points = (Point3D **) realloc(pmesh->points, sizeof(Point3D*)*(pmesh->num_points));
  return pmesh->num_points;
}

int insertPolyMeshObjPoint(PolyMeshObj *pmesh, Point3D *p, int pos)
{
  if (!pmesh || !p || pos <0) return -1;

  if (pos >= pmesh->num_points) return addPolyMeshObjPoint(pmesh, p);
  pmesh->num_points++;
  Point3D **points = (Point3D **) realloc(pmesh->points, sizeof(Point3D*)*(pmesh->num_points));
  if (points) {
    Point3D *np;
    for (np=p; pos < pmesh->num_points ; pos++) {
      Point3D *tmpp;
      tmpp = points[pos];
      points[pos] = np;
      np = tmpp;
    }
    pmesh->points = points;
  }
  else pmesh->num_points--;
  return pmesh->num_points;
}

int removePolyMeshObjPoint(PolyMeshObj *pmesh, int pos)
{
  if (!pmesh || pos <0) return -1;

  if (pos>=pmesh->num_points) return delPolyMeshObjPoint(pmesh);
  Point3D **points = pmesh->points;
  Point3D *p = points[pos];
  /* eliminar punto de los polygonos !!! */
  for (int i=0; i < pmesh->num_polygons ; i++) {
    Polygon *pol = pmesh->polygons[i];
    if (pol->pol_type == POL_POINT_REFS) {
      for (int j=0; j < pol->num_points ; j++) {
         if (pol->points.refs[j] == p) removePolygonPoint(pol, j);
      }
    }
  }

  /* ahora si se borra el punto */
  Point3D *np;
  int i;
  for (i = pmesh->num_points-2, np = points[i+1]; i >= pos ; i--) {
    Point3D *tmpp;
    tmpp = points[i];
    points[i] = np;
    np = tmpp;
  }
  deletePoint3D(p);
  pmesh->num_points--;
  pmesh->points = (Point3D **) realloc(points, sizeof(Point3D*)*(pmesh->num_points));
  return pmesh->num_polygons;
}

int addPolyMeshObjPointEx(PolyMeshObj *pmesh, Point3D *p)
{
  if (!pmesh || !p) return -1;

  int i;
  for (i=0; i < pmesh->num_points && !eqPoint3D(pmesh->points[i], p); i++)
    ;
  if (i >= pmesh->num_points) return addPolyMeshObjPoint(pmesh,p);
  return i;
}

int addPolyMeshObjPolygonPoint(PolyMeshObj *pmesh, Point3D *p)
{
  if (!pmesh || !p) return -1;

  if (pmesh->num_polygons==0) return -1;
  Polygon *pol = pmesh->polygons[pmesh->num_polygons-1];
  return addPolygonPoint(pol,p);
}

int delPolyMeshObjPolygonPoint(PolyMeshObj *pmesh)
{
  if (!pmesh) return -1;

  if (pmesh->num_polygons == 0) return -1;
  Polygon *pol = pmesh->polygons[pmesh->num_polygons-1];
  return delPolygonPoint(pol);
}

int insertPolyMeshObjPolygonPoint(PolyMeshObj *pmesh, int pol_num, Point3D *p, int point_num)
{
  Polygon *pol;

  if (!pmesh || pol_num<0 || !p || point_num<0) return -1;
  if (pmesh->num_polygons == 0) return -1;

  if (pol_num >= pmesh->num_polygons-1) {
    pol = pmesh->polygons[pmesh->num_polygons-1];
    return insertPolygonPoint(pol, p, point_num);
  }
  pol = pmesh->polygons[pol_num];
  return insertPolygonPoint(pol, p, point_num);
}

int removePolyMeshObjPolygonPoint(PolyMeshObj *pmesh, int pol_num, int point_num)
{
  Polygon *pol;

  if (!pmesh || pol_num<0 || point_num<0) return -1;
  if (pmesh->num_polygons == 0) return -1;

  if (pol_num >= pmesh->num_polygons-1) {
    pol = pmesh->polygons[pmesh->num_polygons-1];
    return removePolygonPoint(pol, point_num);
  }
  pol = pmesh->polygons[pol_num];
  return removePolygonPoint(pol, point_num);
}

Point3D * maxPolyMeshObj(PolyMeshObj *pmesh)
{
  if (!pmesh || pmesh->num_points <= 0) return NULL;

  Point3D *p = newPoint3D(0,0,0);
  copyPoint3D(p, pmesh->points[0]);
  for (int i=1; i < pmesh->num_points ; i++) {
    p->x = (pmesh->points[i]->x > p->x)?pmesh->points[i]->x:p->x;
    p->y = (pmesh->points[i]->y > p->y)?pmesh->points[i]->y:p->y;
    p->z = (pmesh->points[i]->z > p->z)?pmesh->points[i]->z:p->z;
  }
  return p;
}

Point3D * minPolyMeshObj(PolyMeshObj *pmesh)
{
  if (!pmesh || pmesh->num_points<=0) return NULL;

  Point3D *p = newPoint3D(0,0,0);
  copyPoint3D(p, pmesh->points[0]);
  for (int i=1; i < pmesh->num_points ; i++) {
    p->x = (pmesh->points[i]->x < p->x)?pmesh->points[i]->x:p->x;
    p->y = (pmesh->points[i]->y < p->y)?pmesh->points[i]->y:p->y;
    p->z = (pmesh->points[i]->z < p->z)?pmesh->points[i]->z:p->z;
  }
  return p;
}

char *_xstrclean(char *in, char *tok)
{
  if (!in || !tok) return NULL;

  char *tmpout = strdup(in);
  if (!tmpout) return NULL;
  char *tmpopos = strtok(tmpout, tok);
  strcpy(in, tmpopos);
  while ((tmpopos = strtok(NULL, tok)) != NULL) strcat(in, tmpopos);
  free(tmpout);
  return in;
}

char *_xstrlwr(char *in)
{
  for (int i=0; i < (int)strlen(in) ; i++)
    if (in[i]>='A' && in[i]<='Z') in[i]=in[i]-'A'+'a';
  return in;
}

char *_xstrupr(char *in)
{
  for (int i=0; i < (int)strlen(in) ; i++)
    if (in[i]>='a' && in[i]<='z') in[i]=in[i]-'a'+'A';
  return in;
}

/*------------------ Object DXF_token ----------------------------------*/
DXF_token * newTokenDXF(DXF_gr_id gr_id, const char *data, int datalen)
{
  DXF_token *tok = new DXF_token[1];
  tok->gr_id = gr_id;
  tok->data = new char[datalen+1];
  memcpy(tok->data, data, datalen+1);
  tok->datalen = datalen;
  return tok;
}

DXF_token * deleteTokenDXF(DXF_token *tok)
{
  if (!tok) return NULL;

  if (tok->data) delete[] tok->data;
  delete[] tok;
  return (tok = NULL);
}

DXF_token * readTokenDXF(FILE *fp)
{
  DXF_token *tok = NULL;
  char buffer1[256], buffer2[256];
  char *b1, *b2;

  for ( ; (b1=fgets(buffer1,255,fp))!=NULL && !strcmp(b1, "\n") ; ) ;
  b2 = fgets(buffer2, 255, fp);
  if (b1 && b2) {
    _xstrupr(b1);
    _xstrclean(b1, (char*)" \t\n");
    _xstrupr(b2);
    if (strcmp(b2, "\n")) _xstrclean(b2, (char*)" \t\n");
    tok = newTokenDXF((DXF_gr_id) atoi(b1), b2, strlen(b2));
  }
  return tok;
}

DXF_token * writeTokenDXF (FILE *fp, DXF_token *tok)
{
  if (!tok) return NULL;

  char buffer[256];
  sprintf(buffer, "%d\n", tok->gr_id);
  if (fputs(buffer, fp) != EOF) {
    sprintf(buffer, "%s\n", tok->data);
    if (fputs(buffer, fp) != EOF) return tok;
  }
  return NULL;
}

char eqTokenDXF(DXF_token *tok1, DXF_token *tok2)
{
  if (!tok1 && !tok2) return TOK_EQ;
  if ((!tok1 && tok2) || (tok1 && !tok2)) return FALSE;

  //if (tok1->gr_id == tok1->gr_id) {
    if (! strcmp(tok1->data, tok2->data)) return TOK_EQ;
    return TOK_GR_EQ;
  //}
  //else
    //if (! strcmp(tok1->data, tok2->data)) return TOK_DATA_EQ;
  return FALSE;
}

/*------------------ Object DXF_file -----------------------------------*/
DXF_file * newDXF(char *filename)
{
  DXF_file *dxffile = new DXF_file[1];
  dxffile->filename = strdup(filename);
  dxffile->objects = NULL;
  dxffile->fp = NULL;
  return dxffile;
}

DXF_file * openDXF(DXF_file *dxffile)
{
  if (! dxffile) return NULL;

  filein = new File();
  if ((dxffile->fp = filein->open(dxffile->filename, "r+")) == NULL) return NULL;
  return dxffile;
}

DXF_file * closeDXF(DXF_file *dxffile)
{
  if (! dxffile) return NULL;

  if (filein) {
    filein->close();
    delete filein;
  }
  return dxffile;
}

DXF_file * readDXF(DXF_file *dxffile)
{
  if (! dxffile) return NULL;

  DXF_rule *rule = initParserRulesDXF();
  if (!rule || parseDXF(dxffile, rule) == NULL) {
    error("parsing null");
    if (rule) deleteRuleDXF(rule);
    return NULL;
  }
  return dxffile;
}

DXF_file * deleteDXF(DXF_file *dxffile)
{
  if (! dxffile) return NULL;

  free(dxffile->filename);
  if (dxffile->fp) {
    filein->close();
    delete filein;
  }
  dxffile->fp = NULL;
  deleteScene(dxffile->objects);
  delete[] dxffile;
  return (dxffile = NULL);
}

/*------------------ Object DXF_rule -----------------------------------*/
DXF_rule * newRuleDXF(DXF_token *tok, funcRule(parse))
{
  DXF_rule *rule = new DXF_rule;
  rule->tok = (tok) ? newTokenDXF(tok->gr_id,tok->data,tok->datalen)
                    : newTokenDXF(COMMENT, "COMMENT", strlen("COMMENT"));
  rule->parse = parse;
  rule->act_rule = -1;
  rule->num_rules = 0;
  rule->lrules = NULL;
  return rule;
}

DXF_rule * deleteRuleDXF(DXF_rule *rule)
{
  if (!rule) return NULL;

  for (int i=0; i < rule->num_rules; i++)
    deleteRuleDXF(rule->lrules[i]);
  deleteTokenDXF(rule->tok);
  delete[] rule;
  return (rule = NULL);
}

int addRuleDXF(DXF_rule *rule, DXF_rule *rul)
{
  if (!rule || !rul) return -1;

  rule->num_rules++;
  DXF_rule **rules = (DXF_rule **) realloc(rule->lrules, sizeof(DXF_rule*)*(rule->num_rules));
  if (rules) {
    rules[rule->num_rules-1] = rul;
    rule->lrules = rules;
  }
  else rule->num_rules--;
  return rule->num_rules;
}

int delRuleDXF(DXF_rule *rule)
{
  if (!rule) return -1;
  if (rule->num_rules == 0) return 0;

  rule->num_rules--;
  DXF_rule *rul = rule->lrules[rule->num_rules];
  deleteRuleDXF(rul);
  rule->lrules = (DXF_rule **) realloc(rule->lrules, sizeof(DXF_rule*)*(rule->num_rules));
  return rule->num_rules;
}

DXF_rule * initParserRulesDXF()
{
  DXF_rule *rule, *nrule, *nrule1, *nruletmp;

  DXF_token *tok = newTokenDXF(DXF_FILE, "DXF_FILE", strlen("DXF_FILE"));
  if ((rule = newRuleDXF(tok, parseDXFfile)) == NULL) return NULL;

  /*--------- header   funcs -----------*/
  deleteTokenDXF(tok);
  tok = newTokenDXF(NAME0, "HEADER", strlen("HEADER"));
  nrule = newRuleDXF(tok, parseDXFHEADER);
  addRuleDXF(rule, nrule);

  /*--------- entities funcs -----------*/
  deleteTokenDXF(tok);
  tok = newTokenDXF(NAME0, "ENTITIES", strlen("ENTITIES"));
  nrule = newRuleDXF(tok, parseDXFENTITIES);
  deleteTokenDXF(tok);

  /*  --- rules for the 3DFACE  -------*/
  tok = newTokenDXF(START, "3DFACE", strlen("3DFACE"));
  nrule1 = newRuleDXF(tok, parseDXF3DFACE);

  deleteTokenDXF(tok);
  tok = newTokenDXF(X0, "X0", strlen("X0"));
  nruletmp = newRuleDXF(tok, parseDXFXVALS);
  addRuleDXF(nrule1, nruletmp);

  deleteTokenDXF(tok);
  tok = newTokenDXF(Y0, "Y0", strlen("Y0"));
  nruletmp = newRuleDXF(tok, parseDXFYVALS);
  addRuleDXF(nrule1, nruletmp);

  deleteTokenDXF(tok);
  tok = newTokenDXF(Z0, "Z0", strlen("Z0"));
  nruletmp = newRuleDXF(tok, parseDXFZVALS);
  addRuleDXF(nrule1, nruletmp);

  addRuleDXF(nrule, nrule1);
  addRuleDXF(rule, nrule);

  /* --------- blocks funcs -----------*/
  deleteTokenDXF(tok);
  tok = newTokenDXF(NAME0, "BLOCKS", strlen("BLOCKS"));
  nrule = newRuleDXF(tok, parseDXFBLOCKS);

  deleteTokenDXF(tok);
  tok = newTokenDXF(START, "3DFACE", strlen("3DFACE"));
  nrule1 = newRuleDXF(tok, parseDXF3DFACE);

  deleteTokenDXF(tok);
  tok = newTokenDXF(X0, "X0", strlen("X0"));
  nruletmp = newRuleDXF(tok, parseDXFXVALS);
  addRuleDXF(nrule1, nruletmp);

  deleteTokenDXF(tok);
  tok = newTokenDXF(Y0, "Y0", strlen("Y0"));
  nruletmp = newRuleDXF(tok, parseDXFYVALS);
  addRuleDXF(nrule1, nruletmp);

  deleteTokenDXF(tok);
  tok = newTokenDXF(Z0, "Z0", strlen("Z0"));
  nruletmp = newRuleDXF(tok, parseDXFZVALS);
  addRuleDXF(nrule1, nruletmp);

  addRuleDXF(nrule,nrule1);
  addRuleDXF(rule, nrule);

  return rule;
}

DXF_file * parseDXF(DXF_file *dxffile, DXF_rule *rule)
{
  if (!dxffile || !rule) return NULL;

  DXF_token *tok;
  char resp;

  for (resp=TRUE; ((tok = readTokenDXF(dxffile->fp)) != NULL) && resp ; ) {
    resp = (*(rule->parse))(dxffile, rule, tok);
  }
  if (!resp || !dxffile->objects || (dxffile->objects && dxffile->objects->num_objects==0))
    return NULL;
  return dxffile;
}

/*------------------  Object SceneObj ----------------------------------*/
SceneObj * newScene(char *name, SRGB *color)
{
  SceneObj* scene = (SceneObj *) malloc(sizeof(SceneObj));
  scene->name = (char *) malloc(2);
  scene->name[0] = '\0';
  changeSceneObjName(scene, name);
  scene->color = color;
  scene->num_objects = 0;
  scene->objects = NULL;
  return scene;
}

SceneObj * deleteScene(SceneObj *scene)
{
  if (!scene) return NULL;

  if (scene->num_objects > 0) {
    for ( ; scene->num_objects > 0 ; scene->num_objects--)
      deletePolyMesh(scene->objects[scene->num_objects-1]);
    //dax free(scene->objects);
  }
  //dax free(scene->color);
  free(scene);
  return (scene = NULL);
}

SceneObj * changeSceneObjName(SceneObj *scene, char *name)
{
  if (!scene) return NULL;

  if (name) {
    scene->name = (char *) realloc(scene->name, strlen(name));
    strcpy(scene->name, name);
    _xstrlwr(scene->name);
  }
  return scene;
}

int addSceneObjPolyMesh(SceneObj *scene, PolyMeshObj *pmesh)
{
  if (!scene || !pmesh) return -1;

  scene->num_objects++;
  PolyMeshObj **pmeshes = (PolyMeshObj **) realloc(scene->objects, sizeof(PolyMeshObj*)*(scene->num_objects));
  if (pmeshes) {
    pmeshes[scene->num_objects-1] = pmesh;
    scene->objects = pmeshes;
  }
  else scene->num_objects--;
  return scene->num_objects;
}

int delSceneObjPolyMesh(SceneObj *scene)
{
  if (!scene) return -1;
  if (scene->num_objects == 0) return 0;

  scene->num_objects--;
  PolyMeshObj *pmesh = scene->objects[scene->num_objects];
  deletePolyMesh(pmesh);
  scene->objects = (PolyMeshObj **) realloc(scene->objects, sizeof(PolyMeshObj*)*(scene->num_objects));
  return scene->num_objects;
}

int insertSceneObjPolyMesh(SceneObj *scene, PolyMeshObj *pmesh, int pos)
{
  if (!scene || !pmesh || pos <0) return -1;

  if (pos >= scene->num_objects) return addSceneObjPolyMesh(scene, pmesh);
  scene->num_objects++;
  PolyMeshObj **pmeshes = (PolyMeshObj **) realloc(scene->objects, sizeof(PolyMeshObj*)*(scene->num_objects));
  if (pmeshes) {
    PolyMeshObj *np;
    for (np=pmesh ; pos<scene->num_objects ; pos++) {
      PolyMeshObj *tmpp;
      tmpp = pmeshes[pos];
      pmeshes[pos] = np;
      np = tmpp;
     }
     scene->objects = pmeshes;
  }
  else scene->num_objects--;
  return scene->num_objects;
}

int removeSceneObjPolyMesh(SceneObj *scene, int pos)
{
  if (!scene || pos <0) return -1;

  if (pos >= scene->num_objects) return delSceneObjPolyMesh(scene);
  PolyMeshObj **pmeshes = scene->objects;
  PolyMeshObj *pol = pmeshes[pos];
  PolyMeshObj *np;
  int i;
  for (i = scene->num_objects-2, np=pmeshes[i+1] ; i >= pos ; i--) {
    PolyMeshObj *tmpp;
    tmpp = pmeshes[i];
    pmeshes[i] = np;
    np = tmpp;
  }
  deletePolyMesh(pol);
  scene->num_objects--;
  scene->objects = (PolyMeshObj **) realloc(pmeshes, sizeof(PolyMeshObj*)*(scene->num_objects));
  return scene->num_objects;
}

Point3D * maxSceneObj(SceneObj *scene)
{
  if (!scene || scene->num_objects <= 0) return NULL;

  Point3D *p = newPoint3D(0,0,0);
  Point3D *p1 = newPoint3D(0,0,0);
  copyPoint3D(p, maxPolyMeshObj(scene->objects[0]));
  for (int i=1; i < scene->num_objects ; i++) {
    copyPoint3D(p1, maxPolyMeshObj(scene->objects[i]));
    p->x=(p1->x > p->x)?p1->x:p->x;
    p->y=(p1->y > p->y)?p1->y:p->y;
    p->z=(p1->z > p->z)?p1->z:p->z;
  }
  return p;
}

Point3D * minSceneObj(SceneObj *scene)
{
  if (!scene || scene->num_objects <= 0) return NULL;

  Point3D *p = newPoint3D(0,0,0);
  Point3D *p1 = newPoint3D(0,0,0);
  copyPoint3D(p, minPolyMeshObj(scene->objects[0]));
  for(int i=1; i < scene->num_objects ; i++) {
    copyPoint3D(p1, minPolyMeshObj(scene->objects[i]));
    p->x=(p1->x < p->x)?p1->x:p->x;
    p->y=(p1->y < p->y)?p1->y:p->y;
    p->z=(p1->z < p->z)?p1->z:p->z;
  }
  return p;
}

DXF_token *toksect = NULL, *tokendsect = NULL, *tokeof = NULL;

char parseDXFfile(DXF_file *dxffile, DXF_rule *rule, DXF_token *tok)
{
  if (!dxffile || !rule || !tok) return FALSE;

  char resp;

  if (!toksect) toksect = newTokenDXF(START, "SECTION", strlen("SECTION"));
  if (!tokendsect) tokendsect = newTokenDXF(START, "ENDSEC", strlen("ENDSEC"));
  if (!tokeof) tokeof = newTokenDXF(START, "EOF", strlen("EOF"));

  if (eqTokenDXF(tok, tokeof) != TOK_EQ) {
    if (eqTokenDXF(tok, toksect) == TOK_EQ || eqTokenDXF(tok, tokendsect) == TOK_EQ)
      rule->act_rule = -1;
    else if (rule->act_rule == -1) {
      for (resp=FALSE, rule->act_rule=0; rule->act_rule < rule->num_rules && !resp ; rule->act_rule++) {
        rule->lrules[rule->act_rule]->act_rule = -1;
        //error("section: %s", rule->lrules[rule->act_rule]->tok->data);
        resp = (*(rule->lrules[rule->act_rule]->parse))(dxffile,rule->lrules[rule->act_rule],tok);
      }
      if (rule->act_rule == rule->num_rules) rule->act_rule = -1;
    }
    else
      (*(rule->lrules[rule->act_rule]->parse))(dxffile,rule->lrules[rule->act_rule],tok);
  }
  else {
    if (toksect) deleteTokenDXF(toksect);
    toksect = NULL;
    if (tokendsect) deleteTokenDXF(tokendsect);
    tokendsect = NULL;
    if (tokeof) deleteTokenDXF(tokeof);
    tokeof = NULL;
  }
  return TRUE;
}

char parseDXFHEADER(DXF_file *dxffile, DXF_rule *rule, DXF_token *tok)
{
  if (!dxffile || !rule || !tok) return FALSE;

  if (eqTokenDXF(rule->tok, tok) == TOK_EQ) {
    rule->act_rule = -2;
    return TRUE;
  }
  else if (rule->act_rule == -2) return TRUE;
  return FALSE;
}

char parseDXFENTITIES(DXF_file *dxffile, DXF_rule *rule, DXF_token *tok)
{
  if (!dxffile || !rule || !tok) return FALSE;

  char resp;
  if (eqTokenDXF(rule->tok, tok) == TOK_EQ) {
    rule->act_rule = -1;
    if (!dxffile->objects) dxffile->objects = newScene(dxffile->filename, newPoint3D(1,1,1));
    addSceneObjPolyMesh(dxffile->objects, newPolyMesh(tok->data,newPoint3D(1,1,1)));
  }
  else if (rule->act_rule == -1) {
    for (rule->act_rule=0; rule->act_rule<rule->num_rules; rule->act_rule++)
      rule->lrules[rule->act_rule]->act_rule = -1;
    for (rule->act_rule=0; rule->act_rule<rule->num_rules &&
        !(*(rule->lrules[rule->act_rule]->parse))(dxffile,rule->lrules[rule->act_rule],tok) ;
        rule->act_rule++);
    if (rule->act_rule == rule->num_rules) {
      rule->act_rule = -1;
      return FALSE;
    }
  }
  else {
    resp = (*(rule->lrules[rule->act_rule]->parse))(dxffile,rule->lrules[rule->act_rule],tok);
    if (!resp) rule->act_rule = -1;
    return resp;
  }
  return TRUE;
}

char parseDXFBLOCKS(DXF_file *dxffile, DXF_rule *rule, DXF_token *tok)
{
  if (!dxffile || !rule || !tok) return FALSE;

  char resp;
  if (eqTokenDXF(rule->tok, tok) == TOK_EQ) {
    rule->act_rule = -1;
    if (dxffile->objects == NULL) dxffile->objects = newScene(dxffile->filename, newPoint3D(1,1,1));
    addSceneObjPolyMesh(dxffile->objects,newPolyMesh(tok->data,newPoint3D(1,1,1)));
  }
  else if (rule->act_rule == -1) {
    for (rule->act_rule=0; rule->act_rule<rule->num_rules; rule->act_rule++)
      rule->lrules[rule->act_rule]->act_rule=-1;
    for (rule->act_rule=0; rule->act_rule<rule->num_rules &&
        !(*(rule->lrules[rule->act_rule]->parse))(dxffile,rule->lrules[rule->act_rule],tok) ;
        rule->act_rule++)
      ;
    if (rule->act_rule==rule->num_rules) {
      rule->act_rule=-1;
      return FALSE;
    }
  }
  else {
    resp = (*(rule->lrules[rule->act_rule]->parse))(dxffile,rule->lrules[rule->act_rule],tok);
    if (!resp) rule->act_rule = -1;
    return resp;
  }
  return TRUE;
}

Point3D * __PointActTok = NULL;
int    __CountCoordTok;

char parseDXF3DFACE(DXF_file *dxffile, DXF_rule *rule, DXF_token *tok)
{
  if (!dxffile || !rule || !tok) return FALSE;

  char tmp;
  int posp;

  if ((tmp = eqTokenDXF(rule->tok, tok)) == TOK_DATA_EQ || tmp == TOK_EQ) {
    rule->act_rule = 0;
    __CountCoordTok = 0;
    __PointActTok=newPoint3D(0,0,0);
    if (!dxffile->objects) {
      dxffile->objects = newScene(dxffile->filename, newPoint3D(1,1,1));
      addSceneObjPolyMesh(dxffile->objects, newPolyMesh(tok->data, newPoint3D(1,1,1)));
    }
    addPolyMeshObjPolygon(dxffile->objects->objects[dxffile->objects->num_objects-1],
                          newPolygon(POL_CLOSED, POL_POINT_REFS,newPoint3D(1,1,1)));
  }
  else if (rule->act_rule != -1) {
    //error("en 3d face making points data %s", tok->data);
    for (rule->act_rule=0; rule->act_rule<rule->num_rules &&
        !((*(rule->lrules[rule->act_rule]->parse))(dxffile,rule->lrules[rule->act_rule],tok)); rule->act_rule++)
      ;
    if (__PointActTok && rule->act_rule<rule->num_rules && __CountCoordTok>=3) {

      posp = addPolyMeshObjPointEx(dxffile->objects->objects[dxffile->objects->num_objects-1],__PointActTok);
       if (posp < dxffile->objects->objects[dxffile->objects->num_objects-1]->num_points) {
          deletePoint3D(__PointActTok);
          __PointActTok = dxffile->objects->objects[dxffile->objects->num_objects-1]->points[posp];
       }

      addPolyMeshObjPolygonPoint(dxffile->objects->objects[dxffile->objects->num_objects-1], __PointActTok);
      __CountCoordTok=0;
      __PointActTok=NULL;
      if (dxffile->objects->objects[dxffile->objects->num_objects-1]->
              polygons[dxffile->objects->objects[dxffile->objects->num_objects-1]->
              num_polygons-1]->num_points>=4) {
        return FALSE;
        rule->act_rule=-1;
      }
    }
  }
  else return FALSE;
  return TRUE;
}

char parseDXFXVALS(DXF_file *dxffile, DXF_rule *rule, DXF_token *tok)
{
  if (!dxffile || !rule || !tok) return FALSE;

  if (!(tok->gr_id>=X0 && tok->gr_id<=XN)) return FALSE;
  if (__PointActTok == NULL && __CountCoordTok == 0) __PointActTok=newPoint3D(0,0,0);
  if (__CountCoordTok == 0) {
    __CountCoordTok++;
    __PointActTok->x = strtod(tok->data, NULL);
  }
  return TRUE;
}

char parseDXFYVALS(DXF_file *dxffile, DXF_rule *rule, DXF_token *tok)
{
  if (!dxffile || !rule || !tok) return FALSE;

  if (!(tok->gr_id>=Y0 && tok->gr_id<=YN)) return FALSE;
  if (__PointActTok == NULL && __CountCoordTok == 1) __PointActTok=newPoint3D(0,0,0);
  if (__CountCoordTok == 1) {
    __CountCoordTok++;
    __PointActTok->y = strtod(tok->data, NULL);
  }
  return TRUE;
}

char parseDXFZVALS(DXF_file *dxffile, DXF_rule *rule, DXF_token *tok)
{
  if (!dxffile || !rule || !tok) return FALSE;

  if (!(tok->gr_id>=Z0 && tok->gr_id<=ZN)) return FALSE;
  if (__PointActTok == NULL && __CountCoordTok == 2) __PointActTok=newPoint3D(0,0,0);
  if (__CountCoordTok == 2) {
    __CountCoordTok++;
    __PointActTok->z = strtod(tok->data, NULL);
  }
  return TRUE;
}
