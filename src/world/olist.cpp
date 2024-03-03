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
#include "stat.hpp"	// new_olist

#include <list>
#include <vector>
using namespace std;


OList::OList()
{
  new_olist++;
}

OList::~OList()
{
  del_olist++;
}

/* Removes all objects from a olist */
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
// called by col.cpp
void OList::clearIspointed()
{
  for (OList *l = this; l && l->pobject; l = l->next) {
    l->pobject->inlist = false;
  }
}

/* Returns the object from the mobile list */
// static
WO * OList::findObj(uint8_t _type, uint32_t src_id, uint16_t port_id, uint16_t obj_id)
{
  for (list<WO*>::iterator it = mobileList.begin(); it != mobileList.end() ; ++it) {
    if (((*it)->type == _type)
    &&  ((*it)->getSrc() == src_id)
    &&  ((*it)->getPort() == port_id)
    &&  ((*it)->getObj() == obj_id))
      return *it;
  }
  return NULL;
}

#if 0 //notused - static
void OList::remove(list<WO*> &olist)
{
  for (list<WO*>::iterator it = olist.begin(); it != olist.end(); ++it) {
    if (*it) {
      delete *it;
    }
  }
}

// static
void OList::clearIspointed(list<WO*> &olist)
{
  for (list<WO*>::iterator it = olist.begin(); it != olist.end(); ++it) {
    (*it)->inlist = false;
  }
}

/* Show an olist: debug only */
// static
void OList::show(list<WO*> &olist)
{
  show(olist, "List:");
}

/* Show an olist with its prefix: debug only */
// static
void OList::show(list<WO*> &olist, const char *str)
{
  echo("%s", str);

  for (list<WO*>::reverse_iterator it = olist.rbegin(); it != olist.rend(); ++it) {
    if ((*it)->name.instance)
      echo("t=%d m=%d n=%d %s", (*it)->type, (*it)->mode, (*it)->num, (*it)->name.instance);
    else
      echo("t=%d m=%d n=%d (null)", (*it)->type, (*it)->mode, (*it)->num);
  }
}
#endif //notused
