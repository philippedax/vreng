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
// * QUICK NOTES *
// This file is created in the hopes that you can just plug it into your code
// easily. You will probably want to query more chunks though for animation, ...
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//---------------------------------------------------------------------------
#ifndef _3DS_HPP
#define _3DS_HPP

#include "wobject.hpp"	// Pos
#include "vec3.hpp"	// Vec3
#include <vector>


/**
 * This is our face structure.
 */
struct t3dsFace {
  int vertIndex[3];	///< indicies for the verts that make up this triangle
  int coordIndex[3];	///< indicies for the tex coords to texture this face
};

/**
 * This holds all the information for our model/scene.
 */
struct tObject {
  int  numVerts;	///< The number of verts in the model
  int  numFaces;	///< The number of faces in the model
  int  numTexVertex;	///< The number of texture coordinates
  int  materialID;	///< The texture ID to use, which is the index into our texture array
  bool bHasTexture;	///< This is TRUE if there is a texture map for this object
  char strName[255];	///< The name of the object
  Vec3 *pVerts;		///< The object's vertices
  Vec3 *pNormals;	///< The object's normals
  Vec2 *pTexVerts;	///< The texture's UV coordinates
  t3dsFace *pFaces;	///< The faces information of the object
};

/**
 * This holds the information for a material.
 */
struct t3dsMaterialInfo {
  char  strName[255];	///< The texture name
  char  strFile[255];	///< The texture file name (If this is set it's a texture map)
  uint8_t color[3];	///< The color of the object (R, G, B)
  int   texureId;	///< the texture ID
  float uTile;		///< u tiling of texture  (Currently not used)
  float vTile;		///< v tiling of texture  (Currently not used)
  float uOffset;	///< u offset of texture  (Currently not used)
  float vOffset;	///< v offset of texture  (Currently not used)
};

/**
 * This holds our model information.
 * This should also turn into a robust class.
 * We use STL's (Standard Template Library) std::vector class
 * to ease our link list burdens. :)
 */
struct t3dsModel {
  int numObjects;		///< The number of objects in the model
  int numMaterials;		///< The number of materials for the model
  std::vector<t3dsMaterialInfo> pMaterials; ///< The list of material information (Textures and colors)
  std::vector<tObject> pObject;	///< The object list for our model
};

#if 0 //dax
/**
 * Here is our structure for our 3DS indicies
 * (since .3DS stores 4 unsigned shorts)
 */
struct tIndices {
  unsigned short a, b, c, bVisible;	///< This will hold point1, 2, and 3 index's into the vertex array plus a visible flag
};
#endif

/**
 * This holds the chunk info
 */
struct tChunk {
  unsigned short int ID;	///< The chunk's ID
  unsigned int length;		///< The length of the chunk
  unsigned int bytesRead;	///< The amount of bytes read within that chunk
};


/**
 * _3ds class
 */
class _3ds {

 private:
  t3dsModel _3dsModel;

  bool loaded;
  float currentScale;
  float desiredScale;
  GLint dlist;		///< display list
  char *url;		///< 3ds url
  FILE *fp;		///< The file pointer

 public:
  _3ds(const char *url);	///< Constructor

  virtual ~_3ds();		///< Destructor

  virtual void render(float *color);
  virtual void render(const Pos &pos, float *color);
  /**< Model rendering */

  virtual void setScale(float scale);
  /**< Changes scale */

  virtual float getRadius();
  /**< Gets spheric BB */

  virtual GLint displaylist();
  /**< Draws in display list */

 private:
  static void httpReader(void *_3ds, class Http *http);

  virtual void draw();
  /**< Model drawing */

  virtual bool importModel(t3dsModel *pModel);

  virtual bool importTextures();

  virtual int loadTexture(const char *imgFile);

  virtual void bindTexture2D(int textureId);

  virtual const char * getUrl() const;

  virtual bool loadFromFile(FILE *f);

  virtual int getString(char *);
  /**< This reads in a string and saves it in the char array passed in */

  virtual void readChunk(tChunk *);
  /**< This reads the next chunk */

  virtual void nextChunk(t3dsModel *pModel, tChunk *);
  /**< This reads the next large chunk */

  virtual void nextObjectChunk(t3dsModel *pModel, tObject *pObject, tChunk *);
  /**< This reads the object chunks */

  virtual void nextMaterialChunk(t3dsModel *pModel, tChunk *);
  /**< This reads the material chunks */

  virtual void readColorChunk(t3dsMaterialInfo *pMaterial, tChunk *pChunk);
  /**< This reads the RGB value for the object's color */

  virtual void readVertices(tObject *pObject, tChunk *);
  /**< This reads the objects vertices */

  virtual void readVertexIndices(tObject *pObject, tChunk *);
  /**< This reads the objects face information */

  virtual void readUVCoordinates(tObject *pObject, tChunk *);
  /**< This reads the texture coodinates of the object */

  virtual void readObjectMaterial(t3dsModel *pModel, tObject *pObject, tChunk *pPreviousChunk);
  /**< This reads in the material name assigned to the object and sets the materialID */

  virtual void computeNormals(t3dsModel *pModel);
  /**< This computes the vertex normals for the object (used for lighting) */

};

#endif
