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
//***************************************//
// Author (10/06/01) :                   //
// Ben Humphrey, Game Programmer         //
// DigiBen@GameTutorials.com             //
// Co-Web Host of www.GameTutorials.com  //
//                                       //
// Vreng port (21/09/06) :               //
// Philippe Dax                          //
// dax@enst.fr                           //
//***************************************//
#include "vreng.hpp"
#include "obj.hpp"
#include "http.hpp"	// httpOpen
#include "texture.hpp"	// open
#include "cache.hpp"	// open, close


Obj::Obj(const char *_url)
 : loaded(false), currentScale(1.0), desiredScale(1.0), fp(NULL)
{
  OBJModel.numObjects = 0;
  OBJModel.numMaterials = 0;
  bObjectHasUV = false;
  bObjectHasNormal = false;
  bJustReadAFace = false;
  flgcolor = false;
  for (int i=0; i<4 ; i++) {
    mat_diffuse[i] = 1;
    mat_ambient[i] = 1;
    mat_specular[i] = 1;
  }
  flgpart = 0;
  url = new char[strlen(_url) + 1];
  strcpy(url, _url);
  Http::httpOpen(url, reader, this, 0);
}

Obj::Obj(const char *_url, int _flgpart)
 : loaded(false), currentScale(1.0), desiredScale(1.0), fp(NULL)
{
  OBJModel.numObjects = 0;
  OBJModel.numMaterials = 0;
  bObjectHasUV = false;
  bObjectHasNormal = false;
  bJustReadAFace = false;
  flgcolor = false;
  for (int i=0; i<4 ; i++) {
    mat_diffuse[i] = 1;
    mat_ambient[i] = 1;
    mat_specular[i] = 1;
  }
  flgpart = _flgpart;
  url = new char[strlen(_url) + 1];
  strcpy(url, _url);
  if (! flgpart) Http::httpOpen(url, reader, this, 0);
}

/** Destructor: Free the faces, normals, vertices, and texture coordinates */
Obj::~Obj()
{
  for (int i=0; i < OBJModel.numObjects; i++) {
    if (OBJModel.pObject[i].pFaces)    delete[] OBJModel.pObject[i].pFaces;
    if (OBJModel.pObject[i].pNormals)  delete[] OBJModel.pObject[i].pNormals;
    if (OBJModel.pObject[i].pVerts)    delete[] OBJModel.pObject[i].pVerts;
    if (OBJModel.pObject[i].pTexVerts) delete[] OBJModel.pObject[i].pTexVerts;
  }
  if (url) delete[] url;
  if (dlist > 0) glDeleteLists(dlist, 1);
}

void Obj::reader(void *_obj, Http *http)
{
  Obj *obj = (Obj *) _obj;
  if (! obj) return;

  Cache *cache = new Cache();
  FILE *f = cache->open(obj->getUrl(), http);
  obj->import(f);
  if (! obj->flgpart) obj->displaylist();
  cache->close();
  delete cache;
}

bool Obj::import(FILE *f)
{
  fp = f;

  if (importModel(&OBJModel) == false) {
    error("OBJ file not loaded");
    return false;
  }
  if (importTextures() == false) error("textures of OBJ file not loaded");
  return loaded = true; //success
}

const char * Obj::getUrl() const
{
  return (const char *) url;
}

// checks the materials
bool Obj::importTextures()
{
  int id = 0;

  for (int i=0; i < OBJModel.numMaterials; i++) {
    if (strlen(OBJModel.pMaterials[i].texFile) > 0) {
      id = openTexture(OBJModel.pMaterials[i].texFile);
      if (id < 0) {
        error("texture %s not loaded", OBJModel.pMaterials[i].texFile);
        return false;
      }
    }
    OBJModel.pMaterials[i].texureId = id;  // set the texture ID for this material
  }
  return true;
}

