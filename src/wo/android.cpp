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
#include "android.hpp"
#include "world.hpp"	// current
#include "body.hpp"	// Body
#include "face.hpp"	// Face
#include "channel.hpp"	// getGroup
#include "socket.hpp"	// openStream
#include "nsl.hpp"	// get_mygethostbyname
#include "pref.hpp"	// reflector
#include "timer.hpp"	// getRate
#include "user.hpp"	// localuser


const OClass Android::oclass(ANDROID_TYPE, "Android", Android::creator);

static uint8_t vaps_offset_port = Android::VAPS_OFFSET_PORT;

//local
static float skin[] = {1, .75, .7}; // skin color
const char DEF_BODY_URL[] = "/avatars/android/android.parts";


/** create from a fileline */
WObject * Android::creator(char *l)
{
  return new Android(l);
}

void Android::defaults()
{
  face_url = new char[URL_LEN];
  memset(face_url, 0, URL_LEN);
  strcpy(vaps, DEF_VAPS_SERVER);   // default server
  strcpy(names.url, DEF_BODY_URL); // default body

  for (int i=0; i<3; i++) cloth[i] = skin[i];
  sdudp = -1;
  state = INACTIVE;
}

void Android::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (!stringcmp(l, "body="))   l = parse()->parseString(l, names.url, "body"); //body
    else if (!stringcmp(l, "face="))   l = parse()->parseString(l, face_url, "face");  //face
    else if (!stringcmp(l, "color="))  l = parse()->parseVector3f(l, cloth, "color");  //color
    else if (!stringcmp(l, "server=")) l = parse()->parseString(l, vaps, "server");    //server
  }
  end_while_parse(l);
}

void Android::makeSolid()
{
  char s[256];
  sprintf(s, "solid shape=\"bbox\" dim=\"%f %f %f\" />",.24,.14,B_HEIGHT);
  parse()->parseSolid(s, SEP, this);
}

void Android::behaviour()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);
  setRenderPrior(RENDER_HIGH);	// NORMAL doesn't work

  initializeMobileObject(0);
  enablePermanentMovement();  // bap/fap frames
}

void Android::inits()
{
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
}

/* Comes from file */
Android::Android(char *l)
{
  parser(l);
  makeSolid();
  behaviour();
  inits();
  usercontrol = false;
}

/* Called from User */
Android::Android()
{
  defaults();
  makeSolid();
  behaviour();
  inits();
  usercontrol = true;
}

void Android::render()
{
  body->render(pos);
}

/** init receiver in UDP mode : returns 1 if OK else 0 if fails */
int Android::initReceiver()
{
  if (sdudp > 0) disconnectFromBapServer();

  if ((sdudp = Socket::openDatagram()) < 0) { error("open UDP failed"); return 0; }
  Socket::bindSocket(sdudp, INADDR_ANY, vaps_port);
#if 0 //dax
  if (ipmode == MULTICAST) {
    char group[GROUP_LEN];
    Channel::getGroup(World::current()->getChan(), group);
    inet4_pton(group, &(udpsa.sin_addr.s_addr));
  }
  switch (ipmode) {
  case MULTICAST:
    memset(&mreq, 0, sizeof(mreq));
    inet4_pton(group, &(mreq.imr_multiaddr.s_addr));
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (Socket::addMembership(sdudp, &mreq) < 0) {
      error("initReceiver: cannot join multicast group %s", group); return 0;
    }
    //error("initReceiver: waiting for Multicast on %x:%d", udpsa.sin_addr.s_addr, udpsa.sin_port);
    break;
  default:  // Unicast
    //error("initReceiver: waiting for Unicast on port %d", vaps_port);
    break;
  }
#endif
  error("initReceiver: waiting on port %d, sdudp=%d", vaps_port, sdudp);
  return 1;
}

