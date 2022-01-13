//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
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
#include "humanoid.hpp"
#include "world.hpp"	// current
#include "body.hpp"	// Body
#include "face.hpp"	// Face
#include "channel.hpp"	// getGroup
#include "socket.hpp"	// openStream
#include "nsl.hpp"	// get_mygethostbyname
#include "pref.hpp"	// reflector
#include "timer.hpp"	// rate
#include "user.hpp"	// localuser


const OClass Humanoid::oclass(HUMANOID_TYPE, "Humanoid", Humanoid::creator);

//local
static const char DEF_BODY_URL[] = "/avatars/humanoid/humanoid.parts";
static float skin[] = {1, .75, .7}; // skin color
static uint8_t vaps_offset_port = Humanoid::VAPS_OFFSET_PORT;


/** create from a fileline */
WObject * Humanoid::creator(char *l)
{
  return new Humanoid(l);
}

void Humanoid::defaults()
{
  face_url = new char[URL_LEN];
  memset(face_url, 0, URL_LEN);
  strcpy(vaps, DEF_VAPS_SERVER);   // default server
  strcpy(names.url, DEF_BODY_URL); // default body

  for (int i=0; i<3; i++) cloth[i] = skin[i];
}

void Humanoid::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (! stringcmp(l, "body="))   l = parse()->parseString(l, names.url, "body"); //body
    else if (! stringcmp(l, "face="))   l = parse()->parseString(l, face_url, "face");  //face
    else if (! stringcmp(l, "color="))  l = parse()->parseVector3f(l, cloth, "color");  //color
    else if (! stringcmp(l, "server=")) l = parse()->parseString(l, vaps, "server");    //server
  }
  end_while_parse(l);
}

void Humanoid::makeSolid()
{
  char s[256];

  sprintf(s, "solid shape=\"bbox\" dim=\"%f %f %f\" />",.24,.14,B_HEIGHT);
  parse()->parseSolid(s, SEP, this);
}

void Humanoid::behaviour()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);

  initMobileObject(0);
  enablePermanentMovement();  // bap/fap frames
}

void Humanoid::inits()
{
  sdudp = -1;
  sdtcp = -1;

  body = new Body();
  body->wobject = this;

  bap = new Bap();
  body->bap = bap;
  body->setColors(skin, cloth);
  body->load(names.url);
  body->draw();

  if (*face_url) {
    body->face = new Face();
    body->face->load(face_url);
  }
  if (face_url) delete[] face_url;

  vaps_offset_port += 2;
  vaps_port = Channel::getPort(World::current()->getChan()) + vaps_offset_port;
  if (! ::g.pref.reflector) reset();
  state = INACTIVE;
}

/* Comes from file */
Humanoid::Humanoid(char *l)
{
  parser(l);
  makeSolid();
  behaviour();
  inits();
  usercontrol = false;
}

/* Called from User */
Humanoid::Humanoid()
{
  defaults();
  makeSolid();
  behaviour();
  inits();
  usercontrol = true;
}

void Humanoid::render()
{
  body->render(pos);
}

/** init receiver in UDP mode : returns 1 if OK else 0 if fails */
int Humanoid::initReceiver()
{
  if (sdudp > 0) disconnectFromBapServer();

  if ((sdudp = Socket::openDatagram()) < 0) {
    //error("initReceiver: open UDP failed");
    return 0;
  }
  Socket::bindSocket(sdudp, INADDR_ANY, vaps_port);
#if 0 //dax
  if (ipmode == MULTICAST) {
    char group[GROUP_LEN];
    Channel::getGroup(World::current()->getChan(), group);
    inet4_pton(group, &(udpsa.sin_addr.s_addr));
    memset(&mreq, 0, sizeof(mreq));
    inet4_pton(group, &(mreq.imr_multiaddr.s_addr));
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (Socket::addMembership(sdudp, &mreq) < 0) {
      error("initReceiver: cannot join multicast group %s", group); return 0;
    }
    //error("initReceiver: waiting for Multicast on %x:%d", udpsa.sin_addr.s_addr, udpsa.sin_port);
  }
  else {  // Unicast
    //error("initReceiver: waiting for Unicast on port %d", vaps_port);
  }
#endif
  //error("initReceiver: waiting on port %d, sdudp=%d", vaps_port, sdudp);
  return 1;
}

