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
// Author   : Yann Renard				        //
// Copyright: This file is totaly free and you may distribute   //
//            it to anyone you want, without modifying this     //
//            header. If you use it in a commercial project (?) //
//            or in bigger project (!), I would be glad to know //
//            about it :) Please mail me...                     //
//                myself_yr@hotmail.com                         //
// ------------------------------------------------------------ //
#include "vreng.hpp"
#include "bonelink.hpp"


BoneLink::BoneLink(Vertex *zeVertex, BoneVertex *zeBoneVertex = NULL, float zeWeight = 0.)
{
  vertex        = NULL;
  boneVertex    = NULL;
  setVertex(zeVertex);
  setBoneVertex(zeBoneVertex);
  setWeight(zeWeight);
}

BoneLink::~BoneLink()
{
  if (vertex) vertex->removeLink(this);
  if (boneVertex) boneVertex->removeLink(this);
}

void BoneLink::setVertex(Vertex *zeVertex)
{
  if (vertex) vertex->removeLink(this);
  vertex = zeVertex;
}

void BoneLink::setBoneVertex(BoneVertex *zeBoneVertex)
{
  if (boneVertex) boneVertex->removeLink(this);
  boneVertex = zeBoneVertex;
}

void BoneLink::notifyTarget()
{
  if (vertex) vertex->addLink(this);
  if (boneVertex) boneVertex->addLink(this);
}

void BoneLink::setWeight(float zeWeight)
{
  weight = zeWeight;
}