/** Establishes a TCP connection and send the setup packet */
int Android::connectToBapServer(int _ipmode)
{
  char setup_cmd[128];
  struct hostent *hp;

  if ((sdtcp = Socket::openStream()) < 0) { error("socket vaps failed"); return 0; }
  if ((hp = my_gethostbyname(vaps, AF_INET)) == NULL) { error("can't resolve name"); return 0; }
  memset(&tcpsa, 0, sizeof(struct sockaddr_in));
  tcpsa.sin_family = AF_INET;
  tcpsa.sin_port = htons(VAPS_PORT);
  memcpy(&tcpsa.sin_addr, hp->h_addr_list[0], hp->h_length);
  my_free_hostent(hp);
  if (connect(sdtcp, (const struct sockaddr *) &tcpsa, sizeof(tcpsa)) < 0) {
    error("Connection failed with the vaps server: %s (%s)", vaps, inet4_ntop(&tcpsa.sin_addr));
    return 0;
  }
  //error("Connection established with vaps server: %s(%s)", vaps, inet4_ntop(&tcpsa.sin_addr));
#if 0 //dax
  ipmode = _ipmode;
  if (ipmode == MULTICAST) {
    char group[GROUP_LEN];
    Channel::getGroup(World::current()->getChan(), group);
    sprintf(setup_cmd, "setup a=%s p=%d t=%d r=%.2f ",
            group, vaps_port, Channel::getTtl(World::current()->getChan()), 
            ::g.times.getRate());
  }
  else
#endif
  sprintf(setup_cmd, "setup p=%d r=%.2f ", vaps_port, ::g.times.getRate());
  write(sdtcp, setup_cmd, strlen(setup_cmd));  // setup packet
  return sdtcp;
}

void Android::disconnectFromBapServer()
{
  if (sdtcp > 0) {
    write(sdtcp, "stop", 4); Socket::closeStream(sdtcp);
    if (ipmode == MULTICAST) Socket::dropMembership(sdudp, &mreq);
  }
  if (sdudp > 0) Socket::closeDatagram(sdudp);
  //error("Connection closed with the vaps server");
  sdtcp = sdudp = -1;
  state = INACTIVE;
}

