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
#include "event.hpp"
#include "vrep.hpp"	// VREP_*
#include "netobj.hpp"	// NetObject
#include "payload.hpp"	// Payload
#include "netprop.hpp"	// NetProperty
#include "oclass.hpp"	// isValidType
#include "user.hpp"	// USER_TYPE
#include "timer.hpp"	// diffDates


/* When something is available on file-descriptor "fd" */
void NetIncoming(int fd)
{
  while (1) {
    fd_set set;
    struct timeval timeout;
    
    FD_ZERO(&set);
    FD_SET(fd, &set);
    timeout.tv_sec = timeout.tv_usec = 0;
    if (select(FD_SETSIZE, &set, NULL, NULL, &timeout) == 0)
      break;
    
    /*
     * read the packet
     */
    struct sockaddr_in from;	// sender unicast address
    int r;
    
    Payload *pp = new Payload();	// create the payload to get the data
    
    if ((r = pp->recvPayload(fd, &from)) <= 0) {
      //error("NetIncoming: from %lx", ntohl(from.sin_addr.s_addr));
      delete pp;
      return;	// read error <0 or ignore == 0
    }
    
    /*
     * check if this packet belongs to VREP (VREng Protocol)
     */
    uint8_t cmd;
    if (pp->len > 0 && pp->getPayload("c", &cmd) >= 0) {
#if 1 //debug
      trace(DBG_FORCE, "Incoming from %lx/%x len=%d cmd=%02x",
            ntohl(from.sin_addr.s_addr), ntohs(from.sin_port),
            pp->len, cmd);
#endif
      //
      // parse VREP
      //
      switch (cmd) {
        case VREP_CREATE: case VREP_CREATE_V1:
          pp->incomingCreate(&from);
          delete pp;
          return;
        case VREP_DELTA: case VREP_DELTA_V1:
          pp->incomingDelta(&from);
          delete pp;
          return;
        case VREP_QUERY: case VREP_QUERY_V1:
          pp->incomingQuery(&from);
          delete pp;
          return;
        case VREP_DELETE: case VREP_DELETE_V1:
          pp->incomingDelete(&from);
          delete pp;
          return;
        default:
          error("Incoming unknown: X'%02x' fd=%d from %lx/%x (mine is %lx/%x)",
                cmd, fd, ntohl(from.sin_addr.s_addr), ntohs(from.sin_port),
                NetObject::getMyHostId(), NetObject::getMyPortId());
          delete pp;
          return;
      }
    }
    else {
      // empty or invalid payload
      pp->incomingOther(&from, r);
      error("NetIncoming other= %lx (%x)", ntohl(from.sin_addr.s_addr), r);
      delete pp;
      return;
    }
  } //end infinite loop
}


/* Check if some responsibilities should to be taken when a timeout occurs */
int NetTimeout()
{
  float refresh = DEF_REFRESH_TIMEOUT;
  
  //statAdjust();
  struct timeval now;
  gettimeofday(&now, NULL);
  
  /* timeout grows as log(nbsources) to keep bandwidth confined */
#ifdef REDUCE_BW
  nsrc = Source::getSourcesNumber() - 1;
  nsrc = (nsrc <= 1) ? 1 : nsrc;
  refresh = DEF_REFRESH_TIMEOUT * ((1.0 + (float) log((double) nsrc)) / 2);
  error("refresh=%.2f nsrc=%d", refresh, nsrc);
#endif
  
  /*
   * for each netobject in netobjectlist
   */
  for (list<NetObject*>::iterator it = NetObject::netobjectList.begin(); it != NetObject::netobjectList.end(); ++it) {
    if (! OClass::isValidType((*it)->type)) {
      error("NetTimout: invalid type (%d)", (*it)->type); return -1;
    }
    
    /*
     * scan its properties
     */
    uint8_t nprop = (*it)->getPropertiesNumber();
    for (int i=0; i < nprop; i++) {
      switch ((*it)->type) {
        case USER_TYPE:
          if (i < User::PROPBEGINVAR || i > User::PROPENDVAR)
            //trace(DBG_NET, "skip property %d", i); // skip static properties
            continue;
      }
      NetProperty *pprop = (*it)->netprop + i;
      
      /*
       * test if refresh timeout reached
       */
      if (pprop->responsible 
          && Timer::diffDates(pprop->last_seen, now) > refresh) {
        // now - last_seen > refresh: we are responsible, we must refresh
        
        (*it)->sendDelta(i);	// publish a heartbeat
      }
      if (Timer::diffDates(pprop->assume_at, now) > 0) {
        // now > assume_at: assume's timeout on this property
        if ((*it)->permanent) { // permanent object (door, lift, anim, button, thing, book)
          if (pprop->version != 0) {
            // if permanent prop hasn't its initial value,
            // somebody must assume responsibility
            trace(DBG_FORCE, "NetTimeout: (permanent) Assuming responsibility for %s prop=%d unseen for %5.2fs",
                  (*it)->noid.getNetNameById(), i,Timer::diffDates(pprop->last_seen, now));
            if (pprop->responsible) {
              trace(DBG_FORCE, "NetTimeout: (permanent) should assume responsibility "
                    "of something I am responsible for");
              return -1;
            }
            /* heartbeat */
            (*it)->declareObjDelta(i);	// assume responsibility: publish
          }
        }
        else { // volatile object (user, ball, dart, bullet, sheet, icon)
          if (pprop->responsible) {
            trace(DBG_FORCE, "NetTimeout: (volatile) should assume death of %s I am responsible for", (*it)->pobject->getInstance());
            return -1;
          }
          trace(DBG_NET, "NetTimeout: (volatile) Assuming death of %s [%s] (unseen for %.2fs)",
                (*it)->pobject->getInstance(), (*it)->noid.getNetNameById(), 
                Timer::diffDates(pprop->last_seen, now));
          (*it)->declareDeletion();
          (*it)->requestDeletionFromNetwork();	// discard the dead
          // no reason to continue after a requestDeletion
          break;
        }
      }
    } // end scan properties
  } // end list

  return (int) ceil(refresh * 1000);
}
