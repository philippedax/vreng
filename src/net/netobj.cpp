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
#include "netobj.hpp"
#include "vrep.hpp"	// VREP
#include "nsl.hpp"	// inet4_ntop
#include "payload.hpp"	// Payload
#include "netprop.hpp"	// NetProperty
#include "channel.hpp"	// Channel
#include "oclass.hpp"	// isValidType
#include "wobject.hpp"	// WObject
#include "stat.hpp"	// new_netobject

#include <list>
using namespace std;

// local

list<NetObject*> NetObject::netobjectList;

// network ids
uint32_t NetObject::mySsrcId = 0;   // ssrc network format
uint32_t NetObject::myHostId = 0;   // addr_IP network format
uint16_t NetObject::myPortId = 0;   // port network format
uint16_t NetObject::myObjId = 0;    // new object'id (naming), fmt host
uint32_t NetObject::myMgrSsrcId = 0; // manager ssrc network format


/*
 * Handling net id names and properties
 */

/* NetObject constructors */

void NetObject::defaults()
{
  new_netobject++;
  nbprop = 0;
  netprop = NULL;
}

/* Creates local generic NetObject */
NetObject::NetObject()
{
  defaults();
  type = 0;
  pobject = NULL;
  state = NET_PERMANENT;
  setNoid();
}

/* Creates local permanent NetObject */
NetObject::NetObject(WObject *po, uint8_t nprop, uint16_t oid)
{
  defaults();
  type = po->typeId();
  pobject = po;
  state = NET_PERMANENT;
  setPropertiesNumber(nprop);

  char str[80];
  sprintf(str, "%d/%d", type, oid);
  setNetName(str, state);     // net objname
  trace(DBG_NET, "NetObject: str=%s %s", str, pobject->getInstance());
}

/* Creates volatile NetObject */
NetObject::NetObject(WObject *po, uint8_t nprop)
{
  defaults();
  type = po->type;
  pobject = po;
  state = NET_VOLATILE;
  setPropertiesNumber(nprop);
  create(state);
  setNoid();
}

/* Creates replicated volatile NetObject */
NetObject::NetObject(WObject *po, uint8_t nprop, Noid _noid)
{
  defaults();
  type = po->type;
  pobject = po;
  state = NET_VOLATILE;
  noid = _noid;
  setPropertiesNumber(nprop);
}

list<NetObject*>::iterator NetObject::getList()
{
  return netobjectList.begin();
}

void NetObject::clearList()
{
  netobjectList.clear();
}

void NetObject::setMgrSsrc(uint32_t mgr_ssrc_id)
{
  myMgrSsrcId = mgr_ssrc_id;
}

uint32_t NetObject::getMgrSsrc()
{
  return myMgrSsrcId;
}

void NetObject::setSsrc(uint32_t ssrc_id)
{
  mySsrcId = ssrc_id;
}

// get current ssrc
uint32_t NetObject::getSsrc()
{
  return mySsrcId;
}

void NetObject::setHost(uint32_t host_id)
{
  myHostId = host_id;
}

uint32_t NetObject::getHost()
{
  return myHostId;
}

void NetObject::setPort(uint16_t _port_id)
{
  myPortId = _port_id;
}

uint16_t NetObject::getPort()
{
  return myPortId;
}

void NetObject::setObj(uint16_t _obj_id)
{
  myObjId = _obj_id;
}

uint16_t NetObject::getObj()
{
  return myObjId;
}

#if 0 //notused
void NetObject::setSrc(uint32_t _src_id)
{
  noid.src_id = _src_id;
}

uint32_t NetObject::getSrc() const
{
  return noid.src_id;
}

void NetObject::setPort(uint16_t _port_id)
{
  noid.port_id = _port_id;
}

uint16_t NetObject::getPort() const
{
  return noid.port_id;
}

void NetObject::setObj(uint16_t _obj_id)
{
  noid.obj_id = _obj_id;
}

uint16_t NetObject::getObj() const
{
  return noid.obj_id;
}
#endif //notused

/* Inserts netobject in head of netobjectList */
void NetObject::addToList()
{
  netobjectList.push_back(this);
}

void NetObject::initProperties(bool _responsible)
{
  if (netprop) return; //error("initProperties: netprop already exists (type=%d)", type);

  uint8_t np = NetProperty::getPropertiesNumber(type);
  if (!np) return;
  trace(DBG_NET, "initProperties: type=%d nobj=%s nprop=%d resp=%d", type, pobject->getInstance(), np, _responsible);

  netprop = new NetProperty[np];

  for (int i=0; i<np; i++) {
    NetProperty *pprop = netprop + i;
    pprop->responsible = _responsible;
    pprop->version = 0;
    pprop->resetDates();
  }
}

/* Returns the number of properties of this type */
uint8_t NetObject::getPropertiesNumber() const
{
  if (nbprop == 0) return NetProperty::getPropertiesNumber(type);
  return nbprop;
}

uint8_t NetObject::getPropertiesNumber(uint8_t _type_id)
{
  return NetProperty::getPropertiesNumber(_type_id);
}

