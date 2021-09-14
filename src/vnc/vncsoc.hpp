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
#ifndef VNCSOC_HPP
#define VNCSOC_HPP

#define VNC_BUF_SIZE	8192


/**
 * VNCSocket class
 */
class VNCSockets {

 private:
  char servername[MAXHOSTNAMELEN];
  ///< name of the server

  uint16_t port;
  ///< port

  uint32_t ipaddr;
  ///< IP address of the server

  int rfbsock;
  ///< the used socket

  uint32_t buffered;
  uint8_t buf[VNC_BUF_SIZE];
  uint8_t *bufoutptr;
  ///< buffer

  bool stringToIP();
  ///< stores the IP address of the server in ipaddr, returns false if unknown

 public:
  VNCSockets() {};
  VNCSockets(const char *_servername, uint16_t _port);
  VNCSockets(uint32_t ipaddr, uint16_t _port);
  ///< constructors

#if 0 //unused
  void PrintInHex(char *buf, int len);
  ///< Print out the contents of a packet for debugging.
#endif

  signed int connectRFB();
  /**<
   * Connects to the given TCP port.
   * Returns the socket if connected, -1 if connection failed */

  bool sameMachine();
  ///< Test if the other end of a socket is on the same machine.

  bool setBlocking();
  ///< sets a socket into non-blocking mode.

  bool readRFB(char *out, uint32_t n);
  ///< Read bytes from the sever and stores it in the buffer

  bool writeExact(char *buf, int n);
  ///< Write an exact number of bytes, and don't return until you've sent them.

  int getSock();
  ///< get the socket used
};

#endif
