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
#include "wobject.hpp"
#include "vjc.hpp"
#include "socket.hpp"	// setNoBlocking
#include "nsl.hpp"	// get_mygethostbyname
#include "netobj.hpp"	// NetObject


const OClass Vjc::oclass(VJC_TYPE, "Vjc", Vjc::creator);

const uint8_t VjcMessage::VERS = 0;		// V0
const uint32_t VjcMessage::MAGIC = 0xABCD;	// magic header
const uint16_t VjcMessage::MAX_PACKET = 512;	// packet size
const uint16_t Vjc::PING_WAIT = 200;		// 200 ms
const uint16_t Vjc::VJS_PORT = DEF_VJS_PORT;	// Vjs port
const uint16_t Vjc::VJC_PORT = DEF_VJS_PORT;	// Vjc port

// class member initialization
Vjc *Vjc::server = NULL;

void Vjc::funcs() {}


WObject * Vjc::creator(char *l)
{
  return new Vjc(l);
}

void Vjc::defaults()
{
  strcpy(host, DEF_VJS_SERVER);
  serverPort = VJS_PORT;
  localPort = VJC_PORT;
}

void Vjc::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (!stringcmp(l, "server=")) l = parse()->parseString(l, host, "server");
  }
  end_while_parse(l);

  char *p;
  if ((p = strchr(host, ':'))) {
    *p++ = 0;
    serverPort = atoi(p);
  }
}

Vjc::Vjc(char *l)
{
  parser(l);

  initObject(INVISIBLE);
  enablePermanentMovement();

  setServer(this);
  sock = NULL;
  lastMessage = NULL;
  lastping = 0;
  ssrc = NetObject::getMySsrcId();
  start();
}

/* Returns the singleton instance */
Vjc * Vjc::getServer()
{
  return server;
}

/* Sets the singleton instance */
void Vjc::setServer(Vjc *_server)
{
  server = _server;
}

/* Used to send pings to the server */
void Vjc::changePermanent(float lasting)
{
  getServer()->ping();
}

/* Sends a simple (no data) control command to the child */
int Vjc::sendCommand(WObject *po, int id)
{
  VjcMessage *msg = new VjcMessage(po, VJC_MSGT_CTRL, id);
  int ret = msg->sendData();
  if (msg) delete msg;
  msg = NULL;
  return ret;
}

/* Sends a ping to the server every PING_WAIT times */
void Vjc::ping()
{
  lastping++;
  if (lastping >= PING_WAIT) {
    sendCommand(this, VJC_MSGV_PING);
    lastping = 0;
  }
}

void Vjc::quit()
{
  stop();
}

/* Sends an initiate to the server */
void Vjc::start()
{
  sock = new VjcSocket(localPort, host, serverPort);

  // Open the socket
  if (sock->openSocket() == -1) {
    error("Unable to open Vjc socket");
    if (sock) delete sock;
  }
  else {
    localPort = sock->listenPort;
    VjcMessage *msg = new VjcMessage(this, VJC_MSGT_CTRL, VJC_MSGV_INITIATE);
    msg->sendData();
    if (msg) delete msg;
  }
}

/* Sends a terminate notification to the server */
void Vjc::stop()
{
  Vjc *srv = getServer();
  if (! srv) return;

  trace(DBG_IFC, "Shutting down vjs server");
  sendCommand(NULL, VJC_MSGV_TERMINATE);
  if (srv->sock) delete srv->sock;
  srv->sock = NULL;
  srv->serverPort = 0;
  srv->localPort = 0;
  server = NULL;
  setServer(NULL);
}

/* Unregister a Vrelet object with the server */
void Vjc::stopApp(Vrelet *po)
{
  Vjc *srv = getServer();
  if (! srv) return;

  sendCommand(po, VJC_MSGV_UNREGISTER);
  if (srv->lastMessage) delete srv->lastMessage;
  srv->lastMessage = NULL;
}

/* Register a Vrelet object with the server */
void Vjc::startApp(Vrelet *po)
{
  Vjc *srv = getServer();
  if (! srv)  return;

  char hostname[MAXHOSTNAMELEN];
  if (gethostname(hostname, sizeof(hostname)-1) < 0) return;

  VjcMessage *msg = new VjcMessage(po, VJC_MSGT_CTRL, VJC_MSGV_REGISTER);
  msg->putStr(hostname);
  msg->put16(srv->localPort);
  msg->putStr(po->getInstance());
  msg->putStr(po->app);
  msg->putStr(po->url);
  msg->put32(po->incrx);
  msg->put32(po->incry);
  msg->sendData();
  if (msg) delete msg;
  msg = NULL;
}


