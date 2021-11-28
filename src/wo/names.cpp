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
#include "vreng.hpp"
#include "wobject.hpp"
#include "world.hpp"	// getName()


#define NAME_HASH_SIZE	5423
#define NAME_DELETED	"XXXXXXX"

/**
 * hash entry
 */
struct hash_elt {
  WObject *po;			///< object ptr
  char name[OBJNAME_LEN];	///< object name
};


// local
static struct hash_elt hashtable[NAME_HASH_SIZE];


void World::initNames()
{
  for (int i=0; i < NAME_HASH_SIZE; i++) {
    memset(hashtable[i].name, 0, OBJNAME_LEN);
  }
}

static uint32_t hash_name(const char *s)
{
  uint32_t h;

  for (h=0; *s ; s++) {
    h = ((h << 8) + *s) % NAME_HASH_SIZE;
  }
  return h;
}

void WObject::setObjName(const char *name)
{
  if (! name) return;

  char fullname[OBJNAME_LEN];
  sprintf(fullname, "%s@%s", name, World::current()->getName());
  uint32_t hval = hash_name(fullname);
  while (hval) {
    if ((*(hashtable[hval].name) == '\0') ||
	(! strcmp(hashtable[hval].name, NAME_DELETED))) {
      strcpy(hashtable[hval].name, fullname);
      hashtable[hval].po = this;
      return;
    }
    hval = (hval + 1) % NAME_HASH_SIZE;
  }
}

WObject * WObject::getObjectByName(const char *name)
{
  if (! name) return (WObject *) NULL;

  char fullname[OBJNAME_LEN];
  sprintf(fullname, "%s@%s", name, World::current()->getName());
  uint32_t hval = hash_name(fullname);
  trace(DBG_WO, "getObjectByName: hval=%d name=%s", hval, fullname);
  while (hval) {
    if (*(hashtable[hval].name) == '\0') {
      return (WObject *) NULL;		// not found
    }
    if (! strcmp(hashtable[hval].name, fullname)) {
      return hashtable[hval].po;	// found
    }
    hval = (hval + 1) % NAME_HASH_SIZE;
  }
  return (WObject *) NULL;		// not found
}

#if 0 //not used
void delObjectName(const char *name)
{
  if (! name) return;

  char fullname[OBJNAME_LEN];
  sprintf(fullname, "%s@%s", name, World::current()->getName());
  uint32_t hval = hash_name(fullname);
  while (hval) {
    if (*(hashtable[hval].name) == '\0') return;
    if (! strcmp(hashtable[hval].name, fullname)) {
      strcpy(hashtable[hval].name, NAME_DELETED);	// found, mark deleted
      return;
    }
    hval = (hval + 1) % NAME_HASH_SIZE;
  }
}
#endif
