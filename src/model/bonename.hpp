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
#ifndef BONENAME_HPP
#define BONENAME_HPP

/**
 * BoneName class
 */
class Bonename {
 public:
  char *name;	///< name

  Bonename();
  Bonename(char *op);
  Bonename(Bonename *op);
  Bonename(Bonename &op);

  virtual ~Bonename();

  virtual void setName(char *zeName);

  virtual char *getName();
};

#endif