//*
//* Network management
//*

/* Initialize a pair of sockets for communicating with the child app */
VjcSocket::VjcSocket(uint16_t _listenPort, const char *_destHost, uint16_t _destPort)
{
  listenPort = _listenPort;
  destPort = _destPort;
  sadest = 0;
  sdr = -1;
  sdw = -1;

  struct sockaddr_in *sa;
  if ((sa = new struct sockaddr_in[1]) == NULL) {
    //error("VjcSocket: can't new sa");
    return;
  }
  memset(sa, 0, sizeof(struct sockaddr_in));
  sa->sin_family = AF_INET;
  sa->sin_port = htons(destPort);

  struct hostent *hp;
  if ((hp = my_gethostbyname(_destHost, AF_INET)) == NULL) {
    error("VjcSocket: unknown server %s", _destHost);
    if (sa) delete[] sa;
    return;
  }
  memcpy(&sa->sin_addr, hp->h_addr_list[0], hp->h_length);
  sadest = sa;

  statecon = SOCK_CLOSED;
}

/* Opens the receiver socket for this socket pair */
int VjcSocket::openRecv()
{
  if ((sdr = Socket::openDatagram()) < 0) return 0;

  for (int tries = 0; tries<10; tries++) {
    if (Socket::bindSocket(sdr, INADDR_ANY, listenPort + tries) != -1) {
      listenPort += tries;
      statecon = SOCK_RCVOPEN;
      notice("Vjc receiver bound to port %d", listenPort);
      return 1;
    }
  }
  Socket::closeDatagram(sdr);
  return 0;
}

/* Opens the sender socket. IO is set to non-blocking on this one */
int VjcSocket::openSend()
{
  if ((sdw = Socket::openDatagram()) < 0) return 0;

  Socket::setNoBlocking(sdw);
  statecon = SOCK_CONNECT;
  return 1;
}

