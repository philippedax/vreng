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
// Author :                              //
// Ben Humphrey, Game Programmer         //
// DigiBen@GameTutorials.com             //
// Co-Web Host of www.GameTutorials.com  //
//                                       //
// Vreng port :                          //
// Romain Piegay                         //
// piegay@enst.fr                        //
// piegaro.free.fr/piegaropro            //
//***************************************//
#include "vreng.hpp"
#include "3ds.hpp"
#include "http.hpp"	// httpOpen
#include "texture.hpp"	// open
#include "cache.hpp"	// open, close
#include "endian.hpp"	// bigEndian


_3ds::_3ds(const char *_url) :
 loaded(false),
 currentScale(1),
 desiredScale(1)
{
  _3dsModel.numMaterials = 0;
  _3dsModel.numObjects = 0;
  fp = NULL;

  url = new char[strlen(_url) + 1];
  strcpy(url, _url);
  Http::httpOpen(url, reader, this, 0);
  displaylist();
}

_3ds::~_3ds()
{
  for (int i=0; i < _3dsModel.numObjects; i++) {
    delete[] _3dsModel.pObject[i].pFaces;
    delete[] _3dsModel.pObject[i].pNormals;
    delete[] _3dsModel.pObject[i].pVerts;
    delete[] _3dsModel.pObject[i].pTexVerts;
  }
  if (url) delete[] url;
  if (dlist > 0) glDeleteLists(dlist, 1);
}

bool _3ds::import(FILE *f)
{
  fp = f;

  if (importModel(&_3dsModel) == false) {
    error("3ds file not loaded");
    return false;
  }
  if (importTextures() == false) {
    error("textures of 3ds file not loaded");
    return false;
  }
  return loaded = true; //success
}

/* called by the client to open the .3ds file and read it */
bool _3ds::importModel(t3dsModel *pModel)
{
  tChunk chunk = {0};

  if (!pModel || !fp) return false;

  readChunk(&chunk);
  if (chunk.ID != _3DS_PRIMARY) {
    error("unable to load 3DS_PRIMARY chuck from file");
    return false;
  }

  // Now we actually start reading in the data.  nextChunk() is recursive
  nextChunk(pModel, &chunk);

  // After we have read the whole 3DS file, we want to calculate our own vertex normals.
  computeNormals(pModel);

  return true;
}

bool _3ds::importTextures()
{
  int id = 0;

  for (int i=0; i < _3dsModel.numMaterials; i++) {	// Go through all the materials
    // Check to see if there is a file name to load in this material
    if (strlen(_3dsModel.pMaterials[i].strFile) > 0) {
      // Use the name of the texture file to load the bitmap, with a texture ID (i).
      if ((id = openTexture(_3dsModel.pMaterials[i].strFile)) < 0) {
        error("texture %s not loaded", _3dsModel.pMaterials[i].strFile);
        return false;
      }
    }
    _3dsModel.pMaterials[i].texureId = id;	// Set the texture ID for this material
  }
  return true;
}

int _3ds::openTexture(const char *img)
{
  char *end = strrchr(url, '/');
  int i = 0;
  char url_tex[255];

  for (char *p = url; p != end; i++, p++) url_tex[i] = *p;
  sprintf(&url_tex[i], "/%s", img);
  //echo("openTexture: %s", url_tex);

  return Texture::open(url_tex);
}

float _3ds::getScale()
{
  float max_radius = 0;

  if (loaded) {
    for (int i=0; i < _3dsModel.numObjects; i++) {
      if (_3dsModel.pObject.size() <= 0) break;
      tObject *pObject = &_3dsModel.pObject[i];
      for (int j=0; j < pObject->numFaces; j++) {
        for (int whichVertex=0; whichVertex < 3; whichVertex++) {
          int vi = pObject->pFaces[j].vertIndex[whichVertex];
          float r= pObject->pVerts[vi].x * pObject->pVerts[vi].x +
                   pObject->pVerts[vi].y * pObject->pVerts[vi].y +
                   pObject->pVerts[vi].z * pObject->pVerts[vi].z ;
          if (r > max_radius) max_radius = r;
        }
      }
    }
  }
  return sqrt(max_radius);
}

