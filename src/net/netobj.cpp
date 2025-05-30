//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// netobj.cpp
//
// NetObj handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "netobj.hpp"
#include "vrep.hpp"	// VREP
#include "nsl.hpp"	// inet4_ntop
#include "payload.hpp"	// Payload
#include "netprop.hpp"	// NetProp
#include "channel.hpp"	// Channel
#include "oclass.hpp"	// isValidType
#include "object.hpp"	// Object
#include "stat.hpp"	// new_netobj


// local
std::list<NetObj*> NetObj::netobjList;

// network ids
uint32_t NetObj::mySsrcId = 0;   // ssrc network format
uint32_t NetObj::myHostId = 0;   // addr_IP network format
uint16_t NetObj::myPortId = 0;   // port network format
uint16_t NetObj::myObjId = 0;    // new object'id (naming), fmt host
uint32_t NetObj::myMgrSsrcId = 0; // manager ssrc network format


/*
 * Handling net id names and properties
 */

/** Sets default values */
void NetObj::defaults()
{
  new_netobj++;
  nbprop = 0;
  prop = NULL;
}

/** Creates local generic NetObj */
NetObj::NetObj()
{
  defaults();
  type = 0;
  object = NULL;
  state = NET_PERMANENT;
  setNoid();
}

/** Creates local permanent NetObj */
NetObj::NetObj(Object *po, uint8_t nprop, uint16_t oid)
{
  defaults();
  type = po->typeId();
  object = po;
  state = NET_PERMANENT;
  setProperties(nprop);

  char str[80];
  sprintf(str, "%d/%d", type, oid);
  setNetName(str, state);     // net objname
  trace1(DBG_NET, "NetObj: str=%s %s", str, object->objectName());
}

/** Creates volatile NetObj */
NetObj::NetObj(Object *po, uint8_t nprop)
{
  defaults();
  type = po->type;
  object = po;
  state = NET_VOLATILE;
  setProperties(nprop);
  set(state);
  setNoid();
}

/** Creates replicated volatile NetObj */
NetObj::NetObj(Object *po, uint8_t nprop, Noid _noid)
{
  defaults();
  type = po->type;
  object = po;
  state = NET_VOLATILE;
  noid = _noid;
  setProperties(nprop);
}

/** Gets begin List */
std::list<NetObj*>::iterator NetObj::getList()
{
  return netobjList.begin();
}

/** Sets ssrc manager */
void NetObj::setMgrSsrc(uint32_t mgr_ssrc_id)
{
  myMgrSsrcId = mgr_ssrc_id;
}

/** Sets ssrc */
void NetObj::setSsrc(uint32_t ssrc_id)
{
  mySsrcId = ssrc_id;
}

/** Gets current ssrc */
uint32_t NetObj::getSsrc()
{
  return mySsrcId;
}

/** Sets host */
void NetObj::setHost(uint32_t host_id)
{
  myHostId = host_id;
}

/** Gets current host */
uint32_t NetObj::getHost()
{
  return myHostId;
}

/** Sets port */
void NetObj::setPort(uint16_t _port_id)
{
  myPortId = _port_id;
}

/** Gets port */
uint16_t NetObj::getPort()
{
  return myPortId;
}

/** Sets Obj */
void NetObj::setObj(uint16_t _obj_id)
{
  myObjId = _obj_id;
}

/** Gets Obj */
uint16_t NetObj::getObj()
{
  return myObjId;
}

/** Inserts netobj into netobjList */
void NetObj::addToList()
{
  netobjList.push_back(this);
}

void NetObj::initProperties(bool _responsible)
{
  if (prop) return; //error("initProperties: prop already exists (type=%d)", type);

  uint8_t np = NetProp::getProperties(type);
  if (!np) return;
  trace1(DBG_NET, "initProperties: type=%d nobj=%s nprop=%d resp=%d", type, object->objectName(), np, _responsible);
  prop = new NetProp[np];
  for (int i=0; i<np; i++) {
    NetProp *pprop = prop + i;
    pprop->responsible = _responsible;
    pprop->version = 0;
    pprop->newDate();
  }
}