/* Sets the number of properties of this type */
void NetObject::setPropertiesNumber(uint8_t _nbprop)
{
  nbprop = _nbprop;
  NetProperty::setPropertiesNumber(type, _nbprop);
}

void NetObject::setPropertiesNumber(uint8_t _type_id, uint8_t _nbprop)
{
  NetProperty::setPropertiesNumber(_type_id, _nbprop);
}

/* Assigns an unique identifier to each Vreng netobject */
void NetObject::setNoid()
{
  noid.src_id = mySsrcId;		// Application's identifier
  noid.port_id = myPortId;		// Comm port identifier
  noid.obj_id = htons(myObjId++);	// Application wide unique number
}

/* Creates a new netobject name */
void NetObject::create(bool _state)
{
  // MS.: Objects need a unique ID from the start,
  // not just for networked objects, so that the
  // Vjc controller apps can tell them apart.
  setNoid();

  state = _state;
  initProperties(true);	// new: then we are responsible
  addToList();
}

/* Builds a netobject name from the string "scene_id/obj_id" */
void NetObject::setNetName(const char *s, bool _state)
{
  uint16_t scene_id, obj_id;

  int c = sscanf(s, "%hu/%hu", &scene_id, &obj_id);

  if (c != 2 || scene_id == 0) {
    error("setNetName: invalid name %s c=%d scene=%d", s, c, scene_id);
    return;
  }

  state = _state;		// should be true
  initProperties(false);	// we are not responsible

  noid.src_id = htonl(1);
  noid.port_id = htons(scene_id);
  noid.obj_id = htons(obj_id);

  if (getNetObject()) {
    return;	//error("setNetName: %s already seen %d/%d", pobject->getInstance(), scene_id, obj_id);
  }
  addToList();	// add to list
}

/* Deletes a netobject from the list */
void NetObject::deleteFromList()
{
  if (! getNetObject()) {
    return;	//error("deleteFromList: already unnamed/deleted type=%d", type);
  }
  netobjectList.remove(this);
}

NetObject::~NetObject()
{
  deleteFromList();

  memset(&noid, 0, sizeof(noid));
  //if (netprop) delete[] netprop;
  netprop = NULL;
  del_netobject++;
}

bool NetObject::isPermanent() const
{
  return state;
}

bool NetObject::isResponsible() const
{
  return (netprop && netprop->responsible);
}

/* Finds a WObject pointer by its noid */
WObject * NetObject::getWObjectByNoid() const
{
  for (list<WObject*>::iterator it = mobileList.begin(); it != mobileList.end(); ++it) {
    if ((*it)->netop) {
      if (noid.equalNoid((*it)->netop->noid))
        return *it;	// found
    }
  }
  return NULL;
}

/* Gets the property prop_id (its local copy) got from Network */
void NetObject::getProperty(uint8_t prop_id, Payload *pp) const
{
  if (pobject) pobject->getProperty(prop_id, pp);
}

/* Puts the property (its local copy) to be sent to Network */
void NetObject::putProperty(uint8_t prop_id, Payload *pp)
{
  if (pobject) pobject->putProperty(prop_id, pp);
}

void NetObject::getAllProperties(Payload *pp) const
{
  uint8_t _nbprop = getPropertiesNumber(type);
  for (int p=0; p < _nbprop; p++) {
    getProperty(p, pp);
  }
}

/* Puts all properties of the netobject */
void NetObject::putAllProperties(Payload *pp)
{
  uint8_t _nbprop = getPropertiesNumber(type);

  for (int p=0; p < _nbprop; p++) {
    putProperty(p, pp);
  }
}

/* Removes netobject */
void NetObject::requestDeletionFromNetwork()
{
  if (pobject) pobject->deleteReplica();
}

/* Creates a replicated object */
NetObject * NetObject::replicateObject(uint8_t type_id, Noid noid, Payload *pp)
{
  WObject *po = OClass::replicatorInstance(type_id, noid, pp);  // factory

  if (po) {
    if (! po->netop) {
      error("replicateObject: no po->netop for type=%d", type_id); return NULL;
    }
    return po->netop;	// OK
  }
  return NULL;		// BAD
}

/* Send a multicast packet of type '0x02' = Delta */
void NetObject::sendDelta(uint8_t prop_id)
{
  if (! netprop) {
    error("sendDelta: netprop NULL");
    return;
  }

  NetProperty *pprop = netprop + prop_id;
  pprop->setResponsible(true);
  pprop->resetDates();

  Payload pp;
  pp.putPayload("cnch", VREP_DELTA, noid, prop_id, pprop->version);

  if (prop_id >= getPropertiesNumber(type)) {
    error("sendDelta: prop_id wrong");
    return;
  }
  putProperty(prop_id, &pp);

  Channel *pchan = Channel::current();
  if (pchan) {
    if (pchan->sa[SA_RTP] == 0) return;	// to is NULL
    pp.sendPayload(pchan->sa[SA_RTP]);
  }
}

