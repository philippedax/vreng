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
#include "vreng.hpp"
#include "vertex.hpp"
#include "bonelink.hpp"


Vertex::Vertex()
{
  defaults();
}

Vertex::Vertex(Vect3D &zePosition)
{
  initialPosition = zePosition;
  currentPosition = zePosition;
  defaults();
}

Vertex::Vertex(Vect3D *zePosition)
{
  initialPosition = *zePosition;
  currentPosition = *zePosition;
  defaults();
}

Vertex::Vertex(float ox, float oy, float oz)
{
  initialPosition.set(ox, oy, oz);
  currentPosition.set(ox, oy, oz);
  defaults();
}

void Vertex::defaults()
{
  link  = NULL;
  links = 0;
  linkListCompiled = 0;
  initialNormal.reset();
  currentNormal.reset();
  u = -1.;
  v = -1.;
}

void Vertex::setPosition(Vect3D &zePosition)
{
  initialPosition =  zePosition;
  currentPosition =  zePosition;
}

void Vertex::setPosition(Vect3D *zePosition)
{
  initialPosition = *zePosition;
  currentPosition = *zePosition;
}

void Vertex::addLink(BoneLink *zeLink)
{
  linkList.addElement(zeLink);
  linkListCompiled = 0;
}

void Vertex::removeLink(BoneLink *zeLink)
{
  linkList.removeElement(zeLink);
  linkListCompiled = 0;
}

void Vertex::compileLinkList()
{
  link = linkList.getNiceTable(&links);
  linkListCompiled = 1;
}