void _3ds::setScale(float scale)
{
  if (scale != currentScale) {
    if (! loaded) desiredScale = scale;
    else {
      currentScale = desiredScale = scale;
      for (int i=0; i < _3dsModel.numObjects; i++) {
        if (_3dsModel.pObject.size() <= 0) break;
        tObject *pObject = &_3dsModel.pObject[i];
        for (int j=0; j < pObject->numVerts; j++) {
          pObject->pVerts[j].x *= scale;
          pObject->pVerts[j].y *= scale;
          pObject->pVerts[j].z *= scale;
        }
      }
    }
  }
}

const char * _3ds::getUrl() const
{
  return (const char *) url;
}

void _3ds::reader(void *__3ds, Http *http)
{
  _3ds *_3d = (_3ds *) __3ds;
  if (! _3d) return;

  Cache *cache = new Cache();
  FILE *f = cache->open(_3d->getUrl(), http);
  _3d->import(f);
  cache->close();
  delete cache;
}

void _3ds::render(float *color)
{
  if (! loaded) return;

  glPushMatrix();
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glScalef(currentScale, currentScale, currentScale);
  glColor3fv(color);
  glCallList(dlist);
  glDisable(GL_COLOR_MATERIAL);
  glPopMatrix();
}

void _3ds::render(const Pos &pos, float *color)
{
  if (!loaded) return;

  glPushMatrix();
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glTranslatef(pos.x, pos.y, pos.z);
  glScalef(currentScale, currentScale, currentScale);
  glRotatef(RAD2DEG(pos.az), 0, 0, 1);
  glRotatef(RAD2DEG(pos.ax), 1, 0, 0);
  glColor3fv(color);
  //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

  glCallList(dlist);

  glDisable(GL_COLOR_MATERIAL);
  glPopMatrix();
}

// Put drawing in the display list
GLint _3ds::displaylist()
{
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  draw();
  glEndList();
  return dlist;
}

void _3ds::draw()
{
  if (! loaded) return;

  if (currentScale != desiredScale) setScale(desiredScale);

  for (int i=0; i < _3dsModel.numObjects; i++) {
    if (_3dsModel.pObject.size() <= 0) break;
    tObject *pObject = &_3dsModel.pObject[i]; // Get the current object

    if (pObject->bHasTexture) {
      glColor3ub(255, 255, 255);  // Reset the color to normal again

      // Bind the texture map to the object by it's materialID
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, _3dsModel.pMaterials[pObject->materialID].texureId);
    }
    else {
      glDisable(GL_TEXTURE_2D);
      glEnable(GL_COLOR);
      glEnable(GL_COLOR_MATERIAL);
      glColor3ub(255, 200, 200);	// Reset the color to normal again
    }

    glBegin(GL_TRIANGLES);		// Begin drawing
    for (int j=0; j < pObject->numFaces; j++) {  // all faces
      for (int whichVertex=0; whichVertex < 3; whichVertex++) {
        int vi = pObject->pFaces[j].vertIndex[whichVertex];
        if (vi < 0) continue;	//DAX BUG: segfault
        glNormal3f(pObject->pNormals[vi].x, pObject->pNormals[vi].y, pObject->pNormals[vi].z);
        if (pObject->bHasTexture) {
          if (pObject->pTexVerts) {
            glTexCoord2f(pObject->pTexVerts[vi].x, pObject->pTexVerts[vi].y);
          }
        }
        else {
          // Make sure there is a valid material/color assigned to this object
          if (_3dsModel.pMaterials.size() && pObject->materialID >= 0) {

            // Get and set the color that the object is, since it must not have a texture
            uint8_t *pColor = _3dsModel.pMaterials[pObject->materialID].color;
            glColor3ub(pColor[0], pColor[1], pColor[2]);
          }
        }
        glVertex3f(pObject->pVerts[vi].x, pObject->pVerts[vi].y, pObject->pVerts[vi].z);
      }
    }
    glEnd();				// End the drawing
    glColor3ub(255, 255, 255);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);
  }
}

