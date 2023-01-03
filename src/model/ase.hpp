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
// This file holds the class and defines to load a .Ase file.  The .Ase file is
// created by 3D Studio Max, but isn't as nice as the .3DS file format.
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//---------------------------------------------------------------------------
#ifndef ASE_HPP
#define ASE_HPP

#include <iostream>
#include <string>
#include <vector>

#include "vec3.hpp"	// Vec3


/**
 * This is our face structure.
 * This is is used for indexing into the vertex and texture coordinate arrays.
 */
struct tASEFace {
  int vertIndex[3];	///< indicies for the verts that make up this triangle
  int coordIndex[3];	///< indicies for the tex coords to texture this face
};

/**
 * This holds all the information for our model/scene.
 */
struct tASEObject {
  int numVerts;		///< The number of verts in the model
  int numFaces;		///< The number of faces in the model
  int numTexVertex;	///< The number of texture coordinates
  int materialID;	///< The texture ID to use, which is the index into our texture array
  bool bHasTexture;	///< TRUE if there is a texture map for this object
  char strName[255];	///< The name of the object
  Vec3 *pVerts;		///< The object's vertices
  Vec3 *pNormals;	///< The object's normals
  Vec2 *pTexVerts;	///< The texture's UV coordinates
  tASEFace *pFaces;	///< The faces information of the object
};

/**
 * This holds the information for a material.
 */
struct tASEMaterialInfo
{
  char  strName[255];	///< The texture name
  char  strFile[255];	///< The texture file name (If this is set it's a texture map)
  uint8_t  color[3];	///< The color of the object (R, G, B) from 0 to 255
  float fColor[3];	///< The color of the object (R, G, B) from 0 to 1
  int   texureId;	///< the texture ID
  float uTile;		///< u tiling of texture  (Currently not used)
  float vTile;		///< v tiling of texture  (Currently not used)
  float vOffset;	///< v offset of texture  (Currently not used)
};

/**
 * This holds our model information.
 */
struct tASEModel {
  int numObjects;		///< The number of objects in the model
  int numMaterials;		///< The number of materials for the model
  std::vector<tASEMaterialInfo> pMaterials;	///< The list of material information (Textures and colors)
  std::vector<tASEObject> pObject;	///< The object list for our model
};


/**
 * This class holds all the data and function for loading in a .Ase file.
 */
class Ase {

private:
  tASEModel ASEModel;	///< model ptr

  bool loaded;		///< flag loaded or not
  float currentScale;	///< current scale
  float desiredScale;	///< desired scale
  GLint dlist;		///< display list
  char *url;		///< ase url
  FILE *fp;		///< file pointer to load the .ase file

 public:
  Ase(const char *url);		///< Constructor

  virtual ~Ase();		///< Destructor

  void setScale(float scale);
  /**< Sets the scale */

  float getRadius();
  /**< Gets the radius */

  GLint displaylist();
  /**< Draws in displaylist */

  virtual void render();
  /**< Model rendering */

 private:
  static void httpReader(void *aase, class Http *http);

  void draw();
  /**< Model drawing */

  bool loadFromFile(FILE *f);
  /**< File loader */

  const char * getUrl() const;
  /**< get an Url */

  bool importModel(tASEModel *pModel);

  bool importTextures();

  int loadTexture(const char *imgFile);

  void bindTexture2D(int textureId);

  void readFile(tASEModel *pModel);
  /**< This is the main loop that parses the .ase file */

  int getObjectCount();
  /**< This returns the number of objects in the .ase file */

  int getMaterialCount();
  /**< This returns the number of materials in the .ase file */

  void getTextureInfo (tASEMaterialInfo *pTexture, int desiredMaterial);
  /**< This fills in the texture information for a desired texture */

  void moveToObject (int desiredObject);
  /**< This moves our file pointer to the desired object */

  float readFloat();
  /**< This reads in a float from the file */

  void readObjectInfo(tASEObject *pObject, int desiredObject);
  /**< This reads a desired object's information (face, vertex and texture coord counts) */

  void getTextureName (tASEMaterialInfo  *pTexture);
  /**< This gets the name of the texture */

  void getMaterialName(tASEMaterialInfo *pTexture);
  /**< This gets the name of the material */

  void readObjectData(tASEModel *pModel, tASEObject *pObject, int desiredObject);
  /**< This loads all the data for the desired object */

  void getData(tASEModel *pModel, tASEObject *pObject, char *strDesiredData, int desiredObject);
  /**< This is the main load loop inside of readObjectData() that calls smaller load functions */

  void readVertex(tASEObject *pObject);
  /**< This reads in a vertice from the file */

  void readTextureVertex(tASEObject *pObject, tASEMaterialInfo texture);
  /**< This reads in a texture coordinate from the file */

  void readFace(tASEObject *pObject);
  /**< This reads in the vertex indices for a face */

  void readTextureFace(tASEObject *pObject);
  /**< This reads in texture coordinate indices for a face */

  void computeNormals(tASEModel *pModel);
  /**< This computes the vertex normals for our objects */

};

#endif