int Obj::openTexture(const char *file)
{
  char *end = strrchr(url, '/');
  int i=0;
  char urltex[URL_LEN];

  for (char *p = url; p != end; i++, p++) urltex[i] = *p;
  sprintf(&urltex[i], "/%s", file);
  return Texture::open(urltex);
}

float Obj::getScale()
{
  double max_radius = 0.0;

  for (int i=0; i < OBJModel.numObjects; i++) {
    if (OBJModel.pObject.size() <= 0) break;
    tOBJObject *pobject = &OBJModel.pObject[i];
    for (int j=0; j < pobject->numFaces; j++)
      for (int v = 0; v < 3; v++) {
        int vi = pobject->pFaces[j].vertIndex[v];
        double r= pobject->pVerts[vi].x * pobject->pVerts[vi].x +
                  pobject->pVerts[vi].y * pobject->pVerts[vi].y +
                  pobject->pVerts[vi].z * pobject->pVerts[vi].z ;
        if (r > max_radius) max_radius = r;
      }
  }
  return sqrt(max_radius);
}

void Obj::setScale(float scale)
{
  if (scale != currentScale) {
    if (! loaded) desiredScale = scale;
    else {
      currentScale = desiredScale = scale;
      for (int i=0; i < OBJModel.numObjects; i++) {
        if (OBJModel.pObject.size() <= 0) break;
        tOBJObject *pobject = &OBJModel.pObject[i];
        for (int j=0; j < pobject->numVerts; j++) {
          pobject->pVerts[j].x *= scale;
          pobject->pVerts[j].y *= scale;
          pobject->pVerts[j].z *= scale;
        }
      }
    }
  }
}

void Obj::setColor(int type, float *color)
{
  flgcolor = true;
  switch (type) {
    case GL_DIFFUSE: for (int i=0; i<4 ; i++) mat_diffuse[i] = color[i]; break;
    case GL_AMBIENT: for (int i=0; i<4 ; i++) mat_ambient[i] = color[i]; break;
    case GL_SPECULAR: for (int i=0; i<4 ; i++) mat_specular[i] = color[i]; break;
    case GL_EMISSION: break;
  }
}

void Obj::render(float *color)
{
  if (!loaded) return;

  glPushMatrix();
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glScalef(currentScale, currentScale, currentScale);
  glColor3fv(color);

  glCallList(dlist);

  glDisable(GL_COLOR_MATERIAL);
  glPopMatrix();
}

void Obj::render(const Pos &pos, float *color)
{
  if (!loaded) return;

  glPushMatrix();
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glTranslatef(pos.x, pos.y, pos.z);
  glScalef(currentScale, currentScale, currentScale);
  glRotatef(RAD2DEG(pos.az), 0, 0, 1);
  glRotatef(RAD2DEG(pos.ax), 1, 0, 0);
  glRotatef(RAD2DEG(pos.ay), 0, 1, 0);
  glColor3fv(color);

  glCallList(dlist);

  glDisable(GL_COLOR_MATERIAL);
  glPopMatrix();
}

// Draws in the display list
GLint Obj::displaylist()
{
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  draw();
  glEndList();
  return dlist;
}

void Obj::draw()
{
  if (!loaded) return;

  if (currentScale != desiredScale) setScale(desiredScale);
  if (flgcolor) {
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  }

  for (int i=0; i < OBJModel.numObjects; i++) {
    if (OBJModel.pObject.size() <= 0) break;

    tOBJObject *pobject = &OBJModel.pObject[i];	// get the current object

    glBegin(GL_QUADS);
    for (int j=0; j < pobject->numFaces; j++) {
      for (int c=0; c < 4; c++) {  // go through each corner of the triangle and draw it.
        int vi = pobject->pFaces[j].vertIndex[c]; // get the vertex index for each point of the face
        if (vi < 0 || vi > pobject->numVerts) continue; //DAX BUGFIX: to avoid segfault
        glNormal3f(pobject->pNormals[vi].x, pobject->pNormals[vi].y, pobject->pNormals[vi].z);
        if (pobject->pTexVerts) { // if the object has a texture associated with it
          int ti = pobject->pFaces[j].coordIndex[c]; // get the texture coordinate index
          glTexCoord2f(pobject->pTexVerts[ti].x, pobject->pTexVerts[ti].y);
        }
        glVertex3f(pobject->pVerts[vi].x, pobject->pVerts[vi].y, pobject->pVerts[vi].z);
      }
    }
    glEnd();
  }
}

