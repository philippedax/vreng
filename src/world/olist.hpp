//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// olist.hpp
//---------------------------------------------------------------------------
#ifndef OLIST_HPP
#define OLIST_HPP


/**
 * OList Class
 */
class OList {
 public:
  class Object *object;	///< object ptr.
  class OList *next;	///< next.

  OList();		///< constructor.
  virtual ~OList();	///< destructor.

  void removeObject();
  /**< Frees an object list. */

  void clearPointed();
  /**< Clears flags "ispointed" of all objects in a list. */

  static Object *findObj(uint8_t type_id, uint32_t src_id, uint16_t port_id, uint16_t obj_id);
  /**< Returns the object identified by type and id from the mobile list. */
};


#endif
