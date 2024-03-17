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
#include "vreng.hpp"
#include "channel.hpp"
#include "netobj.hpp"	// NetObj
#include "socket.hpp"	// Socket
#include "nsl.hpp"	// my_gethostbyname
#include "vrep.hpp"	// SD_R_RTP
#include "payload.hpp"	// Payload
#include "session.hpp"	// Session
#include "universe.hpp"	// current
#include "world.hpp"	// current
#include "gui.hpp"	// addChannelSources
#include "pref.hpp"	// reflector
#include "sap.hpp"	// init
#include "timer.hpp"	// :g.timer
#include "stat.hpp"	// new_channel

#include <list>
using namespace std;


// local

// list
list<Channel*> Channel::channelList;

// fds
static int *tabFd;
static int *tabManagerFd;
static int cntFd;

// channels
static Channel *channel = NULL;		// current channel
static Channel *managerChannel = NULL;	// manager channel
static int16_t channelCount = 0;	// channel count

static char vrum_addr[16];


/** Finds the VRUM address and check if the MBone works */
void Channel::initReflector()
{
  // resolves reflector address
  struct hostent *hp;
  if ((hp = my_gethostbyname(DEF_VRUM_SERVER, AF_INET)) != NULL) {
    strcpy(vrum_addr, inet4_ntop(hp->h_addr_list[0]));
    my_free_hostent(hp);
  }
  if (::g.pref.reflector) Sap::init();
}

/** Network Initialization */
void Channel::init()
{
  static bool first = true;

  if (first) {
    ::g.timer.net.start();
    initReflector();
    first = false;
  }
  Channel::clearList();
  Session::clearList();
  NetObj::clearList();
}

/** Creates a new Channel */
Channel::Channel()
{
  new_channel++;
  ssrc = 0;
  group = 0;
  port = 0;
  ttl = 0;
  for (int i=0; i<5; i++)  sd[i] = -1;
  session = NULL;

  channelList.push_back(this);
}

/** Clears list */
void Channel::clearList()
{
  channelList.clear();
}

/** Joins group */
int Channel::joinGroup(int sd)
{
  if (strcmp(::g.server, "localhost")) {
    memset(&mreq, 0, sizeof(struct ip_mreq));
    mreq.imr_multiaddr.s_addr = group;
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (Socket::addMembership(sd, (void *) &mreq) < 0) {
      error("addMembership");
      return -1;
    }
  }
  return sd;
}

/** Leaves group */
int Channel::leaveGroup(int sd)
{
  if (strcmp(::g.server, "localhost")) {
    if (Socket::dropMembership(sd, (void *) &mreq) < 0)
      return -1;
  }
  return sd;
}

/** Creates a Multicast listen socket on the channel defined by group/port */
int Channel::createMcastRecvSocket(struct sockaddr_in *sa)
{
  int sd = -1;

  if ((sd = Socket::openDatagram()) < 0)  return -1;

  if (Socket::reuseAddr(sd) < 0)
    perror("reuse failed");
  if (bind(sd, (struct sockaddr *) sa, sizeof(struct sockaddr_in)) < 0)
    error("bind: %s port=%d", strerror(errno), ntohs(sa->sin_port));

  joinGroup(sd);
  return sd;
}

/** Closes a multicast socket */
void Channel::closeMcastSocket()
{
  leaveGroup(sd[SD_R_RTP]);
  leaveGroup(sd[SD_R_RTCP]);

  Socket::closeDatagram(sd[SD_R_RTP]);
  sd[SD_R_RTP] = -1;
  Socket::closeDatagram(sd[SD_W_RTP]);
  sd[SD_W_RTP] = -1;
  Socket::closeDatagram(sd[SD_R_RTCP]);
  sd[SD_R_RTCP] = -1;
  Socket::closeDatagram(sd[SD_W_RTCP]);
  sd[SD_W_RTCP] = -1;
}

/** Gets current channel */
Channel * Channel::current()
{
  return channel;
}

/** Decodes the string format "group[/port[/ttl]]" */
void Channel::decodeChan(const char *chan_str, uint32_t *group, uint16_t *port, uint8_t *ttl)
{
  char *ttlstr, *portstr, *groupstr;
  char *chanstr = strdup(chan_str);

  groupstr = chanstr;
  portstr = strchr(chanstr, '/');
  if (portstr != NULL) *portstr = '\0';
  *group = inet_addr(groupstr);
  if (portstr != NULL) {
    portstr++;
    ttlstr = strchr(portstr, '/');
    if (ttlstr != NULL) *ttlstr = '\0';
    *port = atoi(portstr);
    if (ttlstr != NULL) {
      *ttlstr = '\0';
      ttlstr++;
      *ttl = atoi(ttlstr);
    }
  }
  free(chanstr);
}