/* Tries a connect to the client app */
int VjcSocket::connectSend()
{
  struct timeval timeout;      
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;

  if (setsockopt(sdw, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    error("setsockopt failed\n");
  if (connect(sdw, (const struct sockaddr *) sadest, sizeof(struct sockaddr_in)) < 0) {
    if ((errno == EINPROGRESS) || (errno == EALREADY)) {
      // Socket set to non-blocking to prevent an app freeze
      return 1;
    }
    else {
      //error("connectSend: %s", strerror(errno));
      return 0;
    }
  }
  else statecon = SOCK_OPEN;
  return 1;
}

/* Check if the non-blocking connect call on the send socket finished or not */
bool VjcSocket::isConnected()
{
  fd_set set;
  struct timeval timeout;

  FD_ZERO(&set);
  FD_SET(sdw, &set);
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  if (select(sdw+1, NULL, &set, NULL, &timeout) > 0) {
    statecon = SOCK_OPEN;	// The socket finished connecting
    return true;
  }
  return false;
}

/* Opens a VjcSocket. This call is 'non-blocking' */
int VjcSocket::openSocket()
{
  // check if it's not already open
  if (statecon == SOCK_OPEN) return 1;
  // Open the receiver
  if (statecon == SOCK_CLOSED) if (openRecv() == 0) return -1;
  // Open the sender
  if (statecon == SOCK_RCVOPEN) if (openSend() == 0) return -1;
  // Connect the sender
  if (statecon == SOCK_CONNECT) if (connectSend() == 0) return -1;
  if (statecon == SOCK_OPEN) return 1;	// Everything is connected
  // One of the steps didn't finish yet.
  return 0;
}

/* Closes and frees a VjcSocket */
VjcSocket::~VjcSocket()
{
  if (statecon >= SOCK_RCVOPEN) {
    Socket::closeDatagram(sdr);
    sdr = -1;
  }
  if (statecon >= SOCK_CONNECT) {
    Socket::closeDatagram(sdw);
    sdw = -1;
  }
  if (sadest) delete[] sadest;
  sadest = NULL;
}

/*
 * Utility functions
 */
bool VjcSocket::isOpen()
{
  return (statecon == SOCK_OPEN);
}

bool VjcSocket::isClosed()
{
  return (statecon == SOCK_CLOSED);
}


/*
 * VjcMessage
 * Data structure used to send messages to the controler
 */

/* Constructor for outgoing messages */
VjcMessage::VjcMessage(WObject *po, uint32_t ssrc, int type, int id)
{
  setup(po, ssrc, type, id);
}

/* Constructor for outgoing messages */
VjcMessage::VjcMessage(WObject *po, int type, int id)
{
  setup(po, (Vjc::getServer() == NULL ? NetObject::getMySsrcId() : Vjc::getServer()->ssrc), type, id);
}

/* Creates a new tVjcHeader */
static
tVjcHeader vjcNewHeader(uint32_t ssrc, int type, int id, int len)
{
  tVjcHeader hdr;

  hdr.proto = VjcMessage::MAGIC;
  hdr.version = VjcMessage::VERS;
  hdr.app_ssrc = ssrc;
  hdr.msg_type = type;
  hdr.msg_id  = id;
  hdr.data_len = len;
  return hdr;
}

/* Setups the header */
void VjcMessage::setup(WObject *po, uint32_t ssrc, int type, int id)
{
  cursor = 0;	// set the cursor to the start of the buffer
  header = vjcNewHeader(ssrc, type, id, 0);	// create the header
  sender = po;	// set the sender object
  if ((data = new uint8_t[VjcMessage::MAX_PACKET]) == NULL) {	// alloc our data array
    //error("cannot new data buffer for outgoing VjcMessage");
    return;
  }
  putHeader(); // Write out the header
}

/* Constructor for incoming messages */
VjcMessage::VjcMessage(uint8_t *_data, int _datalen)
{
  cursor = 0;		// set the cursor to the start of the buffer
  sender = NULL;	// sender will always be null on incoming messages
  if ((data = new uint8_t[_datalen]) == NULL) {	// alloc the buffer
    //error("VjcMessage::VjcMessage: can't new for incoming VjcMessage");
    return;
  }
  memcpy(data, _data, _datalen);	// copy the data
  maxlen = _datalen;			// mark the maximum read length
  header = readHeader();		// read the header in
}

/* Destructor */
VjcMessage::~VjcMessage()
{
  if (data) delete[] data;
}

/* Checks whether the packet was for this WObject */
bool VjcMessage::isForObject(WObject *po)
{
  return (header.src_id == po->getSrcId())
      && (header.port_id == po->getPortId())
      && (header.obj_id == po->getObjId());
}

/* Returns this message's header */
tVjcHeader VjcMessage::getHeader()
{
  return header;
}

/* Read a header struct for the raw packet data */
void VjcMessage::dumpHeader(tVjcHeader hdr)
{
  //error("%x %d %d %d %d %d",
  //hdr.proto, hdr.version, hdr.app_ssrc, hdr.msg_type, hdr.msg_id, hdr.data_len);
}

#define VJC_CHECK_OVERFLOW_1(a) \
  if (cursor >= VjcMessage::MAX_PACKET-a) { \
    error("Write past end of buffer (%d %d)", getHeader().msg_type, getHeader().msg_id); \
    return; \
  }

/* Add a 8bit int to the message */
void VjcMessage::put8(int val)
{
  //pd VJC_CHECK_OVERFLOW_1(sizeof(uint8_t))
  data[cursor] = (uint8_t) (0x000000FF) & val;
  cursor += sizeof(uint8_t);
}

/* Add a 16bit int to the message */
void VjcMessage::put16(int val)
{
  //pd VJC_CHECK_OVERFLOW_1(sizeof(int16_t))
  data[cursor]   = (val & 0xFF00) >> 8;
  data[cursor+1] = (val & 0x00FF);
  cursor += sizeof(int16_t);
}

/* Add a 32bit int to the message */
void VjcMessage::put32(int val)
{
  //pd VJC_CHECK_OVERFLOW_1(sizeof(int32_t))
  data[cursor]   = (val & 0xFF000000) >> 24;
  data[cursor+1] = (val & 0x00FF0000) >> 16;
  data[cursor+2] = (val & 0x0000FF00) >> 8;
  data[cursor+3] = (val & 0x000000FF);
  cursor += sizeof(int32_t);
}

/* Add a string to the message */
void VjcMessage::putStr(const char *str)
{
  int i, len = strlen(str);
  put16(len);
  VJC_CHECK_OVERFLOW_1(len)
  for (i=0; i<len ; i++)
    data[cursor+i] = (uint8_t) str[i];
  cursor += len;
}

/* Puts an object's net id */
void VjcMessage::putOID(WObject *po)
{
  if (po) {
    put8((po == sender ? 0 : po->type));
    put32(po->getSrcId());
    put16(po->getPortId());
    put16(po->getObjId());
  }
  else {
    put8( 0);
    put32(0);
    put16(0);
    put16(0);
  }
}

/* Puts an object position */
void VjcMessage::putPos(WObject *po)
{
  put32((int32_t) (po->pos.x * 1000));
  put32((int32_t) (po->pos.y * 1000));
  put32((int32_t) (po->pos.z * 1000));
  put32((int32_t) (po->pos.az * 1000));
  put32((int32_t) (po->pos.ax * 1000));
  put32((int32_t) 0);
}

/* Writes the message header */
void VjcMessage::putHeader()
{
  // Write the header (proto part)
  put16(header.proto);
  put8( header.version);
  put32(header.app_ssrc);
  // Write the header (message part)
  put8( header.msg_type);
  put8( header.msg_id);
  put16(header.data_len);
  // Write the caller's id
  putOID(sender);
}

/* Packs this message into an uint8_t array */
uint8_t *VjcMessage::toBytes(int *pktlen)
{
  int datalen = cursor - VJC_HDR_LEN;	// total length - header

  *pktlen = cursor;	// total length is where our cursor currently is
  cursor = VJC_HDR_DATALEN_IDX;	// set the data length in the packet
  put16(datalen);
  cursor = VJC_HDR_SSRC_IDX;	// set the ssrc (could have been updated)
  put32(Vjc::getServer()->ssrc);
  // Restore the cursor. This should actually never be needed, except
  // if someone sends the same message twice, after updating it
  cursor = *pktlen;
  return data;		// return a pointer to the data.
}

/* Send data over to the server */
int VjcMessage::sendData()
{
  int pktlen = 0;
  uint8_t *pkt;

  /* Check that the socket is ready */
  Vjc *srv = Vjc::getServer();
  if (srv && (srv->sock->statecon == VjcSocket::SOCK_CONNECT)) {
    // The socket is still trying to connect. Check if it's done or not.
    if (! srv->sock->isConnected()) return 0;  // The connection is still in progress
  }
  if (srv && (srv->sock->statecon == VjcSocket::SOCK_OPEN)) {
    /* Check if an SSRC change occured since we sent the REGISTER commands */
    if (NetObject::getMySsrcId() != srv->ssrc) {
      if (! ((getHeader().msg_type == VJC_MSGT_CTRL)
          && (getHeader().msg_id  == VJC_MSGV_REGISTER))) {

        VjcMessage *msg = new VjcMessage(srv, srv->ssrc, VJC_MSGT_CTRL, VJC_MSGV_UPDATE);
        //error("Vjc: updating SSRC for Vjc (old:%d, new:%d)", srv->ssrc, NetObject::getMySsrcId());
        msg->put32(NetObject::getMySsrcId());
        pkt = msg->toBytes(&pktlen);
        send(srv->sock->sdw, pkt, pktlen, 0);
        if (msg) delete msg;
      }
      srv->ssrc = NetObject::getMySsrcId();
    }

    /* send the message */
    pkt = toBytes(&pktlen);
    send(srv->sock->sdw, pkt, pktlen, 0);
    //error("Vjc: sending %d bytes with '%02x%02x' %d %08x (%d %d)",
    //	  sent, pkt[0], pkt[1], pkt[2], NetObject::getMySsrcId(),
    //	  getHeader().msg_type,
    //	  getHeader().msg_id);
    return 1;
  }
  return 0;
}

#define VJC_CHECK_OVERFLOW_2(a) \
  if (cursor > maxlen-a) { \
    error("Read past end of buffer (%d %d)", getHeader().msg_type, getHeader().msg_id); \
    return NULL; \
  }

/* Reads in data from the child process, if any is availabe */
VjcMessage * Vjc::getData(WObject *po)
{
  /* check that the socket is really open */
  Vjc *srv = getServer();
  if ((! srv) || (srv->sock->statecon != VjcSocket::SOCK_OPEN)) return NULL;

  if (srv->lastMessage) goto haspack;

  uint8_t *pkt;

  /* set a 0 sec timeout:
   * the select will return instantly if no data is available */
  fd_set fds;
  struct timeval to;

  FD_ZERO(&fds);
  FD_SET(srv->sock->sdr, &fds);
  to.tv_sec = 0;
  to.tv_usec = 0;

  if (select(srv->sock->sdr+1, &fds, NULL, NULL, &to) > 0) {
    int r;

    if ((pkt = new uint8_t[VjcMessage::MAX_PACKET]) == NULL) {
      //error("Vjc::getData: can't new reception buffer");
      return NULL;
    }

    if ((r = recv(srv->sock->sdr, (void *) pkt, VjcMessage::MAX_PACKET, 0)) > 0) {
      if (r < VJC_HDR_LEN) {
	/* We didn't get a whole header */
	error("Vjc::getData: dropped incomplete packet (%d)", r);
      }
      else {
	/* Message size is big enough to contain a header */
	srv->lastMessage = new VjcMessage(pkt, r);

	/* Check the header */
	// TODO: check protocol id and version number
	if (srv->lastMessage->getHeader().data_len > (VjcMessage::MAX_PACKET-VJC_HDR_LEN)) {
	  /* Header and real length don't agree */
	  error("Vjc::getData: illegal data length");
	  if (srv->lastMessage) delete srv->lastMessage;
	  srv->lastMessage = NULL;
	}
        else goto haspack;
      }
    }
    // (r < 0) || (packet too short) || (invalid header)
    if (pkt) delete[] pkt;
    pkt = NULL;
    return NULL;
  }
  return NULL; // select <= 0

haspack:
  // A message was read from the socket, or had been read previously
  if (srv->lastMessage->isForObject(po)) {
    VjcMessage *ret = srv->lastMessage;
    srv->lastMessage = NULL;
    return ret;
  }
  return NULL;
}

/* Reads an 8 bit signed int */
int8_t VjcMessage::read8()
{
  //pd VJC_CHECK_OVERFLOW_2(sizeof(int8_t))
  uint8_t val = data[cursor];
  cursor += sizeof(int8_t);
  return val;
}

/* Reads a 16 bit signed int */
int16_t VjcMessage::read16()
{
  //pd VJC_CHECK_OVERFLOW_2(sizeof(int16_t))
  int16_t val = (data[cursor] << 8) + (0x00ff & data[cursor+1]);
  cursor += sizeof(int16_t);
  return val;
}

/* Reads a 32 bit signed int */
int32_t VjcMessage::read32()
{
  //pd VJC_CHECK_OVERFLOW_2(sizeof(int32_t))
  int32_t val =
          (0xff000000 & (data[cursor]   << 24))
        + (0x00ff0000 & (data[cursor+1] << 16))
        + (0x0000ff00 & (data[cursor+2] << 8))
        + (0x000000ff &  data[cursor+3]);
  cursor += sizeof(int32_t);
  return val;
}

/* Read a header struct for the raw packet data */
tVjcHeader VjcMessage::readHeader()
{
  tVjcHeader hdr;

  hdr.proto    = read16();
  hdr.version  = read8();
  hdr.app_ssrc = read32();
  hdr.msg_type = read8();
  hdr.msg_id   = read8();
  hdr.data_len = read16();
  hdr.obj_type = read8(); if (hdr.obj_type == 0) hdr.obj_type = VJC_TYPE;
  hdr.src_id   = read32();
  hdr.port_id  = read16();
  hdr.obj_id   = read16();
  return hdr;
}

/* Read two 32bit ints, and return them as the x and y coords. of a V3 */
V3 VjcMessage::readPoint2D()
{
  V3 point;

  //pd VJC_CHECK_OVERFLOW_2(2*sizeof(int32_t))
  point.v[0] = ((float) read32());
  point.v[1] = ((float) read32());
  point.v[2] = 0;
  return point;
}

V3 VjcMessage::readPoint3D()
{
  V3 point = readPoint2D();

  //pd VJC_CHECK_OVERFLOW_2(sizeof(int32_t))
  point.v[2] = ((float) read32());
  return point;
}

/* Read three 32bit ints, converted to floats by division by 1000 */
V3 VjcMessage::readDelta()
{
  V3 point = readPoint3D();

  point.v[0] /= 1000.;
  point.v[1] /= 1000.;
  point.v[2] /= 1000.;
  return point;
}

/* Returns true if there is at least size bytes left to be read */
bool VjcMessage::hasData(int size)
{
  return ((cursor+size) <= maxlen);
}
