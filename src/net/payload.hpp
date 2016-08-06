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
#ifndef PAYLOAD_HPP
#define PAYLOAD_HPP

#if 1
#define PAYLOAD_LEN 1460
#else
#define PAYLOAD_LEN 524	// max size of datas (last=496, orig=237)
#endif

/**
 * Payload class
 */
class Payload {
 public:

  uint16_t len;			///< length
  uint16_t idx;			///< current offset
  uint8_t vrep;			///< vrep version
  bool gzipped;			///< if compression
  uint8_t data[PAYLOAD_LEN];	///< data

  Payload();
  /**< Constructor */

  virtual ~Payload();
  /**< Destructor */

  virtual int putPayload(const char *format,...);
  /**<
   * Fills the payload in sprintf style
   * Returns 0 if OK, else -1 if err
   */

  virtual int getPayload(const char *format,...);
  /**<
   * Gets the payload in scanf style
   * Returns 0 if OK, else -1 if err
   */

  virtual bool isValidPayload();
  /**< Checks payload validity */

  virtual Payload * resetPayload();
  /**<
   * Resets the payload, sets offset at the begining of the payload
   * to call before using
   */

  virtual void dumpPayload(FILE *fp);
  /**< Dumps payload in a file pointed by fp */

  virtual void seekPayload(const uint16_t idx);
  /**< Seeks pos inside the payload */

  virtual uint16_t tellPayload();
  /**< Returns current offset inside the payload */

  virtual int tellStrInPayload(const char *str);
  /**< Returns current offset of string "str" inside the payload */

  //
  // Packets
  //
  virtual int sendPayload(const struct sockaddr_in *to);
  /**< Sends a payload packet to the network */

  virtual int recvPayload(const int fd, struct sockaddr_in *sender);
  /**< Receives a payload packet from the network */

  //
  // Incomings
  //
  virtual void incomingDelta(const struct sockaddr_in *sender);
  /**<
   * Incoming Delta:
   *   format (3 bytes)
   *   noid (8 bytes: 4 + 2 + 2)
   *   prop_id (1 byte)
   *   version_id (2 bytes)
   *   total (14 bytes)
   */

  virtual void incomingCreate(const struct sockaddr_in *sender);
  /**<
   * Incoming Create:
   *   format (3 bytes)
   *   type_id (1 byte)
   *   noid (8 bytes: 4 + 2 + 2)
   *   permanent_flag (1 byte)
   *   properties (nprop * 20 bytes: 2 + 2 + 8 + 8)
   * Create the object's local copy if it doesn't exist
   */

  virtual void incomingQuery(const struct sockaddr_in *sender);
  /**<
   * Incoming Query:
   *   format (1 byte)
   *   noid (8 bytes: 4 + 2 + 2)
   *   total (9 bytes)
   */

  virtual void incomingDelete(const struct sockaddr_in *sender);
  /**<
   * Incoming Delete:
   *   format (1 byte)
   *   noid (8 bytes: 4 + 2 + 2)
   *   total (9 bytes)
   */

  virtual void incomingOther(const struct sockaddr_in *sender, const int len);
  /**<
   * Incoming not identified
   */

#if 0 //notused
  virtual void incomingPersist(const struct sockaddr_in *sender);
  virtual void incomingWorld(const struct sockaddr_in *sender);
#endif //notused

};

#endif