/** Returns the number of properties of this type */
uint8_t NetObj::getProperties() const
{
  if (nbprop == 0) return NetProp::getProperties(type);
  return nbprop;
}

/** Returns the number of properties of this type */
uint8_t NetObj::getProperties(uint8_t _type_id)
{
  return NetProp::getProperties(_type_id);
}

/** Sets the number of properties of this type */
void NetObj::setProperties(uint8_t _nbprop)
{
  nbprop = _nbprop;
  NetProp::setProperties(type, _nbprop);
}

/** Sets the number of properties of this type */
void NetObj::setProperties(uint8_t _type_id, uint8_t _nbprop)
{
  NetProp::setProperties(_type_id, _nbprop);
}

/** Sets an unique identifier to each Vreng netobj */
void NetObj::setNoid()
{
  noid.src_id = mySsrcId;		// Application's identifier
  noid.port_id = myPortId;		// Comm port identifier
  noid.obj_id = htons(myObjId++);	// Application wide unique number
}

/** Creates a new netobj name */
void NetObj::set(bool _state)
{
  // MS.: Objects need a unique ID from the start,
  // not just for networked objects, so that the
  // Vjc controller apps can tell them apart.
  state = _state;
  setNoid();
  initProperties(true);			// new: then we are responsible
  netobjList.push_back(this);		// add to list
}

/** Builds a netobj name from the string "scene_id/obj_id" */
void NetObj::setNetName(const char *s, bool _state)
{
  uint16_t scene_id, obj_id;

  int c = sscanf(s, "%hu/%hu", &scene_id, &obj_id);
  if (c != 2 || scene_id == 0) {
    error("setNetName: invalid name %s c=%d scene=%d", s, c, scene_id);
    return;
  }
  state = _state;			// should be true
  initProperties(false);		// we are not responsible
  noid.src_id = htonl(1);
  noid.port_id = htons(scene_id);
  noid.obj_id = htons(obj_id);

  if (getNetObj()) {
    return;	//error("%s already seen %d/%d", object->objectName(), scene_id, obj_id);
  }
  netobjList.push_back(this);		// add to list
}

/** Deletes a netobj from the list */
void NetObj::deleteFromList()
{
  if (! getNetObj()) {
    return;	//error("already unnamed/deleted type=%d", type);
  }
  netobjList.remove(this);
}

/** Destructor */
NetObj::~NetObj()
{
  netobjList.remove(this);

  memset(&noid, 0, sizeof(noid));
  //if (prop) delete[] prop;
  prop = NULL;
  del_netobj++;
}

/** Is permanent */
bool NetObj::isPermanent() const
{
  return state;
}

/** Is responsible */
bool NetObj::isResponsible() const
{
  return (prop && prop->responsible);
}

/** Gets the property prop_id (its local copy) got from Network */
void NetObj::getProperty(uint8_t prop_id, Payload *pp) const
{
  if (object) object->getProperty(prop_id, pp);
}

/** Puts the property (its local copy) to be sent to Network */
void NetObj::putProperty(uint8_t prop_id, Payload *pp)
{
  if (object) object->putProperty(prop_id, pp);
}

void NetObj::getAllProperties(Payload *pp) const
{
  uint8_t nbp = getProperties(type);
  for (int p=0; p < nbp; p++) {
    getProperty(p, pp);
  }
}

/** Puts all properties of the netobj */
void NetObj::putAllProperties(Payload *pp)
{
  uint8_t nbp = getProperties(type);
  for (int p=0; p < nbp; p++) {
    putProperty(p, pp);
  }
}

/** Removes netobj */
void NetObj::requestDeletion()
{
  if (object) object->deleteReplica();
}