/** Channel naming */
void Channel::namingId()
{
  char hostname[MAXHOSTNAMELEN];
  gethostname(hostname, sizeof(hostname)-1);

  struct hostent *ph;
  if ((ph = my_gethostbyname(hostname, AF_INET)) == NULL)  return;
  struct in_addr *pa;
  if ((pa = (struct in_addr*) (ph->h_addr)) == NULL) {
    my_free_hostent(ph);
    return;
  }
  NetObj::setHost(pa->s_addr);
  my_free_hostent(ph);

  /* first packet to learn my_port_id */
  Payload pp;	// dummy payload

  pp.sendPayload(sa[SA_RTP]);	// needed for naming (port number)

#if NEEDLOOPBACK
  NetObj::setPort((uint16_t) (NetObj::getSsrc() & 0x00007FFF));
#else
  NetObj::setPort(Socket::getSrcPort(sd[SD_W_RTP]));
  if (NetObj::getPort() == 0)
    NetObj::setPort((uint16_t) (NetObj::getSsrc()) & 0x00007FFF);
#endif

  pp.sendPayload(sa[SA_RTCP]);	// needed for proxy (source port)
  NetObj::setObj(0);
}

/** Creates a Channel */
int Channel::create(const char *chan_str, int **pfds)
{
  cntfds = 0;	// number of fd

  if (! chan_str)  return 0;
  decodeChan(chan_str, &group, &port, &ttl);

  port &= ~1;	// RTP compliant: even port

  /*
   * RTP channel
   */
  memset(&sa_rtp, 0, sizeof(struct sockaddr_in));
  memset(&sa_rtp_r, 0, sizeof(struct sockaddr_in));
  sa_rtp.sin_family = AF_INET;
  sa_rtp_r.sin_family = AF_INET;
  if (::g.pref.reflector) {
    sa_rtp_r.sin_port = 0;
    sa_rtp_r.sin_addr.s_addr = htonl(INADDR_ANY);
    sa_rtp.sin_port = htons(port - VRUM_PORT_OFFSET);
    inet4_pton(vrum_addr, &sa_rtp.sin_addr.s_addr);
    sa[SA_RTP_R] = &sa_rtp_r;
  }
  else {	// native Multicast
    sa_rtp.sin_port = htons(port);
    sa_rtp.sin_addr.s_addr = group;
  }
  sa[SA_RTP] = &sa_rtp;

  if (::g.pref.reflector) {
    if ((sd[SD_R_RTP] = createMcastRecvSocket(sa[SA_RTP_R])) < 0) {
      error("can't open receive socket RTP reflector");
      return 0;
    }
  }
  else {	// native Multicast
    if ((sd[SD_R_RTP] = createMcastRecvSocket(sa[SA_RTP])) < 0) {
      error("can't open receive socket RTP");
      return 0;
    }
  }

  sd[SD_W_RTP] = Socket::createSendSocket(ttl);
  cntfds += 2;
  //echo("sa[SA_RTP]=%p", sa[SA_RTP]);
  //echo("sd[SD_R_RTP]=%d, sd[SD_W_RTP]=%d", sd[SD_R_RTP], sd[SD_W_RTP]);
  //echo("SA_RTP: port=%d", sa[SA_RTP]->sin_port);

  /*
   * RTCP channel
   */
  memset(&sa_rtcp, 0, sizeof(struct sockaddr_in));
  memset(&sa_rtcp_r, 0, sizeof(struct sockaddr_in));
  sa_rtcp.sin_family = AF_INET;
  sa_rtcp_r.sin_family = AF_INET;
  if (::g.pref.reflector) {
    sa_rtcp_r.sin_port = 0;
    sa_rtcp_r.sin_addr.s_addr = htonl(INADDR_ANY);
    sa_rtcp.sin_port = htons(port - VRUM_PORT_OFFSET + 1);
    inet4_pton(vrum_addr, &sa_rtcp.sin_addr.s_addr);
    sa[SA_RTCP_R] = &sa_rtcp_r;
  }
  else {	// native Multicast
    sa_rtcp.sin_port = htons(port + 1);	// not ++port !!!
    sa_rtcp.sin_addr.s_addr = group;
  }
  sa[SA_RTCP] = &sa_rtcp;

  if (::g.pref.reflector) {
    if ((sd[SD_R_RTCP] = createMcastRecvSocket(sa[SA_RTCP_R])) < 0) {
      error("can't open receive socket RTCP reflector");
      return 0;
    }
  }
  else {	// native Multicast
    if ((sd[SD_R_RTCP] = createMcastRecvSocket(sa[SA_RTCP])) < 0) {
      error("can't open receive socket RTCP");
      return 0;
    }
  }

  sd[SD_W_RTCP] = Socket::createSendSocket(ttl);
  cntfds += 2;
  //echo("sa[SA_RTCP]=%p", sa[SA_RTCP]);
  //echo("sd[SD_R_RTCP]=%d, sd[SD_W_RTCP]=%d", sd[SD_R_RTCP], sd[SD_W_RTCP]);
  //echo("SA_RTCP: port=%d", sa[SA_RTCP]->sin_port);

  /* UDP channel */
  sa[SA_UDP] = NULL;

  /*
   * RTP session initialization
   */
  session = new Session();

  uint32_t oldssrc = 0;
  World *world = NULL;

#if 0 //dax loop in worldByGroup
  if ((world = World::worldByGroup(group)) != NULL) {
    if (world) {
      oldssrc = world->getSsrc();
    }
  }
#endif
  if ((ssrc = session->create(group, port, ttl, oldssrc)) == 0) {
    error("create Channel: can't create session");
    delete session;
    session = NULL;
    return 0;
  }

  if (this == managerChannel) {
    NetObj::setMgrSsrc(ssrc);
    session->mode = MANAGER_MODE;
  }
  else {
    NetObj::setSsrc(ssrc);
    session->mode = WORLD_MODE;

    if (world) {
      world->setGroupAdr(group);
      world->setSsrc(ssrc);
    }
    else if (World::current()) {   // !! tester existence !!!
      World::current()->setGroupAdr(group);
      World::current()->setSsrc(ssrc);
    }
  }

  namingId();

  *pfds = sd;
  cntFd += cntfds;
  return cntfds;
}