/** Establishes a TCP connection and send the setup packet */
int Humanoid::connectToBapServer(int _ipmode)
{
  char setup_cmd[128];
  struct hostent *hp;

  if ((sdtcp = Socket::openStream()) < 0) {
    error("socket vaps failed");
    return 0;
  }
  if ((hp = my_gethostbyname(vaps, AF_INET)) == NULL) {
    error("can't resolve name");
    //sdtcp = -1;
    return 0;
  }
  memset(&tcpsa, 0, sizeof(struct sockaddr_in));
  tcpsa.sin_family = AF_INET;
  tcpsa.sin_port = htons(VAPS_PORT);
  memcpy(&tcpsa.sin_addr, hp->h_addr_list[0], hp->h_length);
  my_free_hostent(hp);

  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;

  if (setsockopt(sdtcp, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    error("setsockopt failed\n");
  if (connect(sdtcp, (const struct sockaddr *) &tcpsa, sizeof(tcpsa)) < 0) {
    error("Connection failed with the vaps server: %s (%s)", vaps, inet4_ntop(&tcpsa.sin_addr));
    sdtcp = -1;
    return 0;
  }
  //error("Connection established with vaps server: %s(%s)", vaps, inet4_ntop(&tcpsa.sin_addr));
#if 0 //dax
  ipmode = _ipmode;
  if (ipmode == MULTICAST) {
    char group[GROUP_LEN];
    Channel::getGroup(World::current()->getChan(), group);
    sprintf(setup_cmd, "setup a=%s p=%d t=%d r=%.2f ",
            group, vaps_port, Channel::getTtl(World::current()->getChan()), ::g.timer.rate());
  }
  else
#endif
  sprintf(setup_cmd, "setup p=%d r=%.2f ", vaps_port, ::g.timer.rate());
  write(sdtcp, setup_cmd, strlen(setup_cmd));  // setup packet
  return sdtcp;
}

void Humanoid::disconnectFromBapServer()
{
  if (sdtcp > 0) {
    Socket::closeStream(sdtcp);
    if (ipmode == MULTICAST)
      Socket::dropMembership(sdudp, &mreq);
  }
  if (sdudp > 0) Socket::closeDatagram(sdudp);
  //error("Connection closed with the vaps server");
  sdtcp = sdudp = -1;
  state = INACTIVE;
}

/** get a frame from the vaps server */
int Humanoid::readBapFrame()
{
  fd_set set;
  FD_ZERO(&set);
  FD_SET(sdudp, &set);
  struct timeval delay;
  delay.tv_sec = delay.tv_usec = 0;  // set select passing

  if (select(FD_SETSIZE, &set, NULL, NULL, &delay) == 0) {
    //error("select=0 sdudp=%d", sdudp);
    return 0;  // nothing to read
  }

  memset(bapline, 0, VAPS_BUFSIZ);
  socklen_t slen = sizeof(struct sockaddr_in);
  int len;
  // receive Bap line
  if ((len = recvfrom(sdudp, bapline, VAPS_BUFSIZ, 0, (struct sockaddr *)&udpsa, &slen)) < 0) {
    error("error recvfrom");
    return 0;
  }
  if (! len) {
    disconnectFromBapServer();  // send stop
#if 0 //pdlastbap: memorize last-bapline for persistency
    char last-bapline[BUFSIZ];
    strcpy(last-bapline, bapline);
#endif
    return 0; // eof
  }
  return 1; // bap is present
}

/** system of equations handling permanent motion */
void Humanoid::changePermanent(float lasting)
{
  if (state != PLAYING) return;  // not playing

  if (usercontrol && localuser) {
    pos.x = localuser->pos.x;
    pos.y = localuser->pos.y;
    pos.z = localuser->pos.z - localuser->height / 2;
    pos.az = localuser->pos.az - M_PI_2;
    pos.ax = localuser->pos.ax;
    pos.ay = localuser->pos.ay;
    updatePosition();
  }

  if ((sdtcp > 0) && readBapFrame()) {		// from vaps server
    // bap is ready to be played
    //error("bapline: %s", bapline);
    uint8_t baptype = bap->parse(bapline);	// parse it

    switch (baptype) {
    case TYPE_BAP_V31: case TYPE_BAP_V32: 
      body->animate();		// play bap
      break;
    case TYPE_FAP_V20: case TYPE_FAP_V21:
      for (int i=1; i <= NUM_FAPS; i++) {
        if (bap->is(i) && bap->getFap(i) && body->face)
          body->face->animate(i, (int) bap->getFap(i)); // play fap
      }
      break;
    default:
      break;
    //TODO: predictive interpollation
    }
  }
  else if ((sdtcp > 0) && body->face) {
    body->face->animate();	// local animation
  }
  else {
    uint8_t baptype = bap->parse(bapline);	// parse it
    //error("bapline: %s", bapline);
    switch (baptype) {
    case TYPE_BAP_V31: case TYPE_BAP_V32: 
      body->animate();		// play bap
      break;
    case TYPE_FAP_V20: case TYPE_FAP_V21:
      for (int i=1; i <= NUM_FAPS; i++) {
        if (bap->is(i) && bap->getFap(i) && body->face)
          body->face->animate(i, (int) bap->getFap(i)); // play fap
      }
      break;
    default:
      break;
    }
  }
}

/** send a play command to the vaps server */
bool Humanoid::sendPlayToBapServer(const char *bap_name)
{
  char play_cmd[128];

  if (sdtcp <= 0) {
    if (! initReceiver()) { // UDP reception
      error("sendPlayToBapServer: receiver fails");
      return false;
    }
    if (! connectToBapServer(UNICAST)) { // opens a TCP connection
      error("sendPlayToBapServer: connect fails");
      return false;
    }
  }
  sprintf(play_cmd, "play f=%s", bap_name);
  write(sdtcp, play_cmd, strlen(play_cmd));  // TCP play packet
  state = PLAYING;
  return true;
}

void Humanoid::quit()
{
  disconnectFromBapServer();
  if (bap) delete bap;
  bap = NULL;
  if (body) delete body;
  body = NULL;
}

char * Humanoid::toPlay(const char *str)
{
  if (connectToBapServer(UNICAST)) { // opens a TCP connection
    sendPlayToBapServer(str);
    return NULL;
  }
  else {
    char *newstr = strdup(str);
    char *p = newstr;
    p = strchr(newstr, '.');
    if (p)
      *p = '_';
    strcpy(bapline, newstr);
    free(newstr);
    state = PLAYING;
    return bapline;
  }
}

void Humanoid::reset()
{
  disconnectFromBapServer();  // first disconnect
  if (! initReceiver()) return;
  state = LISTENING;
  if (! connectToBapServer(MULTICAST)) return;
}

void Humanoid::pause()
{
  sendPlayToBapServer("pause.bap");
  strcpy(bapline, pause_bap);
}

void Humanoid::hi()
{
  sendPlayToBapServer("hi.bap");
  strcpy(bapline, hi_bap);
}

void Humanoid::bye()
{
  sendPlayToBapServer("bye.bap");
  strcpy(bapline, bye_bap);
}

void Humanoid::ask()
{
  sendPlayToBapServer("ask.bap");
  strcpy(bapline, ask_bap);
}

void Humanoid::sit()
{
  sendPlayToBapServer("sit.bap");
  strcpy(bapline, sit_bap);
}

void Humanoid::show()
{
  sendPlayToBapServer("show.bap");
  strcpy(bapline, show_bap);
}

void Humanoid::clap()
{
  sendPlayToBapServer("clap.bap");
  strcpy(bapline, clap_bap);
}

void Humanoid::nak()
{
  sendPlayToBapServer("nak.bap");
  strcpy(bapline, nak_bap);
}

void Humanoid::test()
{
  sendPlayToBapServer("test.bap");
  strcpy(bapline, test_bap);
}

void Humanoid::eyes()
{
  sendPlayToBapServer("eyes.fap");
  strcpy(bapline, eyes_fap);
}

void Humanoid::joy()
{
  sendPlayToBapServer("joy.fap");
  strcpy(bapline, joy_fap);
}

void Humanoid::sad()
{
  sendPlayToBapServer("sad.fap");
  strcpy(bapline, sad_fap);
}

void Humanoid::surp()
{
  sendPlayToBapServer("surp.fap");
  strcpy(bapline, surp_fap);
}

void Humanoid::jag()
{
  sendPlayToBapServer("jag.fap");
}

void Humanoid::pause_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->pause();
}

void Humanoid::hi_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->hi();
}