/* Send a '0x01' packet to mentionned unicast address for the current object */
void NetObject::sendCreate(const struct sockaddr_in *to)
{
  Payload pp;

  pp.putPayload("ccnc", VREP_CREATE, type, noid, state);
  putAllProperties(&pp);

  uint8_t nprop = getPropertiesNumber(type);
  for (int i=0; i < nprop; i++) {
    pp.putPayload("h", netprop[i].version);
  }

  trace(DBG_NET, "sendCreate: nobj=%s to=%s", noid.getNetNameById(), inet4_ntop(&to->sin_addr));
  pp.sendPayload(to);
#if 0 //pddebug
  pp.dumpPayload(stderr);
#endif
}

/* Exported to WO, to call for each new object */
void NetObject::declareObjCreation()
{
  if (! getNetObject()) {
    error("declareObjCreation: unnamed netobject (type=%d)", type);
    return;
  }
  if (ntohl(noid.src_id) == 1) {
    error("declareObjCreation: not a new netobject (type=%d)", type);
    return;
  }
  Channel *pchan = Channel::current();
  if (pchan) sendCreate(pchan->sa[SA_RTP]);
}

/* Updates netobject version */
void NetObject::declareObjDelta(uint8_t prop_id)
{
  if (noid.src_id == 0) {
    noid.src_id = 1;	//localhost;
    //error("declareObjDelta: unnamed netobject type=%d prop=%d", type, prop_id);
    return;
  }
  uint16_t nprop = getPropertiesNumber(type);
  if (prop_id >= nprop) {
    error("declareObjDelta: invalid prop_id=%d > nprop=%d (type=%d)", prop_id, nprop, type);
    return;
  }
  NetProperty *pprop = netprop + prop_id;
  pprop->setResponsible(true);
  pprop->version += 1 + abs(rand() % VREP_VERS_JUMP); /* %10 */
  sendDelta(prop_id);
}

/* Destroy the netobject (its local copy) */
void NetObject::declareDeletion()
{
  if (! getNetObject()) return;

  if (state) {
    error("declareDeletion: on permanent object (type=%d)", type);
    return;
  }
  Channel *pchan = Channel::current();
  if (pchan) sendDelete(pchan->sa[SA_RTP]);
}

void NetObject::sendDelete(const struct sockaddr_in *to)
{
  Payload pp;

  pp.putPayload("cn", VREP_DELETE, noid);
  trace(DBG_NET, "sendDelete: nobj=%s to=%s", noid.getNetNameById(), inet4_ntop(&to->sin_addr));
  pp.sendPayload(to);
}

/* Builds and returns a concataned string name */
char * NetObject::getNetNameById()
{
  static char str[80];

  sprintf(str, "%x/%x/%x", ntohl(noid.src_id), ntohs(noid.port_id), ntohs(noid.obj_id));
  return str;
}

/* Returns 0 if different, other if equal */
bool NetObject::equalNoid(Noid n2) const
{
  return (
          noid.src_id == n2.src_id &&
          noid.port_id == n2.port_id &&
          noid.obj_id == n2.obj_id
         );
}

/* Gets a NetObject by name */
NetObject * NetObject::getNetObject()
{
  for (list<NetObject*>::iterator it = netobjectList.begin(); it != netobjectList.end(); ++it) {
    if ((*it)->equalNoid((*it)->noid)) {
      return *it;  // found
    }
    if (! OClass::isValidType((*it)->type)) {
      error("getNetObject: bad type=%d", (*it)->type);
      return NULL;
    }
  }
  return NULL;
}

/* Heuristic to avoid to send bunch of Queries */
int NetObject::filterQuery()
{
  static Noid oldnoid;
  static int cntdelta = 0;

  if (equalNoid(oldnoid)) {
    // last request was on this name
    if (++cntdelta <= 15) {
      // 15: 10 proprietes en moyenne, avec 15 on saute donc
      // sans doute un "bloc" de deltas, mais sans doute pas deux
      return 0;    // cancel this request
    }
  }
  // it's another one
  oldnoid = noid;
  cntdelta = 0;
  return 1;
}

/* Send a Query '0x03' packet  to the unicast sender */
void NetObject::sendQueryNoid(const struct sockaddr_in *to)
{
  if (! filterQuery()) return;

  Payload pp;
  pp.putPayload("cn", VREP_QUERY, noid);
  trace(DBG_NET, "sendQuery: nobj=%s to=%s", getNetNameById(), inet4_ntop(&to->sin_addr));
  pp.sendPayload(to);
}

/* Send a Delete '0x04' packet */
void NetObject::sendDeleteNoid(const struct sockaddr_in *to)
{
  Payload pp;

  pp.putPayload("cn", VREP_DELETE, noid);
  trace(DBG_NET, "sendDelete: nobj=%s to=%s", getNetNameById(), inet4_ntop( &to->sin_addr));
  pp.sendPayload(to);
}

#if 0 //notused
// get an netobject by SSRC
NetObject * getObjectBySSRC(uint32_t _ssrc)
{
  NetObject *pn = netobject_list;

  for ( ; pn && pn->noid.src_id != _ssrc; pn = pn->next)
	;
  return pn;
}
#endif //notused
