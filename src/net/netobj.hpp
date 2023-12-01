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
 * NetObj class
 */
class NetObj {
 friend class Payload;
 friend class Noid;

 private:

  NetObj *next;		///< next
  NetObj *prev;		///< prev

  // network ids
  static uint32_t mySsrcId;     ///< ssrc network format
  static uint32_t myHostId;     ///< addr_IP network format
  static uint16_t myPortId;     ///< port network format
  static uint16_t myObjId;      ///< new object'id (naming), fmt host
  static uint32_t myMgrSsrcId;  ///< manager ssrc network format

  void defaults();
  /**< Common to constructors */

  void setNoid();
  /**<
   * Initializes a new NetObj.
   * Assigns a unique identifier to each Vreng local netobj
   * whether if be a networked object or not.
   * Now we can do getObjId and declareDelta.
   * It is preferable (perfs) to do a declareCreation
   * when all the properties are initialized.
   */

  void addToList();
  /**< Inserts netobj into NetObj list */

  void setNetName(const char *str, bool netbehave);
  /**<
   * Build a NetObj name from a string "scene_id/obj_id", both uint16_t > 0
   * Used by getNetObj and declareDelta.
   * A declareCreation on such netobj produces a fatal.
   */

 public:

  enum {
    NET_VOLATILE,	// replica
    NET_PERMANENT	// local
  };

  class Noid noid;		///< NetObj id
  uint8_t type;			///< NetObj type
  uint8_t nbprop;		///< number of properties
  uint8_t state;		///< permanent or valatile (not a true bool)
  class WO *pobject;		///< pointer on the WO
  class NetProperty *netprop;	///< netobj properties

  NetObj();
  /**< Constructor for local */

  NetObj(WO *po, uint8_t nprop, uint16_t oid);
  /**< Constructor for local ppermanent with args */

  NetObj(WO *po, uint8_t nprop);
  /**< Constructor for local volatile with args */

  NetObj(WO *po, uint8_t nprop, class Noid _noid);
  /**< Constructor for replica */

  virtual ~NetObj();
  /**< Destructor */

  void deleteFromList();
  /**<
   * Removes the NetObj from the list.
   * To do necessarly before the final delete.
   */

 public:
  static std::list<NetObj*> netobjList;
  /**< netobj list. */

  //
  // exports to WO
  //
  void set(bool netbehave);
  /**<
   * Creates a new local netobj.
   * Then we can do getNetObj, declareDelta.
   * One declareCreation is wish latter, when props are set.
   */

  void declareCreation();
  /**<
   * We assume the header yet initialized,
   * should (perfs) be called after the NetObj naming create()
   * and the initialization of properties.
   * To call for each new objects.
   */

  void declareDelta(uint8_t prop_id);
  /**<
   * Update netobj version.
   * To call at each modification, eg. after a property value changes.
   */

  void declareDeletion();
  /**<
   * Destroy the netobj (local copy), netobj must be valid (name).
   * To call when we want destroy the object before a deleteNetObj.
   */

  // static methods for static members
  static void setSsrc(uint32_t ssrc_id);
  static void setMgrSsrc(uint32_t ssrc_id);
  static void setHost(uint32_t host_id);
  static void setPort(uint16_t port_id);
  static void setObj(uint16_t obj_id);
  static uint32_t getSsrc();
  static uint32_t getMgrSsrc();
  static uint32_t getHost();
  static uint16_t getPort();
  static uint16_t getObj();

  // Send
  void sendCreate(const struct sockaddr_in *to);
  /**<
   * Send a '0x01' packet to mentionned unicast address for the current object
   * we don't touch version and dates
   * Format: '0x01' (c), netobj type (c), netobj name (n), netbehave (c)
   *         then the initialization payload, obtained by a putAllProperties()
   *         then the version vector (nprop *pn).
   */

 public:
  void sendDelta(uint8_t prop_id);
  /**<
   * Send a multicast packet of type '0x02' = Delta,
   * on the property prop_id of the netobj.
   * We do a resetDates (not version).
   * Format: '0x02' (c), netobj name (n), property number (c), version (h),
   *         netobj's properties.
   */

  void sendDelete(const struct sockaddr_in *to);
  /**<
   * Send a Delete '0x04' packet to the unicast sender.
   * Format: '0x04' (c), netobj name (n).
   */

  void initProperties(bool responsible);
  /**<
   * Initializes responsibilities (false=no, true=yes).
   * Dates handled by resetDates.
   */

  uint8_t getProperties() const;

  static uint8_t getProperties(uint8_t type_id);
  /**<
   * Returns le number of properties for this type.
   * numbered from 0 to propertiesnumber-1.
   * Typically called after an incomingCreate.
   */

  void setProperties(uint8_t _nbprop);

  static void setProperties(uint8_t type_id, uint8_t _nbprop);
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
   * Puts all properties of this netobj.
   * The payload is initialized before, and filled here.
   * Called to known the Payload after one declareCreation.
   */

  void requestDeletionFromNetwork();
  /**<
   * Supprime object du monde, si object n'est pas le local user
   * The sequence must include deleteNetObj.
   *  1) faire le menage et afficher tout ce qui est necessaire.
   *  2) si la decision a ete prise localement: declareDeletion
   *  3) deleteNetObj
   *     le nom devient invalide, plus aucun declare n'est possible
   *  4) faire le delete object final
   */

  WO * getWOByNoid() const;
  /**< Finds a WO pointer by its noid */

  bool isResponsible() const;

  bool isPermanent() const;

  static std::list<NetObj*>::iterator getList();
  /**< Gets the NetObj list. */

  static void clearList();
  /**< Clears the NetObj list */

  static NetObj *replicateObject(uint8_t type, class Noid noid, class Payload *pp);
  /**<
   * Dispatching the replicated object.
   * Creates a replication (local copy) of the object.
   * All its fields will be intilisalized by the WO itself
   * using appropriated getProperty().
   * Typically called after an incomingCreate().
   */

  NetObj * getNetObj();
  /**<
   * Gets a NetObj by name,
   * returns NULL if netobj doesn't exist.
   * Header vers noid se fait en lisant directement dans la struct
   * Naming is done by create or createNetObj.
   */

  char * getNetNameById();
  /**< Builds a concatened string name */

  void sendQueryNoid(const struct sockaddr_in *sender);
  /**<
   * Send a Query '0x03' packet toi the unicast sender.
   * We try to avoid to send too many.
   * Format: '0x03' (c), netobj name (n).
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