/** Creates a replicated object */
NetObj * NetObj::replicate(uint8_t type_id, Noid noid, Payload *pp)
{
  Object *o = OClass::replicatorInstance(type_id, noid, pp);  // factory
  if (o) {
    if (! o->netop) {
      error("replicate: no o->netop for type=%d", type_id);
      return NULL;
    }
    return o->netop;	// OK
  }
  return NULL;		// BAD
}

/** Sends a multicast packet of type '0x02' = Delta */
void NetObj::sendDelta(uint8_t prop_id)
{
  if (! prop) {
    error("sendDelta: prop NULL");
    return;
  }
  NetProp *pprop = prop + prop_id;
  pprop->setResponsible(true);
  pprop->newDate();

  Payload pp;
  pp.putPayload("cnch", VREP_DELTA, noid, prop_id, pprop->version);

  if (prop_id >= getProperties(type)) {
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

/** Sends a '0x01' packet to mentionned unicast address for the current object */
void NetObj::sendCreate(const struct sockaddr_in *to)
{
  Payload pp;

  pp.putPayload("ccnc", VREP_CREATE, type, noid, state);
  putAllProperties(&pp);

  uint8_t nprop = getProperties(type);
  for (int i=0; i < nprop; i++) {
    pp.putPayload("h", prop[i].version);
  }
  trace1(DBG_NET, "sendCreate: nobj=%s to=%s", noid.getNoid(), inet4_ntop(&to->sin_addr));
  pp.sendPayload(to);
#if 0 //pddebug
  pp.dumpPayload(stderr);
#endif
}

/** Declares a creation - exported to Object, to call for each new object */
void NetObj::declareCreation()
{
  if (! getNetObj()) {
    error("declareCreation: unnamed netobj (type=%d)", type);
    return;
  }
  if (ntohl(noid.src_id) == 1) {
    error("declareCreation: not a new netobj (type=%d)", type);
    return;
  }
  Channel *pchan = Channel::current();
  if (pchan) sendCreate(pchan->sa[SA_RTP]);
}

/** Updates netobj version */
void NetObj::declareDelta(uint8_t prop_id)
{
  if (noid.src_id == 0) {
    noid.src_id = 1;	//localhost;
    //error("declareDelta: unnamed netobj type=%d prop=%d", type, prop_id);
    return;
  }
  uint16_t nprop = getProperties(type);
  if (prop_id >= nprop) {
    error("declareDelta: invalid prop_id=%d > nprop=%d (type=%d)", prop_id, nprop, type);
    return;
  }
  NetProp *pprop = prop + prop_id;
  pprop->setResponsible(true);
  pprop->version += 1 + abs(rand() % VREP_VERS_JUMP); /* %10 */
  sendDelta(prop_id);
}

/** Destroys the netobj (its local copy) */
void NetObj::declareDeletion()
{
  if (! getNetObj()) return;

  if (state) {
    error("declareDeletion: on permanent object (type=%d)", type);
    return;
  }
  Channel *pchan = Channel::current();
  if (pchan) sendDelete(pchan->sa[SA_RTP]);
}

/** Sends a Delete '0x04' packet */
void NetObj::sendDelete(const struct sockaddr_in *to)
{
  Payload pp;

  pp.putPayload("cn", VREP_DELETE, noid);
  trace1(DBG_NET, "sendDelete: nobj=%s to=%s", noid.getNoid(), inet4_ntop(&to->sin_addr));
  pp.sendPayload(to);
}

/** Gets a NetObj by name */
NetObj * NetObj::getNetObj()
{
  for (std::list<NetObj*>::iterator it = netobjList.begin(); it != netobjList.end(); ++it) {
    if ((*it)->noid.equal((*it)->noid)) {
      return *it;	// NetObj found
    }
    if (! OClass::isValidType((*it)->type)) {
      error("getNetObj: bad type=%d", (*it)->type);
      return NULL;
    }
  }
  return NULL;
}
