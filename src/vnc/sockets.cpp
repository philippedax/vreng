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
/*
 *  Copyright (C) 1999 AT&T Laboratories Cambridge.  All Rights Reserved.
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 *  USA.
 */
#include "vreng.hpp"
#include "sockets.hpp"
#include "socket.hpp"	// setNoBlocking
#include "nsl.hpp"	// my_gethostbyname


// Constructors

VNCSockets::VNCSockets(const char *_servername, uint16_t _port=5901)
{
  strcpy(ServerName, _servername);
  port = _port;
  buffered = 0;
  bufoutptr = buf;
  rfbsock = -1;
  if (! StringToIPAddr()) error("VNCSockets: can't resolve %s", _servername);
}

VNCSockets::VNCSockets(uint32_t IPAddr, uint16_t _port=5901)
{
  error("VNCSockets::VNCSockets: 3 not used, port=%d", _port);
  port = _port;
  buffered = 0;
  bufoutptr = buf;
  rfbsock = -1;
  ipaddr = IPAddr;
}

/*
 * ReadFromRFBServer is called whenever we want to read some data from the RFB
 * server.  It is non-trivial for two reasons:
 *
 * 1. For efficiency it performs some intelligent buffering, avoiding invoking
 *    the read() system call too often.  For small chunks of data, it simply
 *    copies the data out of an internal buffer.  For large amounts of data it
 *    reads directly into the buffer provided by the caller.
 *
 * 2. Whenever read() would block, it invokes the Xt event dispatching
 *    mechanism to process X events.  In fact, this is the only place these
 *    events are processed, as there is no XtAppMainLoop in the program.
 */

/*
 * Read bytes from the server
 */
bool VNCSockets::ReadFromRFBServer(char *out, uint32_t n)
{
  if (n <= buffered) {
    memcpy(out, bufoutptr, n);
    bufoutptr += n;
    buffered -= n;
    return true;
  }
  memcpy(out, bufoutptr, buffered);

  out += buffered;
  n -= buffered;
  bufoutptr = buf;
  buffered = 0;

  if (n <= sizeof(buf)) {
    while (buffered < n) {
      int i = read(rfbsock, buf + buffered, sizeof(buf) - buffered);
      if (i <= 0) {
	if (i < 0) {
	  if (errno == EWOULDBLOCK || errno == EAGAIN) {
	    i = 0;
	  } else {
	    error("VNC: read");
	    return false;
	  }
	} else {
	    error("VNC: VNC server closed connection");
	  return false;
	}
      }
      buffered += i;
    }
    memcpy(out, bufoutptr, n);
    bufoutptr += n;
    buffered -= n;
    return true;
  }
  else {
    while (n > 0) {
      int i = read(rfbsock, out, n);
      if (i <= 0) {
	if (i < 0) {
	  if (errno == EWOULDBLOCK || errno == EAGAIN) {
	    i = 0;
	  } else {
	    error("VNC: read");
	    return false;
	  }
	} else {
	    error("VNC: VNC server closed connection");
	  return false;
	}
      }
      out += i;
      n -= i;
    }
    return true;
  }
}

/*
 * Write an exact number of bytes, and don't return until you've sent them.
 */
bool VNCSockets::WriteExact(char *buf, int n)
{
  for (int i=0; i < n; ) {
    int j = write(rfbsock, buf + i, (n - i));

    if (j <= 0) {
      if (j < 0) {
	if (errno == EWOULDBLOCK || errno == EAGAIN) {
          fd_set fds;
	  FD_ZERO(&fds);
	  FD_SET(rfbsock, &fds);
	  if (select(rfbsock+1, NULL, &fds, NULL, NULL) <= 0) {
	    error("VNC: select");
	    return false;
	  }
	  j = 0;
	} else {
	  error("VNC: write");
	  return false;
	}
      } else {
	error("VNC: write failed");
	return false;
      }
    }
    i += j;
  }
  return true;
}

/*
 * ConnectToTcpAddr connects to the given TCP port.
 */
int VNCSockets::ConnectToTcpAddr()
{
  if ((rfbsock = Socket::openStream()) < 0) { error("ConnectToTcpAddr: socket"); return -1; }

  struct sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = htonl(ipaddr);

  trace(DBG_VNC, "ConnectToTcpAddr: connecting to %s:%i %x",
                 ServerName, ntohs(sa.sin_port), ntohl(sa.sin_addr.s_addr));

  if (connect(rfbsock, (const struct sockaddr *) &sa, sizeof(sa)) < 0) {
    perror("VNC: connect");
    error("ConnectToTcpAddr: %s (%d) sock=%d", strerror(errno), errno, rfbsock);
    Socket::closeStream(rfbsock);
    return -1;
  }
  if (Socket::tcpNoDelay(rfbsock) < 0)
    error("ConnectToTcpAddr: TCP_NODELAY %s (%d)", strerror(errno), errno);
  return rfbsock;
}

/*
 * SetNonBlocking sets a socket into non-blocking mode.
 */
bool VNCSockets::SetNonBlocking()
{
  if (Socket::setNoBlocking(rfbsock) < 0) return false;
  return true;
}

/*
 * StringToIPAddr - convert a host string to an IP address.
 */
bool VNCSockets::StringToIPAddr()
{
  struct hostent *hp;

  if ((hp = my_gethostbyname(ServerName, AF_INET)) != NULL) {
    memcpy(&ipaddr, hp->h_addr, hp->h_length);
    ipaddr = ntohl(ipaddr);
    trace(DBG_VNC, "StringToIPAddr: ServerName=%s (%x)", ServerName, ipaddr);
    return true;
  }
  return false;
}

/*
 * Test if the other end of a socket is on the same machine.
 */
bool VNCSockets::SameMachine()
{
  struct sockaddr_in peersa, mysa;
  socklen_t slen = sizeof(struct sockaddr_in);

  getpeername(rfbsock, (struct sockaddr *) &peersa, &slen);
  getsockname(rfbsock, (struct sockaddr *) &mysa, &slen);

  return (peersa.sin_addr.s_addr == mysa.sin_addr.s_addr);
}

#if 0 //unused
/*
 * Print out the contents of a packet for debugging.
 */
void VNCSockets::PrintInHex(char *buf, int len)
{
  int i;
  char c, str[17];

  str[16] = 0;

  trace(DBG_VNC, "ReadExact: ");

  for (i = 0; i < len; i++) {
    if ((i % 16 == 0) && (i != 0))
      fprintf(stderr, "           ");
    c = buf[i];
    str[i % 16] = (((c > 31) && (c < 127)) ? c : '.');
    fprintf(stderr,"%02x ", (uint8_t) c);
    if ((i % 4) == 3)
      fprintf(stderr, " ");
    if ((i % 16) == 15)
      fprintf(stderr,"%s\n", str);
  }
  if ((i % 16) != 0) {
    for (int j = i % 16; j < 16; j++) {
      fprintf(stderr, "   ");
      if ((j % 4) == 3) fprintf(stderr, " ");
    }
    str[i % 16] = 0;
    fprintf(stderr,"%s\n", str);
  }
  fflush(stderr);
}
#endif //unused

/*
 * Returns the socket used
 */
int VNCSockets::GetSock()
{
  return rfbsock;
}
