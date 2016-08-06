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
#include "vreng.hpp"
#include "socket.hpp"
#include "prof.hpp"


/** Open a datagram socket */
int Socket::openDatagram()
{
  int sock = -1;

  if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    error("socket: %s (%d)", strerror(errno), errno);
  if (sock > 0) cnt_open_socket++;
  return sock;
}

/** Open a stream socket */
int Socket::openStream()
{
  int sock = -1;

  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    error("socket: %s (%d)", strerror(errno), errno);
  if (sock > 0) cnt_open_socket++;
  return sock;
}

/** Close a socket datagram*/
void Socket::closeDatagram(int sock)
{
  if (sock > 0) {
    close(sock);
    cnt_close_socket++;
  }
}

/** Close a socket stream */
void Socket::closeStream(int sock)
{
  if (sock > 0) {
    close(sock);
    cnt_close_socket++;
  }
}

int Socket::connection(int sock, const sockaddr_in *sa)
{
  if (connect(sock, (struct sockaddr *) sa, sizeof(struct sockaddr_in)) < 0) {
    error("connect: %s (%d)", strerror(errno), errno);
    close(sock);
    cnt_close_socket++;
    return -1;
  }
  return sock;
}

/**
 * Do REUSEADDR on the socket
 * return sock if OK, else -1
 */
int Socket::reuseAddr(int sock)
{
  const int one = 1;

  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
		 (char *) &one, sizeof(one)) < 0) {
    error("SO_REUSEADDR: %s (%d)", strerror(errno), errno);
    return -1;
  }
  return sock;
}

int Socket::tcpNoDelay(int sock)
{
  const int one = 1;

  if (setsockopt(sock, SOL_SOCKET, TCP_NODELAY,
		 (char *) &one, sizeof(one)) < 0) {
    error("TCP_NODELAY: %s (%d) sock=%d", strerror(errno), errno, sock);
    return -1;
  }
  return sock;
}

uint16_t Socket::getSrcPort(int sock)
{
  struct sockaddr_in sa;
  socklen_t slen = sizeof(struct sockaddr_in);

  sa.sin_family = AF_INET;
  sa.sin_port = 0;
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  if (getsockname(sock, (struct sockaddr *) &sa, (socklen_t *) &slen) < 0) {
    error("getSrcPort: %s (%d)", strerror(errno), errno);
    return 0;
  }
  return ntohs(sa.sin_port);
}

/**
 * Control blocking(1)/non blocking(0)
 * return sock if OK, else -1
 */
int Socket::handleBlocking(int sock, bool block)
{
#if HAVE_FCNTL
  int flags;

  if ((flags = fcntl(sock, F_GETFL)) < 0) {
    error("F_GETFL: %s (%d)", strerror(errno), errno);
    return -1;
  }
  if (block)
    flags &= ~O_NONBLOCK;
  else
    flags |= O_NONBLOCK;
  if (fcntl(sock, F_SETFL, flags) < 0) {
    error("F_SETFL: %s (%d)", strerror(errno), errno);
    return -1;
  }
#endif // HAVE_FCNTL
  return sock;
}

int Socket::setBlocking(int sock)
{
  return handleBlocking(sock, true);
}

int Socket::setNoBlocking(int sock)
{
  return handleBlocking(sock, false);
}

/**
 * Set the ttl
 * return sock if OK, else -1
 */
int Socket::setScope(int sock, uint8_t _ttl)
{
#if defined(__WIN32__) || defined(_WIN32)
#define TTL_TYPE int
#else
#define TTL_TYPE uint8_t
#endif
  if (_ttl) {
    TTL_TYPE ttl = (TTL_TYPE)_ttl;
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&ttl, sizeof(ttl)) < 0) {
      error("IP_MULTICAST_TTL: %s (%d)", strerror(errno), errno);
      return -1;
    }
  }
  return sock;
}

/**
 * Set loopback: active (1) either inactive (0)
 * return sock if OK, else -1
 */
int Socket::handleLoopback(int sock, uint8_t loop)
{
#ifdef IP_MULTICAST_LOOP // Windoze doesn't handle IP_MULTICAST_LOOP
  if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP,
		 &loop, sizeof(loop)) < 0) {
#if IPMC_ENABLED
    error("IP_MULTICAST_LOOP: %s (%d)", strerror(errno), errno);
#endif
    return -1;
  }
#endif
  return sock;
}

int Socket::setLoopback(int sock)
{
  return handleLoopback(sock, 1);
}

int Socket::setNoLoopback(int sock)
{
  return handleLoopback(sock, 0);
}

int Socket::addMembership(int sock, const void *pmreq)
{
  if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                 pmreq, sizeof(struct ip_mreq)) < 0) {
#if IPMC_ENABLED
    error("IP_ADD_MEMBERSHIP: %s (%d)", strerror(errno), errno);
#endif
    return -1;
  }
  return 0;
}

int Socket::dropMembership(int sock, const void *pmreq)
{
  if (setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                 pmreq, sizeof(struct ip_mreq)) < 0) {
#if IPMC_ENABLED
    error("IP_DROP_MEMBERSHIP: %s (%d)", strerror(errno), errno);
#endif
    return -1;
  }
  return 0;
}

/** Set a Multicast socket */
void Socket::setSendSocket(int sock, uint8_t ttl)
{
#ifdef PROXY_MULTICAST
  if (mup.active) {
    setNoBlocking(sock);
    return;
  }
#endif

  setScope(sock, ttl);
#if NEEDLOOPBACK
  setLoopback(sock);    // loopback
#else
  setNoLoopback(sock);  // no loopback (default)
#endif
  setNoBlocking(sock);
}

/**
 * Create an UDP socket
 * Prevue pour emettre (uni et mcast) et recevoir (unicast donc)
 * Do setScope (ttl) and setLoopback (off)
 * return sock if OK, else -1
 */
int Socket::createSendSocket(uint8_t ttl)
{
  int sock = -1;

  if ((sock = openDatagram()) >= 0)
    setSendSocket(sock, ttl);
  return sock;
}
/**
 * bind
 */
int Socket::bindSocket(int sock, uint32_t uni_addr, uint16_t port)
{
  struct sockaddr_in sa;

  memset(&sa, 0, sizeof(struct sockaddr_in));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = htonl(uni_addr);

  reuseAddr(sock);
  if (bind(sock, (struct sockaddr *) &sa, sizeof(struct sockaddr_in)) < 0) {
    error("receive unicast bind: %s (%d)", strerror(errno), errno);
  }
  return sock;
}

/**
 * Create an Unicast socket
 * return fd, -1 if problem
 */
int Socket::createUcastSocket(uint32_t uni_addr, uint16_t port)
{
  int sock = -1;

  if ((sock = openDatagram()) < 0) return -1;
#if 0
  struct sockaddr_in sa;
  memset(&sa, 0, sizeof(struct sockaddr_in));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = htonl(uni_addr);

  Socket::reuseAddr(sock);
  struct sockaddr_in sa;
  if (bind(sock, (struct sockaddr *) &sa, sizeof(struct sockaddr_in)) < 0) {
    error("receive unicast bind: %s (%d)", strerror(errno), errno);
  }
#endif
  bindSocket(sock, uni_addr, port);
  return sock;
}

bool Socket::isMulticastAddress(uint32_t address)
{
  // Note: We return False for addresses in the range 224.0.0.0
  // through 224.0.0.255, because these are non-routable
  unsigned addressInHostOrder = ntohl(address);
  return addressInHostOrder > 0xE00000FF && addressInHostOrder <= 0xEFFFFFFF;
}
