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
// This header file holds the class information for OBJ.
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//---------------------------------------------------------------------------
#ifndef OBJ_HPP
#define OBJ_HPP

#include <iostream>
#include <string>
#include <vector>

#include "wobject.hpp"
#include "vec3.hpp"	// Vec3

#define MAX_TEXTURES 100	// The maximum amount of textures to load


/**
 * This is used for indexing into the vertex and texture coordinate arrays
 */
struct tOBJFace
{
  int vertIndex[4];	// indicies for the verts that make up this triangle
  int coordIndex[4];	// indicies for the tex coords to texture this face
};

/**
 * This holds the information for a material
 */
struct tOBJMaterialInfo
{
  char  strName[255];	// texture name
  char  strFile[255];	// texture file name (If this is set it's a texture map)
  uint8_t color[3];	// color of the object (R, G, B)
  int   texureId;  	// texture ID
  float uTile;		// u tiling of texture  (Currently not used)
  float vTile;		// v tiling of texture  (Currently not used)
  float uOffset;  	// u offset of texture  (Currently not used)
  float vOffset;  	// v offset of texture  (Currently not used)
} ;

/**
 * This holds all the information for our model/scene.
 */
struct tOBJObject
{
  int  numOfVerts;	// number of verts in the model
  int  numOfFaces;	// number of faces in the model
  int  numTexVertex;	// number of texture coordinates
  int  materialID;	// texture ID to use, which is the index into our texture array
  bool bHasTexture;	// This is TRUE if there is a texture map for this object
  char strName[255];	// name of the object
  Vec3  *pVerts;	// object's vertices
  Vec3  *pNormals;	// object's normals
  Vec2  *pTexVerts;	// texture's UV coordinates
  tOBJFace *pFaces;	// faces information of the object
};

/**
 * This holds our model information
 */
struct tOBJModel
{
  int numOfObjects; 		// number of objects in the model
  int numOfMaterials;		// number of materials for the model
  vector<tOBJMaterialInfo> pMaterials;	// list of material information (Textures and colors)
  vector<tOBJObject> pObject;	// object list for our model
};


/**
 * Class Obj Model
 */
class Obj {

private:
  tOBJModel OBJModel;	///< model

  bool loaded;		///< flag loaded or not
  float currentScale;	///< current scale
  float desiredScale;	///< desired scale
  GLint dlist;		///< displaylist
  char *url;		///< obj url
  FILE *fp;		///< file pointer to load the .obj file
  int flgpart;		///< flag display parts else body
  bool flgcolor;	///< flag color setted
  float mat_diffuse[4]; ///<
  float mat_ambient[4]; ///<
  float mat_specular[4];///<
  int textures[MAX_TEXTURES];

public:

  Obj(const char *url, int flgpart);	///< Constructor

  virtual ~Obj();	///< Destructor

  void setScale(float scale);
  /**< Sets the scale */

  void setColor(int type, float *color);
  /**< Sets the color */

  float getRadius();
  /**< Gets the radius */

  GLint displaylist();
  /**< Draws in displaylist */

  void draw();
  /**< Model drawing */

  virtual void render(float *color);
  virtual void render(const Pos &pos, float *color);
  /**< Model rendering */

  static void reader(void *aobj, class Http *http);

private:
  vector<Vec3>  pVertices;
  /**< This is an STL vector that holds a list of vertices */

  vector<tOBJFace> pFaces;
  /**< This is an STL vector that holds a list of faces */

  vector<Vec2>  pTextureCoords;
  /**< This is an STL vector that holds a list of UV Coordinates */

  bool bObjectHasUV;
  /**< This tells us if the current object has texture coordinates */
  bool bObjectHasNormal;
  /**< This tells us if the current object has normal coordinates */
  bool bJustReadAFace;
  /**< This tells us if we just read in face data so we can keep track of multiple objects */

  bool import(FILE *f);
  /**< File loader */

  bool importModel(tOBJModel *pModel);
  /**< Just pass in your model that will hold the information */

  bool importTextures();

  int openTexture(const char *imgfile);

  void readFile(tOBJModel *pModel);
  /**< This is the main loading loop that gets called in importModel() */

  void readVertexInfo();
  /**< This is called in ReadObjFile() if we find a line starting with 'v' */

  void readFaceInfo();
  /**< This is called in ReadObjFile() if we find a line starting with 'f' */

  void fillInObjectInfo(tOBJModel *pModel);
  /**< This is called when we are finished loading in the face information */

  void computeNormals(tOBJModel *pModel);
  /**< it's nice to have vertex normals for lighting */

  const char * getUrl() const;
  /**< get an Url */

};


#endif