/** reads the main sections of the .3DS file, then dives deeper with recursion */
void _3ds::nextChunk(t3dsModel *pModel, tChunk *pPrev)
{
  tObject newObject = {0};	// This is used to add to our object list
  t3dsMaterialInfo newTexture;	// This is used to add to our material list
  uint32_t version = 0;		// This will hold the file version
  int buffer[50000] = {0};	// This is used to read past unwanted data
  tChunk chunk = {0};		// The current chunk to load
  tChunk tmp = {0};		// A tmp chunk for holding data

  // Continue to read the sub chunks until we have reached the length.
  // After we read ANYTHING we add the bytes read to the chunk and then check
  while (pPrev->read < pPrev->len) {
    readChunk(&chunk);

    switch (chunk.ID) {
    case _3DS_VERSION:		// This holds the version of the file
      // This chunk has an uint16_t that holds the file version.
      chunk.read += fread(&version, 1, chunk.len - chunk.read,fp);
      if (Endian::bigEndian()) { char t; SWAPL(&version, t); }

      if (version > 0x03)
        error("This 3DS file is over version 3 so it may load incorrectly");
      break;
    case _3DS_OBJECTINFO:	// This holds the version of the mesh
      // This chunk holds the version of the mesh.  It is also the head of the _3DS_MATERIAL
      // and _3DS_OBJECT chunks. From here on we start reading in the material and object info.
      readChunk(&tmp);

      // Get the version of the mesh
      tmp.read += fread(&version, 1, tmp.len - tmp.read, fp);
      if (Endian::bigEndian()) { char t; SWAPL(&version, t); }

      chunk.read += tmp.read;

      // Go to the next chunk, which is the object has a texture, it should be _3DS_MATERIAL, then _3DS_OBJECT
      nextChunk(pModel, &chunk);
      break;
    case _3DS_MATERIAL:		// This holds the material information
      // This chunk is the header for the material info chunks
      pModel->numMaterials++;

      // Add a empty texture structure to our texture list.
      pModel->pMaterials.push_back(newTexture);

      // Proceed to the material loading function
      nextMaterial(pModel, &chunk);
      break;
    case _3DS_OBJECT:		// This holds the name of the object being read
      // This chunk is the header for the object info chunks.  It also
      // holds the name of the object.
      pModel->numObjects++;	// Increase the object count - FIXME segfault

      // Add a new tObject node to our list of objects (like a link list)
      pModel->pObject.push_back(newObject);

      // Initialize the object and all it's data members
      memset(&(pModel->pObject[pModel->numObjects - 1]), 0, sizeof(tObject));

      // Get the name of the object and store it, then add the read bytes
      chunk.read += getString(pModel->pObject[pModel->numObjects - 1].strName);

      // Now proceed to read in the rest of the object information
      nextObject(pModel, &(pModel->pObject[pModel->numObjects - 1]), &chunk);
      break;
    case _3DS_KEYFRAME:
      //nextKeyFrame(pModel, chunk);
      // Read past this chunk and add the bytes read
      chunk.read += fread(buffer, 1, chunk.len - chunk.read, fp);
      break;
    default:
      // If we didn't care about a chunk, then we get here.  We still need
      // to read past the unknown or ignored chunk
      chunk.read += fread(buffer, 1, chunk.len - chunk.read, fp);
      break;
    }
    pPrev->read += chunk.read;
  }
  // Free the current chunk
  //delete chunk;
  //chunk = pPrev;
}

/** handles all the information about the objects in the file */
void _3ds::nextObject(t3dsModel *pModel, tObject *pObject, tChunk *pPrev)
{
  int buffer[50000] = {0};	// This is used to read past unwanted data

  // Allocate a new chunk to work with
  tChunk chunk = {0};

  // Continue to read these chunks until we read the end of this sub chunk
  while (pPrev->read < pPrev->len) {
    readChunk(&chunk);

    switch (chunk.ID) {	// Check which chunk we just read

    case _3DS_OBJECT_MESH:	// This lets us know that we are reading a new object
      // We found a new object, so let's read in it's info using recursion
      nextObject(pModel, pObject, &chunk);
      break;
    case _3DS_OBJECT_VERTICES:	// This is the objects vertices
      readVertices(pObject, &chunk);
      break;
    case _3DS_OBJECT_FACES:	// This is the objects face information
      readVertex(pObject, &chunk);
      break;
    case _3DS_OBJECT_MATERIAL:	// This holds the material name that the object has
      // This chunk holds the name of the material that the object has assigned to it.
      // We now will read the name of the material assigned to this object
      readMaterial(pModel, pObject, &chunk);
      break;
    case _3DS_OBJECT_UV:	// This holds the UV texture coords for the object
      // This chunk holds all of the UV coords for our object.  Let's read them in.
      readUVCoords(pObject, &chunk);
      break;
    default:
      chunk.read += fread(buffer, 1, chunk.len - chunk.read, fp);
      break;
    }

    pPrev->read += chunk.read;
  }
  // Free the current chunk
  //delete chunk;
  //chunk = pPrev;
}