void Humanoid::bye_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->bye();
}

void Humanoid::ask_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->ask();
}

void Humanoid::sit_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->sit();
}

void Humanoid::show_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->show();
}

void Humanoid::clap_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->clap();
}

void Humanoid::nak_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->nak();
}

void Humanoid::test_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->test();
}

void Humanoid::reset_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->reset();
}

void Humanoid::eyes_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->eyes();
}

void Humanoid::joy_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->joy();
}

void Humanoid::sad_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->sad();
}

void Humanoid::surp_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->surp();
}

void Humanoid::jag_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->jag();
}

/** functions initialization */
void Humanoid::funcs()
{
  setActionFunc(HUMANOID_TYPE, 0, _Action pause_cb, "Pause");
  setActionFunc(HUMANOID_TYPE, 1, _Action hi_cb, "Hi");
  setActionFunc(HUMANOID_TYPE, 2, _Action bye_cb, "Bye");
  setActionFunc(HUMANOID_TYPE, 3, _Action ask_cb, "Ask");
  setActionFunc(HUMANOID_TYPE, 4, _Action sit_cb, "Sit");
  setActionFunc(HUMANOID_TYPE, 5, _Action show_cb, "Show");
  setActionFunc(HUMANOID_TYPE, 6, _Action clap_cb, "Clap");
  setActionFunc(HUMANOID_TYPE, 7, _Action nak_cb, "Nak");
  setActionFunc(HUMANOID_TYPE, 8, _Action test_cb, "Test");
  setActionFunc(HUMANOID_TYPE, 9, _Action eyes_cb, "Eyes");
  setActionFunc(HUMANOID_TYPE, 10, _Action joy_cb, "Joy");
  setActionFunc(HUMANOID_TYPE, 11, _Action sad_cb, "Sad");
  setActionFunc(HUMANOID_TYPE, 12, _Action surp_cb, "Surp");
  setActionFunc(HUMANOID_TYPE, 13, _Action jag_cb, "Jag");
  setActionFunc(HUMANOID_TYPE, 14, _Action reset_cb, "Reset");
  setActionFunc(HUMANOID_TYPE, 15, _Action Face::changeFace, "New");
  setActionFunc(HUMANOID_TYPE, 16, _Action Face::changeMoveYes, "Yes");
  setActionFunc(HUMANOID_TYPE, 17, _Action Face::changeMoveNo, "No");
  setActionFunc(HUMANOID_TYPE, 18, _Action Face::changeMoveEyeL, "EyeL");
  setActionFunc(HUMANOID_TYPE, 19, _Action Face::changeMoveMouth, "Mouth");
}
