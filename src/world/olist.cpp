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
// olist.cpp
//
// Objects Lists handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "olist.hpp"
#include "wobject.hpp"	// WO
#include "stat.hpp"	// new_olist, del_olist

#include <list>


/** Constructor */
OList::OList()
{
  new_olist++;
}

/** Destructor */
OList::~OList()
{
  del_olist++;
}

/** Removes all objects from a olist */
void OList::remove()
{
  for (OList *list = this; list ; ) {
    OList *next = list->next;
    if (list && list->pobject && list->pobject->type) {
      delete list;
    }
    list = next;
  }
}

/** Clears flags "pointed" of all objects in an olist - called by col.cpp */
void OList::clearPointed()
{
  for (OList *list = this; list && list->pobject; list = list->next) {
    list->pobject->pointed = false;
  }
}

/** Returns the object from the mobile list - static */
WO * OList::findObj(uint8_t _type, uint32_t src_id, uint16_t port_id, uint16_t obj_id)
{
  for (std::list<WO*>::iterator it = mobileList.begin(); it != mobileList.end() ; ++it) {
    if (((*it)->type == _type)
    &&  ((*it)->getSrc() == src_id)
    &&  ((*it)->getPort() == port_id)
    &&  ((*it)->getObj() == obj_id))
      return *it;
  }
  return NULL;
}
