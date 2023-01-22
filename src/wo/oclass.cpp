//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://www.vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
#include "vreng.hpp"
#include "oclass.hpp"
#include "str.hpp"	// mystrcasecmp
#include "netobj.hpp"	// Noid
#include "objects.hpp"	// OBJECTSNUMBER

using namespace std;


OClass** OClass::otable = NULL;
uint16_t OClass::otable_size = 0;

// local
static const OClass end(0, "End", NULL);


/** OClass constructor */
OClass::OClass(uint8_t _type_id, const char* _type_name,
	       WCreator _creator, WReplicator _replicator, WBuiltin _builtin) :
  type_id(_type_id),
  type_name(_type_name),
  creator(_creator),
  replicator(_replicator),
  builtin(_builtin)
{
  if (type_id < otable_size) {
    otable[type_id] = this;
  }
  else {
    if (! (otable = (OClass**) realloc(otable, sizeof(OClass *) * (type_id+1)))) {
      fatal("can't realloc otable");
    }
    for (uint8_t i = otable_size; i < (type_id + 1); i++) {
      otable[i] = (OClass *) NULL;
    }
    otable_size = type_id + 1;
    otable[type_id] = this;
  }
  //echo("otable[%d] %s size=%d", type_id, type_name, otable_size);
}

const OClass * OClass::getOClass(const char *type_name)
{
  for (int i=1; i < otable_size; i++) {
    if (otable[i]) {
      if (otable[i]->type_name) {
        if (type_name) {
          if (! mystrcasecmp(type_name, otable[i]->type_name)) {
            return otable[i];
          }
        }
      }
      else {
        echo("otable[%d]->type_name=%s", i, otable[i]->type_name);
      }
    }
  }
  error("type_name=%s not found, please upgrade VREng!", type_name);
  dumpTable();
  return (OClass *) NULL;
}

const OClass * OClass::getOClass(uint8_t type_id)
{
  if (type_id < otable_size) {
    return otable[type_id];
  }
  error("getOClass: type_id=%d out of bounds", type_id); dumpTable();
  return (OClass *) NULL;
}

WObject * OClass::creatorInstance(uint8_t type_id, char *l)
{
  if (isValidType(type_id)) {
    return otable[type_id]->creator(l);
  }
  error("creatorInstance: type_id=%d out of bounds", type_id); dumpTable();
  return NULL;
}

void OClass::builtinInstance(uint8_t type_id)
{
  if (isValidType(type_id)) {
    otable[type_id]->builtin();
  }
  else {
    error("builtinInstance: type_id=%d out of bounds", type_id);
  }
}

WObject * OClass::replicatorInstance(uint8_t type_id, Noid noid, Payload *pp)
{
  if (isValidType(type_id)) {
    return otable[type_id]->replicator(type_id, noid, pp);
  }
  error("replicatorInstance: type_id=%d out of bounds", type_id); dumpTable();
  return NULL;
}

void OClass::dumpTable()
{
  for (int i=1; i < otable_size; i++) {
    printf("%02d: %p %02d %s\n", i, otable[i], otable[i]->type_id, otable[i]->type_name);
  }
}

bool OClass::isValidType(uint8_t type_id)
{
  return (type_id > 0 && type_id <= OBJECTSNUMBER);
}