void Channel::sendBYE()
{
  if (session) {
    session->sendRTCPPacket(sa[SA_RTCP], RTCP_SR);
    session->sendRTCPPacket(sa[SA_RTCP], RTCP_BYE);
  }
}

/** Deletes from channelList */
void Channel::deleteFromList()
{
  if (this == managerChannel)  return;

  if (channelList.empty()) return;
  channelList.remove(this);
}

/** Quits a channel */
void Channel::quit()
{
  /* respect this order! */
  if (this != managerChannel) {
    sendBYE();

    if (session) delete session;	// delete Session
    session = NULL;
    closeMcastSocket();
    deleteFromList();
  }
}

/** Channel Destructor */
Channel::~Channel()
{
  del_channel++;
  quit();
  ::g.gui.removeChannelSources(WORLD_MODE);

  for (int i=0; i < cntfds && channelCount > 0; i++) { // debugged by efence
    if (tabFd[i] > 0) {
      Socket::closeDatagram(tabFd[i]);
      tabFd[i] = -1;
    }
    cntFd--;
  }
  channelCount--;
  if (channelCount < 0) channelCount = 0;
  channel = NULL;
}

/** join the channel and return the new channel string */
bool Channel::join(char *chan_str)
{
  if (! chan_str)  return false;

  channel = new Channel();
  newChanStr(chan_str);

  int cntfd = channel->create(chan_str, &tabFd);
  if (cntfd == 0)  return false;

  ::g.gui.addChannelSources(WORLD_MODE, tabFd, cntfd);
  channelCount++;
  return true;
}

/** joinManagerChannel: only for vreng client */
bool Channel::joinManager(char *manager_str, const char *chan_str)
{
  managerChannel = new Channel();

  strcpy(manager_str, chan_str);
  newChanStr(manager_str);

  int cntfd = managerChannel->create(manager_str, &tabManagerFd);
  if (cntfd == 0)  return false;

  ::g.gui.addChannelSources(MANAGER_MODE, tabManagerFd, cntfd);
  return true;
}

/** Gets a socket fd by sockaddr_in */
int Channel::getfdbysa(const struct sockaddr_in *sa, int i_fd)
{
  int ret = -2;

  if (! sa) {
    error("getfdbysa: sa NULL");
    return -1;
  }

  if (channelList.empty()) return -1;
  for (list<Channel*>::iterator it = channelList.begin(); it != channelList.end(); ++it) {
    if ((*it)->sa[SA_RTP] == sa || (*it)->sa[SA_RTCP] == sa) {
      if ((*it)->sd[i_fd] < 0) {
        ret = -1;
        break;
      }
      return((*it)->sd[i_fd]);
    }
  }
  ret = channelList.front()->sd[i_fd];
  //echo("getfdbysa: sa=%x not in table, force fd=%d", sa, ret);
  return ret;
}

