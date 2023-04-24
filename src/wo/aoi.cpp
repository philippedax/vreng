//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
#include "aoi.hpp"
#include "world.hpp"	// current->getName()
#include "user.hpp"	// localuser
#include "gui.hpp"	// removeUser
#include "channel.hpp"	// cnt_fd
#include "netobj.hpp"	// NetObject

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
WObject * Aoi::creator(char *l)
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
  trace(DBG_WO, "aoiEnter: world=%s:%s worlds=%p", World::current()->getName(), this->chan, World::current());
  echo("Avatar \"%s\" enters in %s:%s", localuser->getInstance(), World::current()->getName(), this->chan);
  char * avatarname = strdup(localuser->getInstance());

  /* quit current World or AoI (network shutdown) */
  aoiQuit();

  sprintf(localuser->names.instance, "%s", avatarname);
  free(avatarname);

  /* new AoI is the current one */
  currentAoi = this;

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
  localuser->netop->create(NetObject::NET_VOLATILE);

  /* publishes to other VREng processes we are there (no latency) */
  localuser->netop->declareObjCreation();

  /* To do here: broadcast a query on new AoI to see who is there with
   * minimal latency.  In the current solution, we discover
   * other mobile objets in the new AoI only thanks to heart beats,
   * which may be quite long for AoI uses (however it may be
   * ok when entering a World).
   */

  /* GUI stuff */
  g.gui.updateUser(localuser);

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
  for (list<WObject*>::iterator it = mobileList.begin(); it != mobileList.end(); ++it) {
    /* skipping local user object (as we do not want to remove it) */
    if (*it == localuser) continue;
    /* closes GUI */
    if ((*it)->type == USER_TYPE) ::g.gui.removeUser((User *)(*it));
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
