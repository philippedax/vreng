//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// aoi.cpp
//
// Area Of Interest handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "aoi.hpp"
#include "world.hpp"	// current->getName()
#include "user.hpp"	// localuser
#include "gui.hpp"	// removeAvatar
#include "channel.hpp"	// cnt_fd
#include "netobj.hpp"	// NetObj

#include <list>
#include <vector>
using namespace std;

#define DBG_AOI DBG_14	// temporary


const OClass Aoi::oclass(AOI_TYPE, "Aoi", Aoi::creator);

// global
Aoi *currentAoi = NULL;  ///< points to current AoI where localuser is in

/**
 * Intersection with AoI object must be manage by VREng general intersection
 * procedure (see col.c).  That's why AoI does not need any
 * "whenIntersect" function.  Therefore, the procedure initAoiFuncList
 * below sets this member to NULL.  The main reason for this is that AoI
 * are objects we want to be able to go through. The only event we are
 * interested in is when a collision occurs.
 */


/* creation from file */
WO * Aoi::creator(char *l)
{
  return new Aoi(l);
}

void Aoi::defaults()
{
}

void Aoi::parser(char *l)
{
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (! l) break;
    if (! stringcmp(l, "channel")) l = parseChannel(l, chan);
  }
  end_while_parse(l);
}

void Aoi::behaviors()
{
  /* AoI are not visible */
#if !defined(AOI_VISIBLE)
  enableBehavior(UNVISIBLE);
#endif
}

void Aoi::inits()
{
  initStillObject();
}

Aoi::Aoi(char *l)
{
  parser(l);
  behaviors();
  inits();
}

/**
 * Performs action to be done while entering a new Area of Interest:
 */
void Aoi::aoiEnter()
{
  echo("avatar \"%s\" enters in %s:%s", localuser->objectName(), World::current()->getName(), this->chan);
  char * avatarname = strdup(localuser->objectName());

  aoiQuit();		// quit current World or AoI (network shutdown)

  sprintf(localuser->name.current, "%s", avatarname);
  free(avatarname);

  currentAoi = this;	// new AoI is the current one

  /* initializes network with new AoI's mcast group (cf. channel.cc) */
  Channel *pchan = new Channel();

  int *tabfd;
  if (pchan->create(currentAoi->chan, &tabfd) <= 0) {
    error("aoiEnter: create channel failed");
    delete pchan;
    return;
  }

  /* initializes local user's avatar */
  localuser->initMobileObject(0);
  localuser->netop->set(NetObj::NET_VOLATILE);

  /* publishes to other VREng processes we are there (no latency) */
  localuser->netop->declareCreation();

  /* To do here: broadcast a query on new AoI to see who is there with
   * minimal latency.  In the current solution, we discover
   * other mobile objets in the new AoI only thanks to heart beats,
   * which may be quite long for AoI uses (however it may be
   * ok when entering a World).
   */

  /* GUI stuff */
  g.gui.updateAvatar(localuser);

  /* 3D stuff */
  localuser->updatePosition();
  localuser->updateCamera(pos);
}

/**
 * Performs actions to be done while leaving an Area of Interest:
 */
void Aoi::aoiQuit()
{
  /* freeing only mobile object we're responsible for */
  for (list<WO*>::iterator it = mobileList.begin(); it != mobileList.end(); ++it) {
    /* skipping local user object (as we do not want to remove it) */
    if (*it == localuser) continue;
    /* closes GUI */
    if ((*it)->type == USER_TYPE) ::g.gui.removeAvatar(reinterpret_cast<User *>((*it)));
    /* performs some clean up */
    if ((*it)->type) delete *it;
  }
  //dax clearList(mobileList);  //FIXME! don't comment else segfault

  /* explicit declaration to network so that there is minimal latency
     on other VREng processes to see user leaving previous AoI */
  localuser->netop->declareDeletion();
  localuser->netop->deleteFromList();

  /* closes multicast channel now as we do not need it anymore */
  delete Channel::current();  // delete Channel
}

void Aoi::funcs() {}
