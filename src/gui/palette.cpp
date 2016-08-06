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
/*
 *  palette.cpp : Ubit Palette for the VREng GUI
 *
 *  VREng / Ubit Project
 *  Authors: Eric Lecolinet and Philippe Dax
 *
 *  Ubit Toolkit: A Brick Construction Game Model for Creating GUIs
 *  Please refer to the Ubit GUI Toolkit Home Page for details.
 *
 *  (C) 2002/2009 Eric Lecolinet - ENST Paris
 *  WWW: http://www.enst.fr/~elc/ubit
 */

#include "vreng.hpp"
#include "widgets.hpp"
#include "panels.hpp"
#include "palette.hpp"


Palette::Palette(UArgs a) :
ppos(new UPos),
ptitle(new ULabel(" ")),
piconify(new UBox),
ptitlebox(new UBox),
pcontent(new UBox(a))
{
  piconify->addAttr(UColor::red + UFont::bold + ucall(this, &Palette::titleCB));
  piconify->add(UOn::select / ustr(" ^ ") + UOn::deselect / ustr(" X "));

  ptitle->addAttr(UOrient::horizontal  // + new UPosChooser(*ppos) DECONNE !!!
                  + UOn::doubleClick / ucall(this, &Palette::titleCB));

  ptitlebox->addAttr(UOrient::horizontal);
  ptitlebox->add(uleft() + *ptitle + uhflex() + ulabel("") + uright() + *piconify);

  pcontent->addAttr(UOrient::vertical);

  addAttr(*ppos + UOrient::vertical);
  add(*ptitlebox + uvflex() + *pcontent);
}

void Palette::collapse()
{
  pcontent->show(false);
}

void Palette::expand()
{
  pcontent->show(true);
}

void Palette::titleCB()
{
  pcontent->show(! pcontent->isShown());
}

Palette& Palette::setTitle(UArgs a)
{
  ptitle->removeAll();
  ptitle->add(a);
  return *this;
}

Palette& Palette::setPos(ULength x, ULength y)
{
  ppos->set(x,y);
  return *this;
}

Palette& Palette::setPos(UPoint& p)
{
  *ppos = p;
  return *this;
}

Palette& Palette::setPos(UPos& p)
{
  *ppos = p;
  return *this;
}