int Channel::getFdSendRTP(const struct sockaddr_in *sa)
{
  return getfdbysa(sa, SD_W_RTP);
}

int Channel::getFdSendRTCP(const struct sockaddr_in *sa)
{
  return getfdbysa(sa, SD_W_RTCP);
}

/** Gets a socket address by sockaddr_in */
struct sockaddr_in * Channel::getsabysa(const struct sockaddr_in *sa, int i_sa)
{
  if (! sa) {
    error("getsabysa: sa NULL");
    return NULL;
  }

  if (channelList.empty()) return NULL;
  for (list<Channel*>::iterator it = channelList.begin(); it != channelList.end(); ++it) {
    if ((*it)->sa[SA_RTP] == sa || (*it)->sa[SA_RTCP] == sa)
      return (*it)->sa[i_sa];
  }
  return NULL;
}

struct sockaddr_in * Channel::getSaRTCP(const struct sockaddr_in *sa)
{
  return getsabysa(sa, SA_RTCP);
}

/** get a channel by sockaddr_in */
Channel * Channel::getbysa(const struct sockaddr_in *sa)
{
  if (! sa) {
    error("getbysa: sa NULL");
    return NULL;
  }
  if (channelList.empty()) return NULL;
  for (list<Channel*>::iterator it = channelList.begin(); it != channelList.end(); ++it) {
    if ((*it)->sa[SA_RTP] == sa)  return *it;
    if ((*it)->sa[SA_RTCP] == sa) return *it;
    if ((*it)->sa[SA_UDP] == sa)  return *it;
  }
  //echo("getbysa: sa=%x not in table, force curr channel", sa);
  return current();  // hack
}

/*
 * manage group/port/ttl strings
 */

/* Extracts grp_str from chan_str */
void Channel::getGroup(const char *chan_str, char *grp_str)
{
  // TODO
  // Call GNC (Group Name Cache)
  // return gncGroupByName(worldname, leasetime);

  if (! chan_str) {
    *grp_str = 0;	// empty group
    return;
  }

  char *tmpchan = strdup(chan_str);
  char *p;
  if ((p = strchr(tmpchan, '/')) == NULL) {
    *grp_str = 0;	// empty string
    free(tmpchan);
    return;
  }
  *p = '\0';	// null terminated
  if (strlen(tmpchan) < GROUP_LEN)
    strcpy(grp_str, tmpchan);
  else {
    strncpy(grp_str, tmpchan, GROUP_LEN-1);
    grp_str[GROUP_LEN] = '\0';
    error("getGroup: \"%s\" too long", tmpchan);
  }
  free(tmpchan);
}

/* Returns port value fron chan_str */
uint16_t Channel::getPort(const char *chan_str)
{
  uint16_t port;
  const char *p;

  if (! chan_str)
    port = (uint16_t) DEF_VRENG_PORT;
  else if ((p = strchr(chan_str, '/')) == NULL)
    port = (uint16_t) DEF_VRENG_PORT;
  else
    port = (uint16_t) atoi(++p);
  return port;
}

uint8_t Channel::currentTtl()
{
  return Universe::current()->ttl;
}

/* Returns ttl value fron chan_str */
uint8_t Channel::getTtl(const char *chan_str)
{
  uint8_t ttl;
  const char *p;

  if (! chan_str)
    ttl = (uint8_t) DEF_VRENG_TTL;
  else if ((p = strrchr(chan_str, '/')) == NULL)
    ttl = (uint8_t) DEF_VRENG_TTL;
  else
    ttl = (uint8_t) atoi(++p);
  return ttl;
}

/** Modifies a chan_str */
void Channel::newChanStr(char *chan_str)
{
  char *group = NULL;
  char groupstr[GROUP_LEN];

  if (! chan_str)  return;

  getGroup(chan_str, groupstr);
  group = strdup(groupstr);
  sprintf(chan_str, "%s/%u/%d", group, getPort(chan_str), currentTtl());
  if (strlen(chan_str) >= CHAN_LEN) {
    error("buildChanStr: %s too long", chan_str);
    chan_str[CHAN_LEN] = '\0';
  }
  free(group);
  return;
}
