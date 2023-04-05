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
#include "stat.hpp"	// new_objectlist

#include <list>
#include <vector>
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
// virtual
void OList::remove()
{
  for (OList *l = this; l ; ) {
    OList *next = l->next;
    if (l && l->pobject && l->pobject->type) {
      delete l;  	//FIXME: BUG! macosx iconStick
    }
    l = next;
  }
}

/* Clears flags "inlist" of all objects in an olist */
// virtual - called by col.cpp
void OList::clearIspointed()
{
  for (OList *l = this; l && l->pobject; l = l->next) {
    l->pobject->inlist = false;
  }
}

// static
void OList::clearIspointed(list<WObject*> &olist)
{
  for (list<WObject*>::iterator it = olist.begin(); it != olist.end(); ++it) {
    (*it)->inlist = false;
  }
}

/* Returns the object from the mobile list */
// static
WObject * OList::findMobile(uint8_t _type, uint32_t src_id, uint16_t port_id, uint16_t obj_id)
{
  for (list<WObject*>::iterator it = mobileList.begin(); it != mobileList.end() ; ++it) {
    if (((*it)->type == _type)
    &&  ((*it)->getSrc() == src_id)
    &&  ((*it)->getPort() == port_id)
    &&  ((*it)->getObj() == obj_id))
      return *it;
  }
  return NULL;
}

#if 0 //notused - static
void OList::remove(list<WObject*> &olist)
{
  for (list<WObject*>::iterator it = olist.begin(); it != olist.end(); ++it) {
    if (*it) {
      delete *it;
    }
  }
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
  echo("%s", str);

  for (list<WObject*>::reverse_iterator it = olist.rbegin(); it != olist.rend(); ++it) {
    if ((*it)->names.instance)
      echo("t=%d m=%d n=%d %s", (*it)->type, (*it)->mode, (*it)->num, (*it)->names.instance);
    else
      echo("t=%d m=%d n=%d (null)", (*it)->type, (*it)->mode, (*it)->num);
  }
}
#endif //notused
