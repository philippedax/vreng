//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// face.hpp
//
// Author   : This file has been written by Yann Renard
// Copyright: This file is totaly free and you may distribute
//            it to anyone you want, without modifying this
//            header. If you use it in a commercial project (?)
//            or in bigger project (!), I would be glad to know
//            about it :) Please mail me...
//            be glad to know about it, please mail me myself_yr@hotmail.com
//---------------------------------------------------------------------------
#ifndef FACE_HPP
#define FACE_HPP

#include "object.hpp"	// Pos
#include "bone.hpp"	// The bone class

#include <list>
#include <vector>


/**
 * Face class
 */
class Face {
  friend class Body;		///< SCALE
  friend class Humanoid;	///< actions

 private:
  static const float SCALE;

  BoneList <char> urlList;///< list of v3d faces url
  //dax std::vector<char*> urlList;	///< list of v3d faces url

  uint8_t iurl;		///< current index url
  uint8_t nburl;	///< number of urls
  bool indexed;		///< flag if url index
  bool moveYes;		///< flag move yes
  bool moveNo;		///< flag move no
  bool moveMouth;	///< flag move mouth
  bool moveSmile;	///< flag move smile
  bool moveSulk;	///< flag move sulk
  bool moveEyeL;	///< flag move left eye
  bool moveEyeR;	///< flag move right eye
  bool moveNose;	///< flag love nose
  char pathfile[64];	///< file path in cache

  BoneMesh *mesh;	///< mesh
  BoneVertex *skel;	///< bone root
  Bone *bone;		///< bone animator

 public:
  Face();			///< Constructor
  Face(const char *urlindex);	///< Constructor
  ~Face();			///< Destructor

  void render();
  /**< Render the face */

  void play();
  void play(int fapn, float angle);
  /**< Animates the face */

  void change();
  /**< Change the face */

  void load(const char *url);
  /**< Load the face */

  void animHead(float a, int x, int y, int z);
  void animEyeBall(float a, const char *side, int dir);
  void animEyeLid(float a, const char *root1, const char *root2, const char *left, const char *right);
  void animEyeBrow(float a, const char *_root, const char *side);
  void animLip(float a, const char *side);
  void animNose(float a, const char *side);

 private:
  static void facesreader(void *_o, class Http *http);

  // GUI callbacks
  static void Yes(class Humanoid *o, void *d, time_t s, time_t u);
  static void No(class Humanoid *o, void *d, time_t s, time_t u);
  static void Mouth(class Humanoid *o, void *d, time_t s, time_t u);
  static void Smile(class Humanoid *o, void *d, time_t s, time_t u);
  static void Sulk(class Humanoid *o, void *d, time_t s, time_t u);
  static void EyeR(class Humanoid *o, void *d, time_t s, time_t u);
  static void EyeL(class Humanoid *o, void *d, time_t s, time_t u);
  static void Nose(class Humanoid *o, void *d, time_t s, time_t u);
  static void changeFace(class Humanoid *o, void *d, time_t s, time_t u);
};

#endif
