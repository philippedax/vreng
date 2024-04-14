//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
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
#ifndef PAYLOAD_HPP
#define PAYLOAD_HPP


#define PAYLOAD_LEN 1460

/**
 * Payload class
 */
class Payload {

 private:
  uint16_t idx;			///< current offset
  uint8_t vrep;			///< vrep version
  bool gzipped;			///< if compression
  uint8_t data[PAYLOAD_LEN];	///< data

  bool isValid();
  /**< Checks payload validity */

  Payload * resetPayload();
  /**<
   * Resets the payload, sets offset at the begining of the payload
   * to call before using
   */

  void dump(FILE *fp);
  /**< Dumps payload in a file pointed by fp */

 public:
  uint16_t len;			///< length

  Payload();		/**< Constructor */
  virtual ~Payload();	/**< Destructor */

  int putPayload(const char *format,...);
  /**<
   * Fills the payload in sprintf style
   * Returns 0 if OK, else -1 if err
   */

  int getPayload(const char *format,...);
  /**<
   * Gets the payload in scanf style
   * Returns 0 if OK, else -1 if err
   */

  void seekPayload(const uint16_t idx);
  /**< Seeks pos inside the payload */

  uint16_t tellPayload();
  /**< Returns current offset inside the payload */

  int tellPayload(const char *str);
  /**< Returns current offset of string "str" inside the payload */

  //
  // Packets
  //
  int sendPayload(const struct sockaddr_in *to);
  /**< Sends a payload packet to the network */

  int recvPayload(const int fd, struct sockaddr_in *sender);
  /**< Receives a payload packet from the network */

  //
  // Incomings
  //

  void incomingDelta(const struct sockaddr_in *sender);
  /**<
   * Incoming Delta:
   *   format (3 bytes)
   *   noid (8 bytes: 4 + 2 + 2)
   *   prop_id (1 byte)
   *   version_id (2 bytes)
   *   total (14 bytes)
   */

  void incomingCreate(const struct sockaddr_in *sender);
  /**<
   * Incoming Create:
   *   format (3 bytes)
   *   type_id (1 byte)
   *   noid (8 bytes: 4 + 2 + 2)
   *   permanent_flag (1 byte)
   *   properties (nprop * 20 bytes: 2 + 2 + 8 + 8)
   * Create the object's local copy if it doesn't exist
   */

  void incomingQuery(const struct sockaddr_in *sender);
  /**<
   * Incoming Query:
   *   format (1 byte)
   *   noid (8 bytes: 4 + 2 + 2)
   *   total (9 bytes)
   */

  void incomingDelete(const struct sockaddr_in *sender);
  /**<
   * Incoming Delete:
   *   format (1 byte)
   *   noid (8 bytes: 4 + 2 + 2)
   *   total (9 bytes)
   */

  void incomingUnknown(const struct sockaddr_in *sender, const int len);
  /**<
   * Incoming not identified
   */
};

#endif
