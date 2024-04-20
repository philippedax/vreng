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
// vjc.hpp
//---------------------------------------------------------------------------
#ifndef VJC_HPP
#define VJC_HPP

#define VJC_TYPE	37
#define VJC_NAME	"Vjc"

#define DEF_URL_JAR	"/jar/vrengapp.jar"	// jar vjs server

#include "wobject.hpp"

//****************************************** begin of unviolable code
// DON'T MODIFY THE FOLLOWING CONSTANTS !!!
// They are needed by src/vjs/gen.pl
// to build src/vjs/gen/fr/enst/vreng/MessageTypes.java
// They are used in:
//	vjs/base/fr/enst/vreng/net/Receiver.java
//	vjs/base/fr/enst/vreng/server/Server.java
//	vjs/base/fr/enst/vreng/server/VrengApp.java
//	vjs/base/fr/enst/vreng/data/messages/ClickMessage.java
//	vjs/base/fr/enst/vreng/data/messages/ControlMessage.java
//	vjs/base/fr/enst/vreng/data/messages/DrawMessage.java
//	vjs/base/fr/enst/vreng/data/messages/PosMessage.java
//	vjs/base/fr/enst/vreng/data/messages/QueryMessage.java

// Message types
#define VJC_MSGT_CTRL		0
#define VJC_MSGT_CLICK		1
#define VJC_MSGT_ADD		2
#define VJC_MSGT_DEL		3
#define VJC_MSGT_POS		4
#define VJC_MSGT_ISEC		5
#define VJC_MSGT_QUERY		6
#define VJC_MSG_TYPES_STRINGS	Control Click Add Delete Position Intersect Query

// Message val

// control
#define VJC_MSGV_REGISTER	0
#define VJC_MSGV_UNREGISTER	1
#define VJC_MSGV_INITIATE	2
#define VJC_MSGV_TERMINATE	3
#define VJC_MSGV_UPDATE		4
#define VJC_MSGV_PING		5
#define VJC_MSG_CTRL_STRINGS	Register Unregister Inititate Terminate Update Ping

// position
#define VJC_MSGV_ASK		0
#define VJC_MSGV_SET		1
#define VJC_MSGV_UPD		2
#define VJC_MSGV_ERROR		3
#define VJC_MSG_POS_STRINGS	Ask Set Update Error

// query
#define VJC_MSGV_QTYPE		0
#define VJC_MSGV_QANS		1
#define VJC_MSG_QUERY_STRINGS	TypeQuery QueryAnswer
//****************************************** end of unviolable code

// draw  add / del
#define VJC_MSGV_CIRCLE		0
#define VJC_MSGV_LINE		1
#define VJC_MSGV_LOOP		2
#define VJC_MSGV_FILL		3
#define VJC_MSGV_DELALL		4
#define VJC_MSG_DRAW_STRINGS	Circle Line Loop Fill DeleteAll

// click
#define VJC_MSGV_CLICK		0
#define VJC_MSG_CLICK_STRINGS	Click

// intersection
#define VJC_MSGV_ISECIN		0
#define VJC_MSGV_ISECOUT	1
#define VJC_MSG_ISEC_STRINGS	GoingIn GoingOut


#include "vrelet.hpp"

/**
 * VjcSocket class
 * in-out socket holder
 */
class VjcSocket {

 public:
  uint16_t listenPort;	///< Local port
  uint16_t destPort;	///< Server port
  int statecon;		///< Connection state
  int sdr;		///< read fd
  int sdw;		///< write fd
  struct sockaddr_in *sadest;	///< socket for Vjs

  // socket constants
  enum {
    SOCK_CLOSED,
    SOCK_RCVOPEN,
    SOCK_CONNECT,
    SOCK_OPEN
  };

  VjcSocket(uint16_t listenPort, const char *destHost, uint16_t destPort);
  /**<
   * Constructor :
   * Initializes a pair of sockets for communicating with the child app.
   * The sockets aren't actually opened. The addresses are initialized.
   */

  virtual ~VjcSocket();
  /**<
   * Closes and frees a VjcSocket.
   * The handle is invalidated by this call.
   */

  int openSocket();
  /**<
   * Opens a VjcSocket. This call is 'non-blocking' : if the connect
   * does not work because the client is not ready yet, this function
   * returns 0.
   * In this case, it should be called again to finish the connection.
   * If the socket is completly open, it returns 1.
   * If an error occured, it returns -1.
   */

  int openRecv();
  /**<
   * Opens the receiver socket for this socket pair.
   * The socket is bound to the local port passed in when it was initialized.
   */

