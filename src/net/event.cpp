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
// event.cpp
//
// Events handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "event.hpp"
#include "vrep.hpp"	// VREP_*
#include "netobj.hpp"	// NetObj
#include "payload.hpp"	// Payload
#include "netprop.hpp"	// NetProp
#include "oclass.hpp"	// isValidType
#include "user.hpp"	// USER_TYPE
#include "timer.hpp"	// diffDates


/** When something is available on file-descriptor "fd" */
void netIncoming(int fd)
{
  while (1) {
    fd_set set;
    struct timeval timeout;
    
    FD_ZERO(&set);
    FD_SET(fd, &set);
    timeout.tv_sec = timeout.tv_usec = 0;
    if (select(FD_SETSIZE, &set, NULL, NULL, &timeout) == 0) {
      break;
    }
    
    //
    // read the incoming packet
    //
    struct sockaddr_in from;	// sender unicast address
    
    Payload *pp = new Payload();	// create the payload to get the data
    
    int r;
    if (( r = pp->recvPayload(fd, &from)) <= 0) {
      echo("netIncoming: from %lx", ntohl(from.sin_addr.s_addr));
      delete pp;
      return;	// read err <0 or ignore == 0
    }
    
    //
    // check if this packet belongs to VREP (VREng Protocol)
    //
    uint8_t cmd;
    if (pp->len > 0 && pp->getPayload("c", &cmd) >= 0) {
      echo("In %lx/%x l=%d c=%02x", ntohl(from.sin_addr.s_addr),ntohs(from.sin_port), pp->len, cmd);
      //
      // parse VREP
      //
      switch (cmd) {
        case VREP_CREATE: case VREP_CREATE_V1:
          pp->incomingCreate(&from);
          break;
        case VREP_DELTA: case VREP_DELTA_V1:
          pp->incomingDelta(&from);
          break;
        case VREP_QUERY: case VREP_QUERY_V1:
          pp->incomingQuery(&from);
          break;
        case VREP_DELETE: case VREP_DELETE_V1:
          pp->incomingDelete(&from);
          break;
        default:
          echo("netincoming unknown: X'%02x' fd=%d from %lx/%x (mine is %lx/%x)", cmd, fd, ntohl(from.sin_addr.s_addr), ntohs(from.sin_port), NetObj::getHost(), NetObj::getPort());
          break;
      }
      delete pp;
      return;
    }
    else {
      // empty or invalid payload
      pp->incomingUnknown(&from, r);
      echo("netincoming unknown: from %lx (%x)", ntohl(from.sin_addr.s_addr), r);
      delete pp;
      return;
    }
  } //end event loop
}


/** Checks if some responsibilities should to be taken when a timeout occurs */
int netTimeout()
{
  float refresh = DEF_REFRESH_TIMEOUT;
  
  //statKbps();
  struct timeval now;
  gettimeofday(&now, NULL);
  
  // timeout grows as log(nbsources) to keep bandwidth confined
#ifdef REDUCE_BW
  nsrc = Source::getSourcesNumber() - 1;
  nsrc = (nsrc <= 1) ? 1 : nsrc;
  refresh = DEF_REFRESH_TIMEOUT * ((1 + static_cast<float> (log((double) nsrc)) / 2));
  //echo("refresh=%.2f nsrc=%d", refresh, nsrc);
#endif
  
  //
  // for each netobj in netobjList
  //
  for (std::list<NetObj*>::iterator it = NetObj::netobjList.begin(); it != NetObj::netobjList.end(); ++it) {
    if (! OClass::isValidType((*it)->type)) {
      error("netTimout: invalid type (%d)", (*it)->type);
      return -1;
    }
    
    //
    // scan its properties
    //
    uint8_t nprop = (*it)->getProperties();
    for (int i=0; i < nprop; i++) {
      switch ((*it)->type) {
        case USER_TYPE:
          if (i < User::PROPBEGINVAR || i > User::PROPENDVAR) {
            continue;			// skip static properties
          }
      }
      NetProp *pprop = (*it)->prop + i;
      
      //
      // test if refresh timeout reached
      //
      if (pprop->responsible && Timer::diffDates(pprop->last_seen, now) > refresh) {
        // now - last_seen > refresh: we are responsible, we must refresh
        (*it)->sendDelta(i);		// publish a heartbeat
      }
      if (Timer::diffDates(pprop->assume_at, now) > 0) {
        // now > assume_at: assume's timeout on this property
        if ((*it)->isPermanent()) {	// permanent object (door, lift, button, thing, ...)
          if (pprop->version != 0) {
            // if permanent prop hasn't its initial value,
            // somebody must assume responsibility
            echo("netTimeout: (permanent) assuming responsibility for %s prop=%d unseen for %5.2fs", (*it)->noid.getNoid(), i, Timer::diffDates(pprop->last_seen, now));
            if (pprop->responsible) {
              echo("netTimeout: (permanent) should assume responsibility "
                   "of something I am responsible for");
              return -1;
            }
            // heartbeat
            (*it)->declareDelta(i);	// assume responsibility: publish my existence
          }
        }
        else {				// volatile object (user, ball, dart, bullet, sheet, icon)
          if (pprop->responsible) {
            echo("netTimeout: (volatile) should assume death of %s I am responsible for", (*it)->object->objectName());
            return -1;
          }
          echo("netTimeout: (volatile) assuming death of %s [%s] (unseen for %.2fs)", (*it)->object->objectName(), (*it)->noid.getNoid(), Timer::diffDates(pprop->last_seen, now));
          (*it)->declareDeletion();
          (*it)->requestDeletion();	// discard the dead
          // no reason to continue after a requestDeletion
          break;
        }
      }
    } // end scan properties
  } // end list

  return int( ceil(refresh * 1000) );
}