/** handles all the information about the material (Texture) */
void _3ds::nextMaterial(t3dsModel *pModel, tChunk *pPrev)
{
  int buffer[50000] = {0};	// This is used to read past unwanted data
  tChunk chunk = {0};

  while (pPrev->read < pPrev->len) {
    readChunk(&chunk);

    switch (chunk.ID) {
    case _3DS_MATNAME:		// This chunk holds the name of the material
      // Here we read in the material name
      chunk.read += fread(pModel->pMaterials[pModel->numMaterials - 1].strName, 1, chunk.len - chunk.read, fp);
      break;
    case _3DS_MATDIFFUSE:	// This holds the R G B color of our object
      readColor(&(pModel->pMaterials[pModel->numMaterials - 1]), &chunk);
      break;
    case _3DS_MATMAP:		// This is the header for the texture info
      // Proceed to read in the material information
      nextMaterial(pModel, &chunk);
      break;
    case _3DS_MATMAPFILE:	// This stores the file name of the material
      // Here we read in the material's file name
      chunk.read += fread(pModel->pMaterials[pModel->numMaterials - 1].strFile, 1, chunk.len - chunk.read, fp);
      break;
    default:
      chunk.read += fread(buffer, 1, chunk.len - chunk.read, fp);
      break;
    }
    pPrev->read += chunk.read;
  }
  // Free the current chunk
  //delete chunk;
  //chunk = pPrev;
}

/** reads in a chunk ID and it's length in bytes */
void _3ds::readChunk(tChunk *pchunk)
{
  // The chunk ID is like _3DS_OBJECT or _3DS_MATERIAL.
  pchunk->read = fread(&pchunk->ID, 1, 2, fp);
  if (Endian::bigEndian()) { char t; SWAPS(&pchunk->ID, t); }

  // Then, we read the length of the chunk which is 4 bytes.
  pchunk->read += fread(&pchunk->len, 1, 4, fp);
  if (Endian::bigEndian()) { char t; SWAPL(&pchunk->len, t); }
}

/** reads in a string of characters */
int _3ds::getString(char *pbuffer)
{
  int index = 0;

  fread(pbuffer, 1, 1, fp);
  while (*(pbuffer + index++) != 0) {
    fread(pbuffer + index, 1, 1, fp);
  }

  return strlen(pbuffer) + 1;
}

/** reads in the RGB color data */
void _3ds::readColor(t3dsMaterialInfo *pMaterial, tChunk *pchunk)
{
  tChunk tmp = {0};

  readChunk(&tmp); // Read the color chunk info

  // Read in the RGB color (3 bytes - 0 through 255)
  tmp.read += fread(pMaterial->color, 1, tmp.len - tmp.read, fp);

  pchunk->read += tmp.read;
}

/** reads in the indices for the vertex array */
void _3ds::readVertex(tObject *pObject, tChunk *pPrev)
{
  uint16_t index = 0;	// This is used to read in the current face index

  // Read in the number of faces that are in this object (int)
  pPrev->read += fread(&pObject->numFaces, 1, 2, fp);
  if (Endian::bigEndian()) { char t; SWAPL(&pObject->numFaces, t); }

  // Alloc enough memory for the faces and initialize the structure
  pObject->pFaces = new t3dsFace [pObject->numFaces];
  memset(pObject->pFaces, 0, sizeof(t3dsFace) * pObject->numFaces);

  for (int i=0; i < pObject->numFaces; i++) {  // all faces
    // Next, we read in the A then B then C index for the face, but ignore the 4th value.
    // The fourth value is a visibility flag for 3D Studio Max, we don't care about this.
    for (int j=0; j < 4; j++) {
      // Read the first vertice index for the current face
      pPrev->read += fread(&index, 1, sizeof(index), fp);
      if (Endian::bigEndian()) { char t; SWAPS(&index, t); }
      if (j < 3) // Store the index in our face structure.
        pObject->pFaces[i].vertIndex[j] = index;
    }
  }
}

