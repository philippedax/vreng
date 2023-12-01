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
#ifndef NOID_HPP
#define NOID_HPP


/**
 * Noid class
 *
 * A Noid identifies a distributed netobject on 64 bits
 * (src_id + port_id + obj_id)
 */
class Noid {
 friend class NetObj;

 public:
  uint32_t src_id;	///< IPaddr src
  uint16_t port_id;	///< port
  uint16_t obj_id;	///< object id -- network format

  class NetObj * getNetObj();
  /**<
   * Gets a NetObj by name,
   * returns NULL if netobject doesn't exist.
   * Header vers noid se fait en lisant directement dans la struct
   * Naming is done by create or createNetObjFromString.
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

  bool equalNoid(Noid noid2) const;
  /**< Returns 0 if differents, other if equals */

  int filterQuery();
  /**< Heuristic to avoid to send bunch of Query */
};

#endif