/** loads a .obj file into a specified model by a .obj file name */
bool Obj::importModel(tOBJModel *pmodel)
{
  if (!pmodel || !fp) return false;

  readFile(pmodel);		// let's read in the info!
  computeNormals(pmodel);	// compute the vertex normals for lighting

  return true;
}

/** main loop for reading in the .obj file */
void Obj::readFile(tOBJModel *pmodel)
{
  char line[256];

  while (! feof(fp)) {
    switch (fgetc(fp)) { // get the beginning character of the current line
    case 'v':		// check if we just read in a 'v' (vertice/normal/textureCoord)
      if (bJustReadAFace) fillInObjectInfo(pmodel); // save the last object's info
      readVertexInfo(); // see if it's a vertex "v", normal "vn", or UV coordinate "vt"
      break;
    case 'f':		// check if we just read in a face header ('f')
      readFaceInfo();
      bJustReadAFace = true;
      break;
    default:
      fgets(line, sizeof(line), fp);
      break;
    }
  }
  fillInObjectInfo(pmodel); // we are done reading in the file
}

/** reads in the vertex information ("v" vertex : "vt" UVCoord) */
void Obj::readVertexInfo()
{
  Vec3 vVertex   = {0};
  Vec2 vTexCoord = {0};
  char line[256];

  char ch = fgetc(fp); // next character

  if (ch == ' ') {	// if we get a space it must have been a vertex ("v")
    // here we read in a vertice.  the format is "v x y z"
    fscanf(fp, "%f %f %f", &vVertex.x, &vVertex.y, &vVertex.z);
    fgets(line, sizeof(line), fp);
    pVertices.push_back(vVertex);	// add a new vertice to our list
  }
  else if (ch == 't') {	// a texture coordinate ("vt")
    // the format is "vt u v"
    fscanf(fp, "%f %f", &vTexCoord.x, &vTexCoord.y);
    fgets(line, sizeof(line), fp);
    pTextureCoords.push_back(vTexCoord); // add a new texcoord to our list

    // set the flag that tells us this object has texture coordinates.
    // we know that the face information will list the vertice AND UV index.
    // for example, ("f 1 3 2" verses "f 1/1 2/2 3/3")
    bObjectHasUV = true;
  }
  else {	// otherwise it's probably a normal so we don't care ("vn")
    if (ch == 'n') bObjectHasNormal = true;
    // we calculate our own normals at the end so we read past them.
    fgets(line, sizeof(line), fp);
  }
}

/** reads in the face information ("f") */
void Obj::readFaceInfo()
{
  tOBJFace Face;
  char line[255];

  if (bObjectHasUV) { // check if this object has texture coordinates
    // here we read in the object's vertex and texture coordinate indices.
    // the format: "f vertexIndex1/coordIndex1 vertexIndex2/coordIndex2 vertexIndex3/coordIndex3"
    int vi[4]={0,0,0,0}, ti[4]={0,0,0,0}, ni[4]={0,0,0,0};

    for (int c=0; c < 4; c++) {
      int r;
      if (bObjectHasNormal) {
        if (fscanf(fp, "%d/%d/%d", &vi[c], &ti[c], &ni[c]) == 3) {
          Face.vertIndex[c] = vi[c];
          Face.coordIndex[c] = ti[c];
        }
        else if (fscanf(fp, "%d//%d", &vi[c], &ni[c]) == 2) {
          Face.vertIndex[c] = vi[c];
        }
        else {
          Face.vertIndex[3] = vi[2];
          Face.coordIndex[3] = ti[2];
        }
      }
      else {
        if ((r = fscanf(fp, "%d/%d", &vi[c], &ti[c])) == 2) {
          Face.vertIndex[c] = vi[c];
          Face.coordIndex[c] = ti[c];
        }
        else {
          Face.vertIndex[3] = vi[2];
          Face.coordIndex[3] = ti[2];
        }
      }
    }
  }
  else {	// the object does NOT have texture coordinates
    // here we read in just the object's vertex indices.
    // the format: "f vertexIndex1 vertexIndex2 vertexIndex3"
    fscanf(fp, "%d %d %d", &Face.vertIndex[0], &Face.vertIndex[1], &Face.vertIndex[2]);
  }
  fgets(line, sizeof(line), fp);	// purge current line
  pFaces.push_back(Face);		// add the  face to our face list
}