  int openSend();
  /**<
   * Opens the sender socket. IO is set to non-blocking on this one
   * so that the connect (done in the next step) does not lock up
   * the whole VRENG app.
   */

  int connectSend();
  /**<
   * Tries a connect to the client app. Since the IO mode is NONBLOCK,
   * two types of errors can happen:
   * - EINPROGRESS : the connect didn't complet - the child app is not ready,
   * but is not dead either. The connect will have to be tried again.
   * - Other : the child app is not in a good state. Not worth retrying.
   */

  bool isConnected();
  /**<
   * Checks if the non-blocking connect call on the send socket
   * finished or not.
   */
};

/**
 * Packet header.
 * WARNING !!! If you update this structure, do not forget to update the
 * VJC_HDR_LEN, VJC_HDR_SSRC_IDX and VJC_HDR_DATALEN_IDX.
 * VJC_HDR_LEN is the total length (in 8bit ints, no padding)
 * of this structure, and VJC_HDR_DATALEN_IDX is the position
 * of the 16bit data length field within the structure.
 * VJC_HDR_SSRC_IDX is the same, but to the 32bit app_ssrc field.
 */
#define VJC_HDR_SSRC_IDX	3
#define VJC_HDR_DATALEN_IDX	9
#define VJC_HDR_LEN		20

typedef struct {
  int16_t proto;	///< protocol id X'abcd'
  uint8_t version;	///< protocol version
  uint32_t app_ssrc;	///< application's SSRC
  uint8_t msg_type;	///< Message type
  uint8_t msg_id;	///< Message id
  int16_t data_len;	///< Data length
  uint8_t obj_type;	///< Sender type
  uint32_t src_id;	///< Sender ssrc
  uint16_t port_id;	///< Sender port id
  uint16_t obj_id;	///< Sender obj id
} tVjcHeader;

/**
 * VjcMessage class.
 * This class is used to receive/transmit data between
 * Vreng and the external server.
 */
class VjcMessage {

 public:
  static const uint8_t VERS;
  static const uint32_t MAGIC;
  static const uint16_t MAX_PACKET;

  VjcMessage(WO *sender, uint8_t type, uint8_t id);
  /**<
   * Outgoing constructor
   * WO *sender should be set to the object sending the data
   * int type is the message type (VJC_MSGT_*)
   * int val  is the message value (subtype) (VJC_MSGV_*)
   */

  VjcMessage(WO *sender, uint32_t ssrc, uint8_t type, uint8_t id);
  /**<
   * This constructor is used by Vjc for special update
   * messages. The additional ssrc int is used to force
   * the message header's ssrc to something else than the current value.
   */

  VjcMessage(uint8_t *data, int datalen);
  /**<
   * Incoming constructor
   * Rebuilds a message from the data.
   * uint8_t *data should point to a valid memory area, of lenght
   * int datalen. The data is copied into an internal buffer
   * so the orginal data can be freed once the message is constructed.
   */

  virtual ~VjcMessage();
  /**<
   * Destructor: frees the internal buffer
   */

  tVjcHeader getHeader();
  /**<
   * Returns this message's header.
   * For outgoing messages, the header's data_len field is not set.
   */

  bool isForObject(WO *po);
  /**< Checks wether this message is for the object *po */

  void put8(int val);
  /**< Add a 8bit int to the message */

  void put16(int val);
  /**< Add a 16bit int to the message */

  void put32(int val);
  /**< Add a 32bit int to the message */

  void putOID(WO *po);
  /**<
   * Puts an object's net id.
   * 8bit type, 32bit src id, 16bit port id, 16bit obj id.
   * Special case: the id passed in is the same as the sender's.
   *               We set the type identifier to 0.
   * Special case: NULL is passed as a parameter.
   *               We set all the fields to 0.
   */

  void putPos(WO *po);
  /**<
   * Puts an object position.
   * 6*32bit ((x,y,z),(az,ax,0)).
   * This includes (x,y,z) coordinates, as well as angle data (az, ax, 0)
   * (Y angle never used).
   * All values are rounded to the 3rd decimal, and multiplied by 1000
   * so as not to have to worry about marshalling floating point values.
   */

  void putStr(const char *str);
  /**<
   * Adds a string to the message.
   * A 16b length field is set, followed by the caracters.
   * This method is pretty bad. No encoding is taken into account.
   * Results across heterogenous platforms are undefined.
   * Behavior on platforms where a char is more than 8 bits
   * is completly out of control.
   */

