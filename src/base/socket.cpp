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
// socket.cpp
//
// Socket operations
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "socket.hpp"
#include "stat.hpp"	// opn_sock, cls_sock


/** Opens a datagram socket */
int Socket::openDatagram()
{
  int sd = -1;

  if ((sd = ::socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    error("socket: %s (%d)", strerror(errno), errno);
  if (sd > 0)
    opn_sock++;
  return sd;
}

/** Opens a stream socket */
int Socket::openStream()
{
  int sd = -1;

  if ((sd = ::socket(PF_INET, SOCK_STREAM, 0)) < 0)
    error("socket: %s (%d)", strerror(errno), errno);
  if (sd > 0)
    opn_sock++;
  return sd;
}

/** Closes a socket datagram */
void Socket::closeDatagram(int sd)
{
  if (sd > 0) {
    close(sd);
    cls_sock++;
  }
}

/** Closes a socket stream */
void Socket::closeStream(int sd)
{
  if (sd > 0) {
    close(sd);
    cls_sock++;
  }
}

/** Connects *sa */
int Socket::connection(int sd, const sockaddr_in *sa)
{
  if (connect(sd, reinterpret_cast<const struct sockaddr *>(sa), sizeof(struct sockaddr_in)) < 0) {
    error("connect: %s (%d)", strerror(errno), errno);
    close(sd);
    cls_sock++;
    return -1;
  }
  return sd;
}

/**
 * Do REUSEADDR on the socket
 * return sd if OK, else -1
 */
int Socket::reuseAddr(int sd)
{
  int one = 1;

  if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&one), sizeof(one)) < 0) {
    error("SO_REUSEADDR: %s (%d)", strerror(errno), errno);
    return -1;
  }
  return sd;
}

/** Sets TCP_NODELAY */
int Socket::tcpNoDelay(int sd)
{
  int one = 1;

  if (setsockopt(sd, SOL_SOCKET, TCP_NODELAY, reinterpret_cast<char *> (&one), sizeof(one)) < 0) {
    error("TCP_NODELAY: %s (%d) sock=%d", strerror(errno), errno, sd);
    return -1;
  }
  return sd;
}

/** Gets src port */
uint16_t Socket::getSrcPort(int sd)
{
  struct sockaddr_in sa;
  socklen_t slen = sizeof(struct sockaddr_in);

  sa.sin_family = AF_INET;
  sa.sin_port = 0;
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  if (getsockname(sd, reinterpret_cast<struct sockaddr *> (&sa), &slen) < 0) {
    error("getSrcPort: %s (%d)", strerror(errno), errno);
    return 0;
  }
  return ntohs(sa.sin_port);
}

/**
 * Control blocking(1)/non blocking(0)
 * return sd if OK, else -1
 */
int Socket::handleBlocking(int sd, bool block)
{
  int flags;

  if ((flags = fcntl(sd, F_GETFL)) < 0) {
    error("F_GETFL: %s (%d)", strerror(errno), errno);
    return -1;
  }
  if (block)
    flags &= ~O_NONBLOCK;
  else
    flags |= O_NONBLOCK;
  if (fcntl(sd, F_SETFL, flags) < 0) {
    error("F_SETFL: %s (%d)", strerror(errno), errno);
    return -1;
  }
  return sd;
}

int Socket::setNoBlocking(int sd)
{
  return handleBlocking(sd, false);
}

/**
 * Sets the ttl
 * return sd if OK, else -1
 */
int Socket::setScope(int sd, uint8_t ttl)
{
  if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_TTL, reinterpret_cast<const char*> (&ttl), sizeof(ttl)) < 0) {
    error("IP_MULTICAST_TTL: %s (%d)", strerror(errno), errno);
    return -1;
  }
  return sd;
}

/**
 * Sets loopback: active (1) either inactive (0)
 * return sd if OK, else -1
 */
int Socket::handleLoopback(int sd, uint8_t loop)
{
  if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0) {
    error("IP_MULTICAST_LOOP: %s (%d)", strerror(errno), errno);
    return -1;
  }
  return sd;
}

int Socket::setLoopback(int sd)
{
  return handleLoopback(sd, 1);
}

int Socket::setNoLoopback(int sd)
{
  return handleLoopback(sd, 0);
}

/** Adds IP membership */
int Socket::addMembership(int sd, const void *pmreq)
{
  if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, pmreq, sizeof(struct ip_mreq)) < 0) {
    error("IP_ADD_MEMBERSHIP: %s (%d)", strerror(errno), errno);
    return -1;
  }
  return 0;
}

/** Drops IP membership */
int Socket::dropMembership(int sd, const void *pmreq)
{
  if (setsockopt(sd, IPPROTO_IP, IP_DROP_MEMBERSHIP, pmreq, sizeof(struct ip_mreq)) < 0) {
    error("IP_DROP_MEMBERSHIP: %s (%d)", strerror(errno), errno);
    return -1;
  }
  return 0;
}

/** Sets an IP Multicast socket */
void Socket::setSendSocket(int sd, uint8_t ttl)
{
  setScope(sd, ttl);
  setNoLoopback(sd);	// no loopback (default)
  setNoBlocking(sd);
}

/**
 * Sends an UDP socket
 * Prevue pour emettre (uni et mcast) et recevoir (unicast donc)
 * Do setScope (ttl) and setLoopback (off)
 * return sd if OK, else -1
 */
int Socket::sendSocket(uint8_t ttl)
{
  int sd = -1;

  if ((sd = openDatagram()) >= 0)
    setSendSocket(sd, ttl);
  return sd;
}

/**
 * Binds
 */
int Socket::bindSocket(int sd, uint32_t uni_addr, uint16_t port)
{
  struct sockaddr_in sa;

  memset(&sa, 0, sizeof(struct sockaddr_in));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = htonl(uni_addr);

  reuseAddr(sd);
  if (::bind(sd, reinterpret_cast<struct sockaddr *> (&sa), sizeof(struct sockaddr_in)) < 0) {
    error("receive unicast bind: %s (%d)", strerror(errno), errno);
  }
  return sd;
}

/**
 * Creates an Unicast socket
 * return fd, -1 if problem
 */
int Socket::ucastSocket(uint32_t uni_addr, uint16_t port)
{
  int sd = -1;

  if ((sd = openDatagram()) < 0) return -1;
  bindSocket(sd, uni_addr, port);
  return sd;
}

#if 0 //notused
bool Socket::isMulticastAddress(uint32_t address)
{
  // Note: We return False for addresses in the
  // range 224.0.0.0 through 224.0.0.255,
  // because these are non-routable
  unsigned addressInHostOrder = ntohl(address);
  return addressInHostOrder > 0xE00000FF && addressInHostOrder <= 0xEFFFFFFF;
}
#endif //notused
