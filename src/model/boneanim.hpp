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
#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP

#include "bonelist.hpp"
#include "bonevertex.hpp"
#include "bonelink.hpp"
#include "bonemesh.hpp"

//#define __GL_TEXTURE__
//#define __GL_LIGHTING__


/**
 * This is the main class that is to animate the bones
 * and the 3d mesh correctly... It creates the link
 * between the different vertices and bones and
 * calculates the new position each time you ask
 *
 * Typical use of this class :
 *   myBoneAnimator.registerMesh ( oneMesh );
 *   myBoneAnimator.registerSkeleton ( oneSkeleton );
 *   myBoneAnimator.compileLinkList ();
 *   ... in loop ...
 *     change skeleton current pos
 *   myBoneAnimator.animate ();
 *   myBoneAnimator.render ();
 *   ... end loop ...
 */
class BoneAnimator {
 public:
  /* What we'll animate */
  BoneMesh *mesh;
  BoneVertex *skel;

  /* Links management */
  BoneList < BoneLink > linkList;
  BoneLink **link;
  int links;
  int linkListCompiled;

  BoneAnimator();		///< Constructor
  virtual ~BoneAnimator();	///< Destructor

  // Accessing datas
  virtual void registerMesh(BoneMesh *zeMesh);
  virtual void registerSkeleton(BoneVertex *zeRoot);

  // Operations on both mesh and skeleton
  virtual void scale(float sx, float sy, float sz);

  // List compilation
  virtual void compileLinkList();
  virtual void emptyLinkList();
  virtual void genLinkList();
  inline float getLength(Vertex *vertex, BoneVertex *node);
  inline float getWeight(Vertex *vertex, BoneVertex *node);

  // Rendering and animating the mesh and skeleton
  virtual void render();
  virtual void renderSkeletonNode(BoneVertex *node);
  virtual void animate();
  virtual void animateSkeletonNode(BoneVertex *node);

 private:
  void addNodeAndChildren(BoneVertex *boneVertex, BoneList < BoneVertex > *list);
};

#endif