/** get a frame from the vaps server */
int Android::readBapFrame()
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
  if ((len = recvfrom(sdudp, bapline, VAPS_BUFSIZ, 0,
                      (struct sockaddr *) &udpsa, &slen)) < 0) { // receive Bap line
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
void Android::changePermanent(float lasting)
{
  if (usercontrol) {
    pos.x = localuser->pos.x;
    pos.y = localuser->pos.y;
    pos.z = localuser->pos.z - localuser->height / 2;
    pos.az = localuser->pos.az - M_PI_2;
    pos.ax = localuser->pos.ax;
    pos.ay = localuser->pos.ay;
    updatePosition();
  }

  if (state != PLAYING) return;  // not connected

  if (readBapFrame()) {
    // bap is ready to be played
    bap->readFrame(bapline);  // get it

    switch (bap->getType()) {
    case TYPE_BAP_V31: case TYPE_BAP_V32: 
      body->animate();  // play bap
      break;
    case TYPE_FAP_V20: case TYPE_FAP_V21:
      for (int i=1; i <= NUM_FAPS; i++)
        if (bap->is(i) && bap->getFap(i) && body->face)
          body->face->animate(i, (int) bap->getFap(i)); // play fap
      break;
    //TODO: predictive interpollation
    }
  }
  else if (body->face) body->face->animate(); // local animation
}

/** send a play command to the vaps server */
void Android::sendPlayToBapServer(const char *bap_name)
{
  char play_cmd[128];

  if (! initReceiver()) { // UDP reception
    error("sendPlayToBapServer: receiver fails"); return;
  }
  if (! connectToBapServer(UNICAST)) { // TCP connection
    error("sendPlayToBapServer: connect fails"); return;
  }
  sprintf(play_cmd, "play f=%s", bap_name);
  write(sdtcp, play_cmd, strlen(play_cmd));  // TCP play packet
  state = PLAYING;
}

void Android::quit()
{
  disconnectFromBapServer();
  if (bap) delete bap; bap = NULL;
  if (body) delete body;
}

void Android::reset()
{
  disconnectFromBapServer();  // first disconnect
  if (! initReceiver()) return;
  state = LISTENING;
#if 0 //dax
  if (! connectToBapServer(MULTICAST)) return;
#endif
}

void Android::reset_cb(Android *android, void *d, time_t s, time_t u)
{
  android->reset();
}

void Android::pause_cb(Android *android, void *d, time_t s, time_t u)
{ android->sendPlayToBapServer("pause.bap"); }

void Android::hi_cb(Android *android, void *d, time_t s, time_t u)
{ android->sendPlayToBapServer("hi.bap"); }

void Android::bye_cb(Android *android, void *d, time_t s, time_t u)
{ android->sendPlayToBapServer("bye.bap"); }

void Android::ask_cb(Android *android, void *d, time_t s, time_t u)
{ android->sendPlayToBapServer("ask.bap"); }

void Android::sit_cb(Android *android, void *d, time_t s, time_t u)
{ android->sendPlayToBapServer("sit.bap"); }

void Android::show_cb(Android *android, void *d, time_t s, time_t u)
{ android->sendPlayToBapServer("show.bap"); }

void Android::clap_cb(Android *android, void *d, time_t s, time_t u)
{ android->sendPlayToBapServer("clap.bap"); }

void Android::nak_cb(Android *android, void *d, time_t s, time_t u)
{ android->sendPlayToBapServer("nak.bap"); }

void Android::test_cb(Android *android, void *d, time_t s, time_t u)
{ android->sendPlayToBapServer("test.bap"); }

void Android::eyes_cb(Android *android, void *d, time_t s, time_t u)
{ android->sendPlayToBapServer("eyes.fap"); }

void Android::joy_cb(Android *android, void *d, time_t s, time_t u)
{ android->sendPlayToBapServer("joy.fap"); }

void Android::sad_cb(Android *android, void *d, time_t s, time_t u)
{ android->sendPlayToBapServer("sad.fap"); }

void Android::surp_cb(Android *android, void *d, time_t s, time_t u)
{ android->sendPlayToBapServer("surp.fap"); }

void Android::jag_cb(Android *android, void *d, time_t s, time_t u)
{ android->sendPlayToBapServer("jag.fap"); }

/** functions initialization */
void Android::funcs()
{
  setActionFunc(ANDROID_TYPE, 0, WO_ACTION pause_cb, "Pause");
  setActionFunc(ANDROID_TYPE, 1, WO_ACTION hi_cb, "Hi");
  setActionFunc(ANDROID_TYPE, 2, WO_ACTION bye_cb, "Bye");
  setActionFunc(ANDROID_TYPE, 3, WO_ACTION ask_cb, "Ask");
  setActionFunc(ANDROID_TYPE, 4, WO_ACTION sit_cb, "Sit");
  setActionFunc(ANDROID_TYPE, 5, WO_ACTION show_cb, "Show");
  setActionFunc(ANDROID_TYPE, 6, WO_ACTION clap_cb, "Clap");
  setActionFunc(ANDROID_TYPE, 7, WO_ACTION nak_cb, "Nak");
  setActionFunc(ANDROID_TYPE, 8, WO_ACTION test_cb, "Test");
  setActionFunc(ANDROID_TYPE, 9, WO_ACTION eyes_cb, "Eyes");
  setActionFunc(ANDROID_TYPE, 10, WO_ACTION joy_cb, "Joy");
  setActionFunc(ANDROID_TYPE, 11, WO_ACTION sad_cb, "Sad");
  setActionFunc(ANDROID_TYPE, 12, WO_ACTION surp_cb, "Surp");
  setActionFunc(ANDROID_TYPE, 13, WO_ACTION jag_cb, "Jag");
  setActionFunc(ANDROID_TYPE, 14, WO_ACTION reset_cb, "Reset");
  setActionFunc(ANDROID_TYPE, 15, WO_ACTION Face::changeFace, "New");
  setActionFunc(ANDROID_TYPE, 16, WO_ACTION Face::changeMoveYes, "Yes");
  setActionFunc(ANDROID_TYPE, 17, WO_ACTION Face::changeMoveNo, "No");
  setActionFunc(ANDROID_TYPE, 18, WO_ACTION Face::changeMoveEyeL, "EyeL");
  setActionFunc(ANDROID_TYPE, 19, WO_ACTION Face::changeMoveMouth, "Mouth");
}