/** called after an object is read in to fill in the model structure */
void Obj::fillInObjectInfo(tOBJModel *pmodel)
{
  tOBJObject object = {0};
  int textureOff = 0, vertexOff = 0;
  int i = 0;

  // we just finished reading in an object and need to increase the object count.
  pmodel->numObjects++;

  // add a new object to the list of objects
  pmodel->pObject.push_back(object);

  // get a pointer to the current object
  tOBJObject *pobject = &(pmodel->pObject[pmodel->numObjects - 1]);

  // now that we have our list's full of information, we can get the size
  pobject->numFaces   = pFaces.size();
  pobject->numVerts   = pVertices.size();
  pobject->numTexVertex = pTextureCoords.size();

  //echo("numObjects=%d numFaces=%d numVerts=%d numTexVertex=%d", pmodel->numObjects, pobject->numFaces, pobject->numVerts, pobject->numTexVertex);

  // if we read in any faces for this object (required)
  if (pobject->numFaces)
    pobject->pFaces = new tOBJFace [pobject->numFaces];

  // if we read in any vertices for this object (required)
  if (pobject->numVerts)
    pobject->pVerts = new Vec3 [pobject->numVerts];

  // if we read in any texture coordinates for this object (optional)
  if (pobject->numTexVertex) {
    pobject->pTexVerts = new Vec2 [pobject->numTexVertex];
    pobject->bHasTexture = true;
  }

  for (i=0; i < pobject->numFaces; i++) { // go through all of the faces
    pobject->pFaces[i] = pFaces[i]; // copy the current face from the temporary list

    // if a new object is found in the file, the face and texture indices start
    // from the last index that was used in the last object.  That means that if
    // the last one was 8, it would then go to 9 for the next object. We need to
    // bring that back down to 1, so we just create an offset that we subtract
    // from the vertex and UV indices.

    if (i == 0 && ! flgpart) { // check if this is the first face
      // if the first index is not 1, then we must be past the first object
      if (pobject->pFaces[0].vertIndex[0] != 1) {
        // to create the offset we take the current starting point minus 1.
      	// lets say the last object ended at 8.  we would then have 9 here.
        // we want to then subtract 8 from the 9 to get back to 1.
        vertexOff = pobject->pFaces[0].vertIndex[0] - 1;

      	// the same goes for texture coordinates, if we have them do the same
      	if (pobject->numTexVertex > 0)
          textureOff = pobject->pFaces[0].coordIndex[0] - 1; // take the current index
      }
    }
    // because the face indices start at 1, we need to minus 1 from them due
    // to arrays being zero based, this is VERY important!
    for (int j=0; j<4; j++) {
      // for each index, minus 1 to conform with zero based arrays.
      // we also need to add the vertex and texture offsets to subtract
      // the total amount necessary for this to work.  The first object
      // will have a offset of 0 for both since it starts at 1.
      pobject->pFaces[i].vertIndex[j]  -= 1 + vertexOff;
      pobject->pFaces[i].coordIndex[j] -= 1 + textureOff;
    }
  }
  for (i=0; i < pobject->numVerts; i++) { // go through all the vertices in the object
    pobject->pVerts[i] = pVertices[i];  // copy the current vertice from the temporary list
  }
  for (i=0; i < pobject->numTexVertex; i++) { // go through all of the texture coordinates
    pobject->pTexVerts[i] = pTextureCoords[i]; // copy the current UV coordinate
  }

  // .OBJ files don't have materials, we set the material ID to -1.
  pobject->materialID = -1;

  pVertices.clear();
  pFaces.clear();
  pTextureCoords.clear();

  bObjectHasUV   = false;
  bObjectHasNormal = false;
  bJustReadAFace = false;
}

