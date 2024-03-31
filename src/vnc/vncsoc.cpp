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
/*  Copyright (C) 1999 AT&T Laboratories Cambridge.  All Rights Reserved.
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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "vreng.hpp"
#include "vncsoc.hpp"
#include "socket.hpp"	// setNoBlocking
#include "nsl.hpp"	// my_gethostbyname


/** Constructor */
VNCSoc::VNCSoc(const char *_servername, uint16_t _port = DEF_VNC_PORT)
{
  strcpy(servername, _servername);
  port = _port;
  buffered = 0;
  bufptr = buf;
  rfbsock = -1;
  struct hostent *hp;

  if ((hp = my_gethostbyname(_servername, AF_INET)) != NULL) {
    memcpy(&ipaddr, hp->h_addr, hp->h_length);
    ipaddr = ntohl(ipaddr);
  }
  else {
    if (! strcmp(servername, "localhost")) {     // force localhost (not resolved)
      struct in_addr myip;
      inet_aton("127.0.0.1", &myip);
      //ipaddr = static_cast<uint32_t>(myip);
    }
    else
      error("VNCSoc: can't resolve %s", _servername);
  }
}

/**
 * ReadFromRFB is called whenever we want to read some data from the RFB
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

/**
 * Read bytes from the server
 */
bool VNCSoc::readRFB(char *out, uint32_t n)
{
  if (n <= buffered) {
    memcpy(out, bufptr, n);
    bufptr += n;
    buffered -= n;
    return true;
  }
  memcpy(out, bufptr, buffered);

  out += buffered;
  n -= buffered;
  bufptr = buf;
  buffered = 0;

  if (n <= sizeof(buf)) {
    while (buffered < n) {
      int i = read(rfbsock, buf + buffered, sizeof(buf) - buffered);
      if (i <= 0) {
	if (i < 0) {
	  if (errno == EWOULDBLOCK || errno == EAGAIN) {
	    i = 0;
	  }
          else {
	    error("VNC: read");
	    return false;
	  }
	}
        else {
          error("VNC: VNC server closed connection");
	  return false;
	}
      }
      buffered += i;
    }
    memcpy(out, bufptr, n);
    bufptr += n;
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
	  }
          else {
	    error("VNC: read");
	    return false;
	  }
	}
        else {
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

/**
 * Write an exact number of bytes, and don't return until you've sent them.
 */
bool VNCSoc::writeExact(char *buf, int n)
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
	}
        else {
	  error("VNC: write");
	  return false;
	}
      }
      else {
	error("VNC: write failed");
	return false;
      }
    }
    i += j;
  }
  return true;
}

/**
 * connectRFB connects to the given TCP port.
 * returns opened socket.
 */
int VNCSoc::connectRFB()
{
  if ((rfbsock = Socket::openStream()) < 0) {
    error("connectRFB: open socket");
    return -1;
  }

  struct sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = htonl(ipaddr);

  trace(DBG_VNC, "connectRFB: connecting to %s:%i %x",
                 servername, ntohs(sa.sin_port), ntohl(sa.sin_addr.s_addr));

  struct timeval timeout;      
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;

  if (setsockopt(rfbsock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&timeout), sizeof(timeout)) < 0) {
    error("setsockopt failed\n");
  }

  if (connect(rfbsock, (const struct sockaddr *) &sa, sizeof(sa)) < 0) {
    perror("VNC: connect");
    error("connectRFB: %s (%d) sock=%d", strerror(errno), errno, rfbsock);
    Socket::closeStream(rfbsock);
    return -1;
  }
  if (Socket::tcpNoDelay(rfbsock) < 0) {
    error("connectRFB: TCP_NODELAY %s (%d)", strerror(errno), errno);
  }
  return rfbsock;
}

/**
 * setBlocking sets a socket into non-blocking mode.
 */
bool VNCSoc::setBlocking()
{
  if (Socket::setNoBlocking(rfbsock) < 0) return false;
  return true;
}

/**
 * Test if the other end of a socket is on the same machine.
 */
bool VNCSoc::sameMachine()
{
  struct sockaddr_in peersa, mysa;
  socklen_t slen = sizeof(struct sockaddr_in);

  getpeername(rfbsock, (struct sockaddr *) &peersa, &slen);
  getsockname(rfbsock, (struct sockaddr *) &mysa, &slen);

  return (peersa.sin_addr.s_addr == mysa.sin_addr.s_addr);
}

/**
 * Returns the socket used
 */
int VNCSoc::getSock()
{
  return rfbsock;
}

#if 0 //notused
/**
 * Print out the contents of a packet for debugging.
 */
void VNCSoc::PrintInHex(char *buf, int len)
{
  int i;
  char c, str[17];

  str[16] = 0;

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
#endif //notused
