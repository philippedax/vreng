//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
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
// oclass.cpp
//
// Object Class handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "oclass.hpp"
#include "str.hpp"	// mystrcasecmp
#include "netobj.hpp"	// Noid
#include "objects.hpp"	// OBJECTSNUMBER


OClass** OClass::otable = NULL;
uint16_t OClass::otable_size = 0;


/** Constructor */
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
    return;	// OK
  }
  if (! (otable = static_cast<OClass**> (realloc(otable, sizeof(OClass *) * (type_id+1))))) {
    fatal("can't realloc otable");
  }
  for (int i = otable_size; i < (type_id+1); i++) {
    otable[i] = NULL;
  }
  otable_size = type_id + 1;
  otable[type_id] = this;
  //echo("otable[%d] %s size=%d", type_id, type_name, otable_size);
}

/** Gets object class by type_name */
const OClass * OClass::getOClass(const char *type_name)
{
  for (int i=1; i < otable_size; i++) {
    if (otable[i]) {
      if (otable[i]->type_name) {
        if (type_name) {
          if (! mystrcasecmp(type_name, otable[i]->type_name)) {
            return otable[i];		// found return it !
          }
          if (! mystrcasecmp(type_name, "html")) {	// hugly hack!!!
            //dumpTable();
            fatal("bad world: type=html");
            Vreng::quit(1);
          }
        }
      }
      else {
        error("otable[%d]->type_name=%s", i, otable[i]->type_name);	// debug
      }
    }
  }
  error("type_name=%s not found, please upgrade VREng!", type_name);
  dumpTable();
  return NULL;
}

/** Gets object class by type_id */
const OClass * OClass::getOClass(uint8_t type_id)
{
  if (type_id >= otable_size) {
    error("getOClass: type_id=%d out of bounds", type_id); dumpTable();
    return NULL;
  }
  return otable[type_id];
}

/** Returns a wobject creator */
WO * OClass::creatorInstance(uint8_t type_id, char *l)
{
  if (! isValidType(type_id)) {
    error("creatorInstance: type_id=%d out of bounds", type_id);
    dumpTable();
    return NULL;
  }
  return otable[type_id]->creator(l);
}

/** Returns a wobject replicator */
WO * OClass::replicatorInstance(uint8_t type_id, Noid noid, Payload *pp)
{
  if (! isValidType(type_id)) {
    error("replicatorInstance: type_id=%d out of bounds", type_id); dumpTable();
    return NULL;
  }
  return otable[type_id]->replicator(type_id, noid, pp);
}

/** Dumps the otable */
void OClass::dumpTable()
{
  for (int i=1; i < otable_size; i++) {
    if (otable[i]) {
      echo("%02d: %s", otable[i]->type_id, otable[i]->type_name);
    }
    else {
      error("otable[%d] null -> segfault", i);
    }
  }
}

/** Is type valid ? */
bool OClass::isValidType(uint8_t type_id)
{
  return (type_id > 0 && type_id <= OBJECTSNUMBER);
}