  uint8_t *toBytes(int *len);
  /**<
   * Packs this message into an uint8_t array.
   * The total length of the array is stored in the len parameter.
   * DO NOT free the returned data. Free the message object when
   * you're done with it.
   * Used to transmit the message on a socket.
   * The return value is the binary representation of this message,
   * and the total length of that array is stored in *len
   */

  int sendData();
  /**<
   * Sends data over to the server.
   * The message is not deleted.
   */

  tVjcHeader readHeader();
  /**< Reads a message header */

  int8_t  read8();
  /**< Reads an 8 bit signed int */

  int16_t read16();
  /**< Reads an 16 bit signed int */

  int32_t read32();
  /**< Reads an 32 bit signed int */

  V3 readPoint2D();
  /**< 2D point (2*32bit) */

  V3 readPoint3D();
  /**< 3D point (3*32bit) */

  V3 readDelta();
  /**<
   * Reads three 32bit ints, converted to floats by division by 1000,
   * and returns them in a V3.
   */

  bool hasData(int size);	
  /**<
   * Data availability.
   * Returns true if there are at least size bytes left
   */

 private:
  uint8_t *data;		// buffer
  tVjcHeader header;		// header
  class WO *sender;		// sender
  int cursor;			// current position within the buffer
  int maxlen;			// length of the buffer

  void putHeader();
  /**< Writes the header */

  void setup(WO *po, uint32_t ssrc, uint8_t type, uint8_t id);
  /**< Creates the buffer and the header */

  void dumpHeader(tVjcHeader hdr);
};

/**
 * Vjc class
 *
 * This Vreng object class is used to manage
 * communications between Vreng Vrelet objects
 * and an external server.
 * There should be only one Vjc per scene.
 *
 * All the Vrelet objects will want to send and
 * receive data from the external server. To avoid
 * having each Vrelet object deal with its own sockets,
 * the Vjc centralizes all the work.
 *
 * The class keeps a single instance of Vjc
 * running, and all the methods the Vrelet objects
 * use are static and delegate to that singelton.
 */
class Vjc: public WO {

 public:
  char host[MAXHOSTNAMELEN];	///< Controler's hostname
  uint32_t ssrc;		///< This app's ssrc
  uint16_t serverPort;		///< Controler's listen port
  uint16_t localPort;		///< Local listen port

  static const OClass oclass;   ///< class variable
  const OClass* getOClass() {return &oclass;}

  Vjc(char *l);
  /**<
   * Constructor: checks whether an instance of Vjc has already
   * been created or not. If there already was a server,
   * it actually doesn't do anything else than warn the user
   * of an error. Otherwise, it starts up (opens the sockets),
   * and stores itself in the class static variable.
   *
   * Configuration line syntax:
   * servername[[:server_port[:local_port]]
   * - servername where the server lives
   * - optional port number for that server
   * - optional port number the server should call back to
   */

  static void funcs();	///< init funclist

  static WO * (creator)(char *l);
  /**< Creates from fileline */

  void changePermanent(float lasting);
  /**< Used to scan the sockets for incoming messages */

  void quit();

  static void stop();
  /**<
   * Sends a terminate notification to the external server
   * and closes the comm sockets. The class's singleton instance
   * is also removed.
   */

  static VjcMessage *getData(WO *po);
  /**<
   * Reads in data from the child process, if any is availabe.
   * It is the caller's responsability to delete the message
   * received if it was non null.
   */

  static int sendCommand(WO *po, int val);
  /**< Sends a command */

  static void startApp(class Vrelet *pifc);
  /**<
   * Registers a Vrelet object with the external server
   * The message contains:
   * - localhost name (string)
   * - local listen port (16bit)
   * - Vrelet's given name (string)
   * - Vrelet's class name (string)
   * - Vrelet's codebase URL (string)
   * - Width and heigh (32bit ints)
   */

  static void stopApp(class Vrelet *pifc);
  /**< Application control stuff */

  static Vjc *getServer();
  /**< Returns the current running instance, NULL if there is none */

  class VjcSocket *sock;
  ///< The two-way socket

 private:
  static const uint16_t PING_WAIT;
  static const uint16_t VJS_PORT;
  static const uint16_t VJC_PORT;

  static Vjc *server;		// The singelton instance of this class
  VjcMessage *lastMessage;	// The last message received
  int lastping;			// 'Time' since last ping

  void start();
  /**< Opens the sockets */

  static void setServer(Vjc *server);
  /**< Sets the current running instance */

  void ping();
  /**< Sends a ping to the server */

  void parser(char *l);
  /**< Parses */

  void defaults();
  ///< Sets defaults
};

#endif
