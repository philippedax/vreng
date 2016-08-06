//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
#include "vreng.hpp"
#include "olist.hpp"
#include "wobject.hpp"
#include "prof.hpp"	// new_objectlist

#include <list>
using namespace std;


ObjectList::ObjectList()
{
  new_objectlist++;
}

ObjectList::~ObjectList()
{
  del_objectlist++;
}

/* Removes all objects from a olist */
// static
void ObjectList::remove(list<WObject*> &olist)
{
  for (list<WObject*>::iterator it = olist.begin(); it != olist.end(); ++it)
    if (*it) delete *it;
}

// virtual
void ObjectList::remove()
{
  for (ObjectList *pl = this; pl ; ) {
    ObjectList *next = pl->next;
    if (pl && pl->pobject && pl->pobject->type)  delete pl;  //FIXME: BUG! macosx iconStick
    pl = next;
  }
}

/* Clears flags "ispointed" of all objects in an olist */
// static
void ObjectList::clearIspointedFlag(list<WObject*> &olist)
{
  for (list<WObject*>::iterator it = olist.begin(); it != olist.end(); ++it)
    (*it)->is_in_a_list = false;
}

// virtual
void ObjectList::clearIspointedFlag()
{
  for (ObjectList *pl = this; pl && pl->pobject; pl = pl->next)
    pl->pobject->is_in_a_list = false;
}

/* Returns the object from the mobile list */
// static
WObject * ObjectList::findInMobile(uint8_t _type, uint32_t src_id, uint16_t port_id, uint16_t obj_id)
{
  for (list<WObject*>::iterator it = mobileList.begin(); it != mobileList.end() ; ++it) {
    if (((*it)->type == _type)
    &&  ((*it)->getSrcId() == src_id)
    &&  ((*it)->getPortId() == port_id)
    &&  ((*it)->getObjId() == obj_id))
      return *it;
  }
  return NULL;
}

/* Show an olist: debug only */
// static
void ObjectList::show(list<WObject*> &olist)
{
  show(olist, "List:");
}

/* Show an olist with its prefix: debug only */
// static
void ObjectList::show(list<WObject*> &olist, const char *str)
{
  trace(DBG_FORCE, "\n%s", str);

  for (list<WObject*>::reverse_iterator it = olist.rbegin(); it != olist.rend(); ++it) {
    if ((*it)->names.instance)
      trace(DBG_FORCE, "t=%d m=%d n=%d %s", (*it)->type, (*it)->mode, (*it)->num, (*it)->names.instance);
    else
      trace(DBG_FORCE, "t=%d m=%d n=%d (null)", (*it)->type, (*it)->mode, (*it)->num);
  }
}