/** reads in the UV coords for the object */
void _3ds::readUVCoords(tObject *pObject, tChunk *pPrev)
{
  // Read in the number of UV coords there are (int)
  pPrev->read += fread(&pObject->numTexVertex, 1, 2, fp);
  if (Endian::bigEndian()) { char t; SWAPL(&pObject->numTexVertex, t); }

  // Allocate memory to hold the UV coords
  pObject->pTexVerts = new Vec2 [pObject->numTexVertex];

  // Read in the texture coods (an array 2 float)
  pPrev->read += fread(pObject->pTexVerts, 1, pPrev->len - pPrev->read, fp);
  for (int i=0; i < pObject->numTexVertex; i++) {
    if (Endian::bigEndian()) { char t; SWAPL(&pObject->pTexVerts[i].x, t); }
    if (Endian::bigEndian()) { char t; SWAPL(&pObject->pTexVerts[i].y, t); }
  }
}

/** reads in the vertices for the object */
void _3ds::readVertices(tObject *pObject, tChunk *pPrev)
{
  // Read in the number of vertices (int)
  pPrev->read += fread(&(pObject->numVerts), 1, 2, fp);
  if (Endian::bigEndian()) { char t; SWAPL(&pObject->numVerts, t); }

  //echo("readVertices: numVerts=%d", pObject->numVerts);

  // Allocate the memory for the verts and initialize the structure
  pObject->pVerts = new Vec3 [pObject->numVerts];
  memset(pObject->pVerts, 0, sizeof(Vec3) * pObject->numVerts);

  // Read in the array of vertices (an array of 3 floats)
  pPrev->read += fread(pObject->pVerts, 1, pPrev->len - pPrev->read, fp);

  // Now we should have all of the vertices read in.  Because 3D Studio Max
  // Models with the Z-Axis pointing up (strange and ugly I know!), we need
  // to flip the y values with the z values in our vertices.  That way it
  // will be normal, with Y pointing up.  If you prefer to work with Z pointing
  // up, then just delete this next loop.  Also, because we swap the Y and Z
  // we need to negate the Z to make it come out correctly.

  // Go through all of the vertices that we just read and swap the Y and Z values
  for (int i=0; i < pObject->numVerts; i++) {
    if (Endian::bigEndian()) { char t; SWAPL(&pObject->pVerts[i].x, t); }
    if (Endian::bigEndian()) { char t; SWAPL(&pObject->pVerts[i].y, t); }
    if (Endian::bigEndian()) { char t; SWAPL(&pObject->pVerts[i].z, t); }

    float fTmpY = pObject->pVerts[i].y;

    // Set the Y value to the Z value
    pObject->pVerts[i].y = pObject->pVerts[i].z;

    // Set the Z value to the Y value,
    // but negative Z because 3D Studio max does the opposite.
    pObject->pVerts[i].z = -fTmpY;
  }
}

/** reads in the material name assigned to the object and sets the materialID */
void _3ds::readMaterial(t3dsModel *pModel, tObject *pObject, tChunk *pPrev)
{
  char strMaterial[255] = {0};	// This is used to hold the objects material name
  int buffer[50000] = {0};	// This is used to read past unwanted data

  pPrev->read += getString(strMaterial);

  // Now that we have a material name, we need to go through all of the materials
  // and check the name against each material.  When we find a material in our material
  // list that matches this name we just read in, then we assign the materialID
  // of the object to that material index.  You will notice that we passed in the
  // model to this function.  This is because we need the number of textures.
  // Yes though, we could have just passed in the model and not the object too.

  // Go through all of the textures
  for (int i=0; i < pModel->numMaterials; i++) {
    // If the material we just read in matches the current texture name
    if (strcmp(strMaterial, pModel->pMaterials[i].strName) == 0) {
      // Set the material ID to the current index 'i' and stop checking
      pObject->materialID = i;

      // Now that we found the material, check if it's a texture map.
      // If the strFile has a string length of 1 and over it's a texture
      if (strlen(pModel->pMaterials[i].strFile) > 0) {
        // Set the object's flag to say it has a texture map to bind.
        pObject->bHasTexture = true;
      }
      break;
    }
    else {
      // Set the ID to -1 to show there is no material for this object
      pObject->materialID = -1;
    }
  }
  // Read past the rest of the chunk since we don't care about shared vertices
  pPrev->read += fread(buffer, 1, pPrev->len - pPrev->read, fp);
}

