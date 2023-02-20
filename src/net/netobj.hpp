//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
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
#ifndef NETOBJ_HPP
#define NETOBJ_HPP

#include "noid.hpp"

#include <list>
using namespace std;


/**
 * NetObject class
 */
class NetObject {
 friend class Payload;
 friend class Noid;

 private:
  // network ids
  static uint32_t mySsrcId;     ///< ssrc network format
  static uint32_t myHostId;     ///< addr_IP network format
  static uint16_t myPortId;     ///< port network format
  static uint16_t myObjId;      ///< new object'id (naming), fmt host
  static uint32_t myMgrSsrcId;  ///< manager ssrc network format

 public:
  class Noid noid;		///< NetObject id
  uint8_t type;			///< NetObject type
  uint8_t permanent;		///< permanent or valatile (not a true bool)
  uint8_t nbprop;		///< number of properties
  class WObject *pobject;	///< pointer on the WObject
  class NetProperty *netprop;	///< netobject properties
  NetObject *next;		///< next
  NetObject *prev;		///< prev

  enum {
    NET_VOLATILE, // replicatable
    NET_PERMANENT
  };

  NetObject();
  /**< Constructor for local */

  NetObject(WObject *po, uint8_t nprop, uint16_t oid);
  /**< Constructor for local ppermanent with args */

  NetObject(WObject *po, uint8_t nprop);
  /**< Constructor for local volatile with args */

  NetObject(WObject *po, uint8_t nprop, class Noid _noid);
  /**< Constructor for replica */

  virtual ~NetObject();
  /**< Destructor */

  void defaults();
  /**< Common to constructors */

  void addToList();
  /**< Inserts netobject in head of NetObject list */

  void deleteFromList();
  /**<
   * Removes the NetObject from the list.
   * To do necessarly before the final delete.
   */

  //
  // exports to WO
  //
  void create(bool netbehave);
  /**<
   * Creates a new local netobject.
   * Then we can do getNetObject, declareObjDelta.
   * One declareObjCreation is wish latter, when props are set.
   */

  void setNoid();
  /**<
   * Initializes a new NetObject.
   * Assigns a unique identifier to each Vreng local netobject
   * whether if be a networked object or not.
   * Now we can do getObjId and declareObjDelta.
   * It is preferable (perfs) to do a declareObjCreation
   * when all the properties are initialized.
   */

  void setNetName(const char *str, bool netbehave);
  /**<
   * Build a NetObject name from a string "scene_id/obj_id", both uint16_t > 0
   * Used by getNetObject and declareObjDelta.
   * A declareObjCreation on such netobject produces a fatal.
   */

  void declareObjCreation();
  /**<
   * We assume the header yet initialized,
   * should (perfs) be called after the NetObject naming create()
   * and the initialization of properties.
   * To call for each new objects.
   */

  void declareObjDelta(uint8_t prop_id);
  /**<
   * Update netobject version.
   * To call at each modification, eg. after a property value changes.
   */

  void declareDeletion();
  /**<
   * Destroy the netobject (local copy), netobject must be valid (name).
   * To call when we want destroy the object before a deleteNetObject.
   */

  // Accessors
  void setSrc(uint32_t src_id);
  void setPort(uint16_t port_id);
  void setObj(uint16_t obj_id);
  uint32_t getSrc() const;
  uint16_t getPort() const;
  uint16_t getObj() const;

  static void setMySsrc(uint32_t ssrc_id);
  static void setMgrSsrc(uint32_t ssrc_id);
  static void setMyHost(uint32_t host_id);
  static void setMyPort(uint16_t port_id);
  static void setMyObj(uint16_t obj_id);
  static uint32_t getMySsrc();
  static uint32_t getMyMgrSsrc();
  static uint32_t getMyHost();
  static uint16_t getMyPort();
  static uint16_t getMyObj();

  // Send
  void sendCreate(const struct sockaddr_in *to);
  /**<
   * Send a '0x01' packet to mentionned unicast address for the current object
   * we don't touch version and dates
   * Format: '0x01' (c), netobject type (c), netobject name (n), netbehave (c)
   *         then the initialization payload, obtained by a putAllProperties()
   *         then the version vector (nprop *pn).
   */