/** computes the normals and vertex normals of the objects */
void Obj::computeNormals(tOBJModel *pmodel)
{
  Vec3 v1, v2, vNormal, vPoly[3];

  if (pmodel->numObjects <= 0) return; // if there are no objects, we can skip

  for (int index = 0; index < pmodel->numObjects; index++) { // for each of the objects
    tOBJObject *pobject = &(pmodel->pObject[index]); // get the current object

    Vec3 *pNormals     = new Vec3 [pobject->numFaces];
    Vec3 *pTempNormals = new Vec3 [pobject->numFaces];
    pobject->pNormals  = new Vec3 [pobject->numVerts];

    for (int i=0; i < pobject->numFaces; i++) { // go though all of the faces
      // we extract the 3 points of this face
      int vx = pobject->pFaces[i].vertIndex[0];
      int vy = pobject->pFaces[i].vertIndex[1];
      int vz = pobject->pFaces[i].vertIndex[2];
      int vc = pobject->numVerts;
      if (vx > vc || vy > vc || vz > vc) {
        echo("Obj: vx=%d vy=%d vz=%d vc=%d", vx, vy, vz, vc);
        continue;	//dax BUG: segfault
      }
      vPoly[0] = pobject->pVerts[pobject->pFaces[i].vertIndex[0]];
      vPoly[1] = pobject->pVerts[pobject->pFaces[i].vertIndex[1]];
      vPoly[2] = pobject->pVerts[pobject->pFaces[i].vertIndex[2]];

      // let's calculate the face normals (get 2 vectors and find the cross product of those 2)
      v1 = Vec3::subVect(vPoly[0], vPoly[2]); // get the vector of the polygon (we just need 2 sides)
      v2 = Vec3::subVect(vPoly[2], vPoly[1]); // get a second vector of the polygon
      vNormal  = Vec3::crossVect(v1, v2);  // return the cross product of the 2 vectors
      pTempNormals[i] = vNormal;     // save the un-normalized normal for the vertex normals
      vNormal = Vec3::normVect(vNormal);  // normalize the cross product to give us the polygons normal
      pNormals[i] = vNormal;         // assign the normal to the list of normals
    }

    Vec3 vSum = {0., 0., 0.};
    Vec3 vZero = vSum;
    int shared=0;

    // get the vertex normals
    for (int i=0; i < pobject->numVerts; i++) {  // go through all of the vertices
      for (int j=0; j < pobject->numFaces; j++) {// go through all of the triangles
        // check if the vertex is shared by another face
        if (pobject->pFaces[j].vertIndex[0] == i ||
            pobject->pFaces[j].vertIndex[1] == i ||
            pobject->pFaces[j].vertIndex[2] == i) {
          vSum = Vec3::addVect(vSum, pTempNormals[j]); // add the unnormalized normal of the shared face
          shared++;                // increase the number of shared triangles
        }
      }
      // get the normal by dividing the sum by the shared. we negate so normals pointing out.
      pobject->pNormals[i] = Vec3::divVect(vSum, float(-shared));

      // normalize the normal for the final vertex normal
      pobject->pNormals[i] = Vec3::normVect(pobject->pNormals[i]);
      vSum = vZero;                  // reset the sum
      shared = 0;                    // reset the shared
    }
    if (pTempNormals) delete[] pTempNormals;
    if (pNormals) delete[] pNormals;
  }
}
