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
#include "noid.hpp"
#include "vrep.hpp"	// VREP
#include "netobj.hpp"	// NetObj
#include "nsl.hpp"	// inet4_ntop
#include "payload.hpp"	// Payload
#include "oclass.hpp"	// isValidType

#include <list>
using namespace std;


/*
 * Handling id objects names
 */

/* Builds a concataned string name */
char * Noid::getNetNameById()
{
  static char str[80];

  sprintf(str, "%x/%x/%x", ntohl(src_id), ntohs(port_id), ntohs(obj_id));
  return str;
}

/* Returns 0 if different, other if equal */
bool Noid::equalNoid(Noid n2) const
{
  return src_id == n2.src_id && port_id == n2.port_id && obj_id == n2.obj_id;
}

/* Gets a NetObj by name */
NetObj * Noid::getNetObj()
{
  list<NetObj*>::iterator it;
  for (list<NetObj*>::iterator it = NetObj::getList(); it != NetObj::netobjList.end(); ++it) {
    if (! equalNoid((*it)->noid)) {
      if (! OClass::isValidType((*it)->type)) {
        error("getNetObj: bad type=%d", (*it)->type); return NULL;
      }
    }
  }
  return *it;
}

/* Heuristic to avoid to send bunch of Query */
int Noid::filterQuery()
{
  static Noid oldnoid;
  static int countDelta = 0;

  if (equalNoid(oldnoid)) {
    // last request was on this name
    if (++countDelta <= 15)
      // 15: 10 proprietes en moyenne, avec 15 on saute donc
      // sans doute un "bloc" de deltas, mais sans doute pas deux
      return 0;    // cancel this request
  }
  // it's another one
  oldnoid = *this;
  countDelta = 0;
  return 1;
}

/* Send a Query '0x03' packet  to the unicast sender */
void Noid::sendQueryNoid(const struct sockaddr_in *to)
{
  if (! filterQuery()) return;

  Payload pp;
  pp.putPayload("cn", VREP_QUERY, this);
  trace(DBG_NET, "sendQuery: nobj=%s to=%s", getNetNameById(), inet4_ntop(&to->sin_addr));
  pp.sendPayload(to);
}

/* Send a Delete '0x04' packet */
void Noid::sendDeleteNoid(const struct sockaddr_in *to)
{
  Payload pp;
  pp.putPayload("cn", VREP_DELETE, this);
  trace(DBG_NET, "sendDelete: nobj=%s to=%s", getNetNameById(), inet4_ntop( &to->sin_addr));
  pp.sendPayload(to);
}