  void sendDelta(uint8_t prop_id);
  /**<
   * Send a multicast packet of type '0x02' = Delta,
   * on the property prop_id of the netobject.
   * We do a resetDates (not version).
   * Format: '0x02' (c), netobject name (n), property number (c), version (h),
   *         netobject's properties.
   */

  void sendDelete(const struct sockaddr_in *to);
  /**<
   * Send a Delete '0x04' packet to the unicast sender.
   * Format: '0x04' (c), netobject name (n).
   */

  void initProperties(bool responsible);
  /**<
   * Initializes responsibilities (false=no, true=yes).
   * Dates handled by resetDates.
   */

  uint8_t getPropertiesNumber() const;

  static uint8_t getPropertiesNumber(uint8_t type_id);
  /**<
   * Returns le number of properties for this type.
   * Numbered from 0 to propertiesnumber-1.
   * Typically called after an incomingCreate.
   */

  void setPropertiesNumber(uint8_t _nbprop);

  static void setPropertiesNumber(uint8_t type_id, uint8_t _nbprop);
  /**<
   * Sets the number of properties for this type
   */

  void getProperty(uint8_t prop, class Payload *pp) const;
  /**<
   * Modifies the property (its local copy) get from Network.
   * The new value is in the payload.
   * We must return the new read position given by getPayload.
   * Typically called after an incomingDelta.
   */

  void putProperty(uint8_t prop, class Payload *pp);
  /**<
   * Puts the property (its local copy) to be sent to Network.
   * Then we must write the payload which return the written length.
   * Typically called to fill a payload before a sendDelta.
   */

  void getAllProperties(class Payload *pp) const;
  /**<
   * Gets all properties from the network.
   */

  void putAllProperties(class Payload *pp);
  /**<
   * Puts all properties of this netobject.
   * The payload is initialized before, and filled here.
   * Called to known the Payload after one declareObjCreation.
   */

  void requestDeletionFromNetwork();
  /**<
   * Supprime object du monde, si object n'est pas le local user
   * The sequence must include deleteNetObject.
   *  1) faire le menage et afficher tout ce qui est necessaire.
   *  2) si la decision a ete prise localement: declareDeletion
   *  3) deleteNetObject
   *     le nom devient invalide, plus aucun declare n'est possible
   *  4) faire le delete object final
   */

  WObject * getWObjectByNoid() const;
  /**< Finds a WObject pointer by its noid */

  bool isResponsible() const;

  bool isPermanent() const;

  static std::list<NetObject*> netobjectList;
  /**< netobject list. */

  static std::list<NetObject*>::iterator getList();
  /**< Gets the NetObject list. */

  static void clearList();
  /**< Clears th NetObject list */

  static NetObject *replicateObject(uint8_t type, class Noid noid, class Payload *pp);
  /**<
   * Dispatching the replicated object.
   * Creates a replication (local copy) of the object.
   * All its fields will be intilisalized by the WObject itself
   * using appropriated getProperty().
   * Typically called after an incomingCreate().
   */

  NetObject * getNetObject();
  /**<
   * Gets a NetObject by name,
   * returns NULL if netobject doesn't exist.
   * Header vers noid se fait en lisant directement dans la struct
   * Naming is done by create or createNetObjectFromString.
   */

  char * getNetNameById();
  /**< Builds a concatened string name */

  void sendQueryNoid(const struct sockaddr_in *sender);
  /**<
   * Send a Query '0x03' packet toi the unicast sender.
   * We try to avoid to send too many.
   * Format: '0x03' (c), netobject name (n).
   * Called by incomingDelta.
   */

  void sendDeleteNoid(const struct sockaddr_in *sender);
  /**<
   * Send a Delete '0x04' packet toi the unicast sender.
   */

 protected:
  bool equalNoid(class Noid noid2) const;
  /**< Returns 0 if differents, other if equals */

  int filterQuery();
  /**< Heuristic to avoid to send bunch of Query */

};

#endif
