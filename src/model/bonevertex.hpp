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
// ------------------------------------------------------------ //
// Author   : Yann Renard 				        //
// Copyright: This file is totaly free and you may distribute   //
//            it to anyone you want, without modifying this     //
//            header. If you use it in a commercial project (?) //
//            or in bigger project (!), I would be glad to know //
//            about it :) Please mail me...                     //
//                myself_yr@hotmail.com                         //
// ------------------------------------------------------------ //
#ifndef BONEVERTEX_HPP
#define BONEVERTEX_HPP

#include "bonename.hpp"
#include "vec3.hpp"
#include "bonelist.hpp"


class BoneLink;

/**
 * BoneVertex class
 *
 * This class represents the extremity of a bone
 * So a bone is made of one of this class (that
 * contains translation/rotation) and a reference
 * to it's father ( the extremity it depends of )
 * This extremity also has several children whom
 * he'll be the father ( see that like a tree )
 */
class BoneVertex : public Bonename {
 public:
  // -> Position at the begining
  Vect3D initialPosition;
  float  initialAngle;
  Vect3D initialAxis;

  // -> Position during the animation
  Vect3D currentPosition;
  float  currentAngle;
  Vect3D currentAxis;

  // -> Children management
  BoneList < BoneVertex > childList;
  BoneVertex **child;
  BoneVertex *father;
  int children;
  int childListCompiled;

  // -> Links management
  // (a link is defined by a bone vertex, a vertex of the mesh and a weight)
  BoneList < BoneLink > linkList;
  BoneLink **link;
  int links;
  int linkListCompiled;

  // -> Saves of the matrices
  float initialMatrix[16];
  float initialMatrixInverted[16];
  float currentMatrix[16];
  // -> Same for rotation matrices (may be used for normal computations)
  float initialRotMatrix[16];
  float initialRotMatrixInverted[16];
  float currentRotMatrix[16];

  // -> a field to scale the influence of this boneVertex
  float influenceScaleFactor;

  // -> a field to tell if this boneVertex should animated or not
  int animated;

  // Construct / destruct
  BoneVertex();
  BoneVertex(Vect3D & zePosition, float zeAngle, Vect3D & zeAxis);
  BoneVertex(Vect3D * zePosition, float zeAngle, Vect3D * zeAxis);
  virtual ~BoneVertex();

  // Accessing initial position datas
  void setInitialPosition(Vect3D & zePosition);
  void setInitialPosition(Vect3D * zePosition);
  void setInitialPosition(float ox, float oy, float oz);
  void setInitialRotation(float zeAngle, Vect3D & zeAxis);
  void setInitialRotation(float zeAngle, Vect3D * zeAxis);
  void setInitialRotation(float zeAngle, float axisx, float axisy, float axisz);

  // Accessing current position datas ( during animation)
  void setPos(Vect3D & zePosition);
  void setPos(Vect3D * zePosition);
  void setPos(float ox, float oy, float oz);
  void setRot(float zeAngle, Vect3D & zeAxis);
  void setRot(float zeAngle, Vect3D * zeAxis);
  void setRot(float zeAngle, float axisx, float axisy, float axisz);

  // Accessing current position datas ( during animation)
  // with relative values (realtive to initial position)
  void resetPos(void);
  void resetRot(void);
  void setTrans(Vect3D & delta);
  void setTrans(Vect3D * delta);
  void setTrans(float dx, float dy, float dz);
  void setScale(float scalex, float scaley, float scalez);
  void setScale(float scale);

  // Actions on this node and its children
  // Those actions are definitives
  void scale(float sx, float sy, float sz);

  // Children list managing
  void setFather(BoneVertex *zeFather);
  void addChild(BoneVertex *newChild);
  void removeChild(const char *zeName);
  BoneVertex *getBone(const char *zeName);

  // Link list managing
  void addLink(BoneLink *zeLink);
  void removeLink(BoneLink *zeLink);

  // Intern functions to compile the lists... should be private maybe ?
  void compileChildList();
  void compileLinkList();
  void generateInitialMatrix(); // needs glPush / glPop and glLoadIdentity at the begining
  void generateCurrentMatrix(); // needs glPush / glPop and glLoadIdentity at the begining

  // I/O functions
  void read(char *filename, float size = 1.);
  void readFromFile(FILE *file, float size = 1.);

#if 0 //notused
  void write(char *filename);
  void writeToFile(FILE *file);
  void print(int cpt = 0, FILE *dest = stderr);
#endif

};

#if 0 //dax
// Reading part
inline char readChar(FILE *in)
{
  return fgetc(in);
}

inline short readShort(FILE *in)
{
  char b1 = fgetc(in);
  char b2 = fgetc(in);
  return (b1<<8) + b2;
}

inline int readInt(FILE *in)
{
  unsigned char b1 = fgetc(in);
  unsigned char b2 = fgetc(in);
  unsigned char b3 = fgetc(in);
  unsigned char b4 = fgetc(in);
  unsigned int result;
  result = b1;
  result <<= 8; result += b2;
  result <<= 8; result += b3;
  result <<= 8; result += b4;
  return result;
}

inline float readFloat(FILE *in)
{
  int value = readInt(in);
  float *valuePtr = (float *) &value;
  return *valuePtr;
}

inline char *readString(FILE *in, char *str)
{
  int cpt=0;
  while ((str[cpt++] = readChar(in)) != '\0');
  return str;
}
#endif //dax

// Writing part
#if 0 //notused
inline void writeChar(FILE *out, char data)
{
  fputc(data, out);
}

inline void writeShort(FILE *out, short data)
{
  fputc((data>>8) & 255, out);
  fputc( data     & 255, out);
}

inline void writeInt(FILE *out, int data)
{
  fputc((data>>24) & 255, out);
  fputc((data>>16) & 255, out);
  fputc((data>>8 ) & 255, out);
  fputc( data      & 255, out);
}

inline void writeFloat(FILE *out, float data)
{
  int *dataPtr = (int *) &data;
  writeInt(out, *dataPtr);
}

inline void writeString(FILE *out, char *data)
{
  int cpt = 0;
  while (data[cpt] != '\0')
    fputc(data[cpt++], out);
  fputc(data[cpt], out);
}
#endif


/**
 * Vertex class
 */
class Vertex {
 public:
  // Champs de la classe vertex
  Vect3D initialPosition;
  Vect3D currentPosition;
  Vect3D initialNormal;
  Vect3D currentNormal;

  // -> Liens
  BoneList < BoneLink > linkList;
  BoneLink **link;
  int links;
  int linkListCompiled;

  // -> Coordonnees de mapping
  float u,v;

  // Constructeurs / destructeur
  Vertex();
  Vertex(Vect3D & zePosition);
  Vertex(Vect3D * zePosition);
  Vertex(float ox, float oy, float oz);
  virtual ~Vertex() {}

  // Acces aux champs
  void setPosition(Vect3D & zePosition);
  void setPosition(Vect3D * zePosition);

  // Gestion des liens
  void addLink(BoneLink *zeLink);
  void removeLink(BoneLink *zeLink);

  // Acces aux champs de transformation initiaux
  void compileLinkList();

 private:
  void defaults();

};

#endif
