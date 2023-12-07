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
#ifndef SOCKET_HPP
#define SOCKET_HPP


#define NEEDLOOPBACK	0	// no loopback for ipmulticast

/**
 * Socket class
 */
class Socket {

public:
  Socket() {};
  virtual ~Socket() {};

  static int openDatagram();
  static int openStream();
  static void closeDatagram(int sock);
  static void closeStream(int sock);
  static int connection(int sock, const sockaddr_in *sa);
  static int reuseAddr(int sock);
  static int tcpNoDelay(int sock);
  static uint16_t getSrcPort(int sock);
  static int setNoBlocking(int sock);
  static int setNoLoopback(int sock);
  static int setScope(int sock, uint8_t ttl);
  static int addMembership(int sock, const void *pmreq);
  static int dropMembership(int sock, const void *pmreq);
  static int bindSocket(int sock, uint32_t uni_addr, uint16_t port);
  static int createUcastSocket(uint32_t uni_addr, uint16_t port);
  static int createSendSocket(uint8_t ttl);
  static bool isMulticastAddress(uint32_t address);

  //notused static int setBlocking(int sock);

private:
  static void setSendSocket(int sock, uint8_t ttl);
  static int handleLoopback(int sock, uint8_t loop);
  static int handleBlocking(int sock, bool block);
  static int setLoopback(int sock);
};

#endif
