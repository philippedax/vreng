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


OList::OList()
{
  new_objectlist++;
}

OList::~OList()
{
  del_objectlist++;
}

/* Removes all objects from a olist */
// static
void OList::remove(list<WObject*> &olist)
{
  for (list<WObject*>::iterator l = olist.begin(); l != olist.end(); ++l) {
    if (*l) delete *l;
  }
}

// virtual
void OList::remove()
{
  for (OList *l = this; l ; ) {
    OList *next = l->next;
    if (l && l->pobject && l->pobject->type)  delete l;  //FIXME: BUG! macosx iconStick
    l = next;
  }
}

/* Clears flags "ispointed" of all objects in an olist */
// static
void OList::clearIspointed(list<WObject*> &olist)
{
  for (list<WObject*>::iterator l = olist.begin(); l != olist.end(); ++l) {
    (*l)->is_in_a_list = false;
  }
}

// virtual
void OList::clearIspointed()
{
  for (OList *l = this; l && l->pobject; l = l->next) {
    l->pobject->is_in_a_list = false;
  }
}

/* Returns the object from the mobile list */
// static
WObject * OList::findInMobile(uint8_t _type, uint32_t src_id, uint16_t port_id, uint16_t obj_id)
{
  for (list<WObject*>::iterator o = mobileList.begin(); o != mobileList.end() ; ++o) {
    if (((*o)->type == _type)
    &&  ((*o)->getSrcId() == src_id)
    &&  ((*o)->getPortId() == port_id)
    &&  ((*o)->getObjId() == obj_id))
      return *o;
  }
  return NULL;
}

/* Show an olist: debug only */
// static
void OList::show(list<WObject*> &olist)
{
  show(olist, "List:");
}

/* Show an olist with its prefix: debug only */
// static
void OList::show(list<WObject*> &olist, const char *str)
{
  trace(DBG_FORCE, "\n%s", str);

  for (list<WObject*>::reverse_iterator o = olist.rbegin(); o != olist.rend(); ++o) {
    if ((*o)->names.instance)
      trace(DBG_FORCE, "t=%d m=%d n=%d %s", (*o)->type, (*o)->mode, (*o)->num, (*o)->names.instance);
    else
      trace(DBG_FORCE, "t=%d m=%d n=%d (null)", (*o)->type, (*o)->mode, (*o)->num);
  }
}
