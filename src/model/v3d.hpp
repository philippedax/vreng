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
// ------------------------------------------------------------ //
// Author   : This file has been written by Yann Renard         //
// Copyright: This file is totaly free and you may distribute   //
//            it to anyone you want, without modifying this     //
//            header. If you use it in a commercial project (?) //
//            or in bigger project (!), I would be glad to know //
//            about it :) Please mail me...                     //
//            be glad to know about it, please mail me          //
//                myself_yr@hotmail.com                         //
// ------------------------------------------------------------ //
#ifndef V3D_HPP
#define V3D_HPP

#include "wobject.hpp"
#include "bone.hpp"	// The bone class

class Http;

/**
 * V3d class
 */
class V3d {
  friend class Body;		///< SCALE
  friend class Humanoid;	///< actions

 protected:
  static const float SCALE;

  BoneList < char > urlList;	///< list of v3d faces url

  int currentUrl;	///< current url
  GLint textureId;	///< texture id
  bool indexed;		///< flag if url index
  bool moveYes;		///< flag move yes
  bool moveNo;		///< flag move no
  bool moveMouth;	///< flag move mouth
  bool moveSmile;	///< flag move smile
  bool moveSulk;	///< flag move sulk
  bool moveEyeL;	///< flag move left eye
  bool moveEyeR;	///< flag move right eye
  bool moveNose;	///< flag love nose
  char cachefile[64];	///< file path in cache

  BoneMesh *mesh;	///< mesh
  BoneVertex *root;	///< bone root
  Bone bone;		///< bone animator

 public:
  V3d();			///< Constructor
  V3d(const char *urlindex);	///< Constructor
  virtual ~V3d();		///< Destructor

  virtual void render();
  /**< Render the face */

  void animate();
  void animate(int fapn, int value);
  /**< Animate the face */

  void change();
  /**< Change the face */

  void load(const char *url);
  /**< Load the face */

  void animEyeBall(float angle, const char *_side, int dir);
  void animEyeLid(float angle, const char *root1, const char *root2, const char *left, const char *right);
  void animEyeBrow(float angle, const char *_root, const char *_side);
  void animLip(float angle, const char *_side);
  void animNose(float angle, const char *_side);
  void animHead(float angle, int x, int y, int z);

 private:
  static void reader(void *_url, Http *http);
  static void v3dsreader(void *_o, Http *http);

  // GUI callbacks
  static void changeMoveYes(class Humanoid *o, void *d, time_t s, time_t u);
  static void changeMoveNo(class Humanoid *o, void *d, time_t s, time_t u);
  static void changeMoveMouth(class Humanoid *o, void *d, time_t s, time_t u);
  static void changeMoveSmile(class Humanoid *o, void *d, time_t s, time_t u);
  static void changeMoveSulk(class Humanoid *o, void *d, time_t s, time_t u);
  static void changeMoveEyeR(class Humanoid *o, void *d, time_t s, time_t u);
  static void changeMoveEyeL(class Humanoid *o, void *d, time_t s, time_t u);
  static void changeMoveNose(class Humanoid *o, void *d, time_t s, time_t u);
  static void changeFace(class Humanoid *o, void *d, time_t s, time_t u);
};


#endif
