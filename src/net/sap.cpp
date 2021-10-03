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
#include "sap.hpp"
#include "socket.hpp"
#include "nsl.hpp"	// inet4_pton
#include "channel.hpp"	// switchChannel
#include "pref.hpp"	// reflector

/**
 * SAP packet
 */
typedef struct {
  uint32_t hdr;
  uint32_t src;
  char data[1];
} sapPkt_t;


#if IPMC_ENABLED

void Sap::finish(int status)
{
#if HAVE_LIBPTHREAD
  pthread_exit(NULL);
#else
  exit(status);
#endif
}

/** SAP/SDR listener: MBone detection */
void * Sap::listener(void *arg)
{
  int sd;
  struct sockaddr_in sasap, sasrc;

  if ((sd = Socket::openDatagram()) < 0)
    finish(1);

  sasap.sin_family = AF_INET;
  sasap.sin_addr.s_addr = htonl(INADDR_ANY);
  sasap.sin_port = htons(DEF_SAP_PORT);

  if (Socket::reuseAddr(sd) < 0)
    finish(1);
  if (bind(sd, (struct sockaddr *) &sasap, sizeof(sasap)) < 0) {
    perror("bind sap");
    finish(1);
  }

  struct ip_mreq mreq;
  memset(&mreq, 0, sizeof(mreq));
  inet4_pton(DEF_SAP_ADDR, &(mreq.imr_multiaddr.s_addr));
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  if (Socket::addMembership(sd, &mreq) < 0) {
    error("sapListener: can't membership");
    finish(1);
  }
  Socket::setNoLoopback(sd);

  socklen_t len = sizeof(struct sockaddr_in);
  char bufsap[BUFSIZ];
  memset(bufsap, 0, BUFSIZ);

  recvfrom(sd, bufsap, sizeof(bufsap), 0, (struct sockaddr *) &sasrc, &len);
  for (int i=8; i<12; i++)
    printf("%c", bufsap[i]);	// print SDP version v=

#if HAVE_LIBPTHREAD
  ::g.pref.reflector = false;	// no reflector needed, we are on the MBone
  //pd Channel::switchChannel();
#else
  kill(getppid(), SIGUSR1);
#endif

  finish(0);
  return (void *) NULL;
}

#if !defined(HAVE_LIBPTHREAD)
void Sap::here(int sig)
{
  //trace(DBG_FORCE, "sapHere: sig=%d", sig);
  signal(SIGUSR1, SIG_DFL);
  ::g.pref.reflector = false;	// no reflector needed, we are on the MBone
  //pd Channel::switchChannel();
}
#endif

#endif

/** Sap init: called by initNetwork */
void Sap::init()
{
#if IPMC_ENABLED
#if HAVE_LIBPTHREAD
  pthread_t tid;
  pthread_create(&tid, NULL, listener, (void *) NULL);
#else
  signal(SIGUSR1, here);
  if (! fork())
    listener((void *) NULL);
#endif
#endif
}