/** computes the normals and vertex normals of the objects */
void _3ds::computeNormals(t3dsModel *pModel)
{
  Vec3 v1, v2, vNormal, vPoly[3];
  int i;

  // If there are no objects, we can skip this part
  if (pModel->numObjects <= 0) return;

  // What are vertex normals?  And how are they different from other normals?
  // Well, if you find the normal to a triangle, you are finding a "Face Normal"
  // If you give OpenGL a face normal for lighting, it will make your object look
  // really flat and not very round.  If we find the normal for each vertex, it makes
  // the smooth lighting look.  This also covers up blocky looking objects and they appear
  // to have more polygons than they do.  Basically, what you do is first
  // calculate the face normals, then you take the average of all the normals around each
  // vertex. It's just averaging. That way you get a better approximation for that vertex.

  // Go through each of the objects to calculate their normals
  for (int index=0; index < pModel->numObjects; index++) {
    tObject *pObject = &(pModel->pObject[index]); // Get the current object

    Vec3 *pNormals     = new Vec3 [pObject->numFaces];
    Vec3 *pTmpNormals  = new Vec3 [pObject->numFaces];
    pObject->pNormals  = new Vec3 [pObject->numVerts];
    //echo("numFaces=%d", pObject->numFaces);

    for (i=0; i < pObject->numFaces; i++) { // all of the faces
      // To cut down LARGE code, we extract the 3 points of this face
      vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
      vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
      vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];

      // Now calculate the face normals (Get 2 vectors and find the cross product of those 2)
      v1 = Vec3::subVect(vPoly[0], vPoly[2]); // Get the vector of the polygon (we just need 2 sides for the normal)
      v2 = Vec3::subVect(vPoly[2], vPoly[1]); // Get a second r of the polygon
      vNormal  = Vec3::crossVect(v1, v2); // Return the cross product of the 2 vectors (normalize vector, but not a unit vector)
      pTmpNormals[i] = vNormal;   // Save the un-normalized normal for the vertex normals
      vNormal  = Vec3::normVect(vNormal); // Normalize the cross product to give us the polygons normal
      pNormals[i] = vNormal;	// Assign the normal to the list of normals
    }

    //// Now Get The Vertex Normals ////
    Vec3 vSum = {0.0, 0.0, 0.0};
    Vec3 vZero = vSum;
    int shared = 0;

    for (i=0; i < pObject->numVerts; i++) {	// all the vertices
      for (int j=0; j < pObject->numFaces; j++) {  // all the triangles
      	// Check if the vertex is shared by another face
        if (pObject->pFaces[j].vertIndex[0] == i ||
            pObject->pFaces[j].vertIndex[1] == i ||
            pObject->pFaces[j].vertIndex[2] == i) {
          vSum = Vec3::addVect(vSum, pTmpNormals[j]);// Add the un-normalized normal of the shared face
          shared++;		// Increase the number of shared triangles
        }
      }

      // Get the normal by dividing the sum by the shared.  We negate the shared so it has the normals pointing out.
      pObject->pNormals[i] = Vec3::divVect(vSum, float(-shared));
      pObject->pNormals[i] = Vec3::normVect(pObject->pNormals[i]);
      vSum = vZero;		// Reset the sum
      shared = 0;		// Reset the shared
    }

    // Free our memory and start over on the next object
    if (pTmpNormals) delete[] pTmpNormals;
    if (pNormals) delete[] pNormals;
  }
}
