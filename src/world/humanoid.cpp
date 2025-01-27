//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// humanoid.cpp
//
// Humanoid handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "humanoid.hpp"
#include "world.hpp"	// current
#include "body.hpp"	// Body
#include "face.hpp"	// Face
#include "channel.hpp"	// getGroup
#include "socket.hpp"	// openStream
#include "nsl.hpp"	// my_gethostbyname
#include "pref.hpp"	// reflector
#include "timer.hpp"	// rate
#include "user.hpp"	// localuser
#include "bap.hpp"	// setMask, get, set
#include "gestures.hpp"	// *_bap


const OClass Humanoid::oclass(HUMANOID_TYPE, "Humanoid", Humanoid::creator);

//local
static const char DEF_BODY_URL[] = "/body/man.body";
static float skin[] = {1, .75, .7}; // skin color
static uint8_t vaps_offset_port = Humanoid::VAPS_OFFSET_PORT;

#define DEF_FACES_URL "/v3d/faces.index"


/** Creates from a fileline */
Object * Humanoid::creator(char *l)
{
  return new Humanoid(l);
}

/** Sets default values */
void Humanoid::defaults()
{
  vaps = new char[URL_LEN];
  strcpy(vaps, DEF_VAPS_SERVER);	// default server
  strcpy(name.url, DEF_BODY_URL);	// default body
  face_url = new char[URL_LEN];
  memset(face_url, 0, URL_LEN);
  bapline = new char[VAPS_BUFSIZ];
  memset(bapline, 0, VAPS_BUFSIZ);

  for (int i=0; i<3; i++) cloth[i] = skin[i];
}

/** Parses humanoid */
void Humanoid::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "body="))   l = parseString(l, name.url, "body");	//body
    else if (! stringcmp(l, "head="))   l = parseString(l, face_url, "head");	//head
    else if (! stringcmp(l, "face="))   l = parseString(l, face_url, "face");	//head
    else if (! stringcmp(l, "color="))  l = parseVector3f(l, cloth, "color");	//color
    else if (! stringcmp(l, "server=")) l = parseString(l, vaps, "server");	//server
  }
  end_while_parse(l);
}

/** Sets solid geometry */
void Humanoid::geometry()
{
  char s[128];

  sprintf(s, "solid shape=\"bbox\" dim=\"%f %f %f\" />", .24, .14, B_HEIGHT);
  parseSolid(s);
}

/** Sets behaviors */
void Humanoid::behaviors()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(PERMANENT_MOVEMENT);	// bap/fap frames
  enableBehavior(SPECIFIC_RENDER);
}

/** Inits humanoid */
void Humanoid::inits()
{
  initMobileObject(0);

  sdudp = -1;
  sdtcp = -1;

  bap = new Bap();
  bapstring = NULL;
  baptype = 0;
  bapframes = 0;
  bapparams = 186;

  body = new Body();
  body->object = this;

  body->bap = bap;
  body->setColors(skin, cloth);
  body->load(name.url);
  body->draw();

  if (*face_url) {
    //body->face = new Face();			// onlyone face
    body->face = new Face(DEF_FACES_URL);	// multiple faces
    body->face->load(face_url);
  }
  if (face_url) delete[] face_url;

  vaps_offset_port += 2;
  vaps_port = Channel::getPort(World::current()->chan) + vaps_offset_port;
  if (! ::g.pref.reflector) reset();
  state = INACTIVE;
}

/** Constructor from fileline */
Humanoid::Humanoid(char *l)
{
  parser(l);
  geometry();
  behaviors();
  inits();
  usercontrol = false;
}

/** Constructor from User */
Humanoid::Humanoid()
{
  defaults();
  geometry();
  behaviors();
  inits();
  usercontrol = true;
}

/** Renders */
void Humanoid::render()
{
  if (localuser->humanoid && ! localuser->isVisible()) return;

  body->render(pos);
}

/** Inits receiver in UDP mode : returns 1 if OK else 0 if fails */
int Humanoid::receiver()
{
  if (sdudp > 0) disconnectVaps();

  if ((sdudp = Socket::openDatagram()) < 0) {
    //error("receiver: open UDP failed");
    return 0;
  }
  Socket::bindSocket(sdudp, INADDR_ANY, vaps_port);
#if 1 //dax
  if (ipmode == MULTICAST) {
    char group[GROUP_LEN];
    Channel::getGroup(World::current()->chan, group);
    inet4_pton(group, &(udpsa.sin_addr.s_addr));
    memset(&mreq, 0, sizeof(mreq));
    inet4_pton(group, &(mreq.imr_multiaddr.s_addr));
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (Socket::addMembership(sdudp, &mreq) < 0) {
      error("receiver: cannot join multicast group %s", group); return 0;
    }
  }
#endif
  //echo("receiver: waiting on port %d, sdudp=%d", vaps_port, sdudp);
  return 1;
}

/** Establishes a TCP connection with the VAPS server and send the setup packet */
int Humanoid::connectVaps(int _ipmode)
{
  char setup_cmd[128];
  struct hostent *hp;

  if ((sdtcp = Socket::openStream()) < 0) {
    error("socket vaps failed");
    return 0;
  }
  if (! (hp = my_gethostbyname(vaps, AF_INET))) {
    error("can't resolve vaps");
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

  if (setsockopt(sdtcp, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&timeout), sizeof(timeout)) < 0) {
    error("setsockopt failed");
    return 0;
  }
  if (connect(sdtcp, (const struct sockaddr *) &tcpsa, sizeof(tcpsa)) < 0) {
    //error("connection failed with the vaps server: %s (%s)", vaps, inet4_ntop(&tcpsa.sin_addr));
    sdtcp = -1;
    return 0;
  }
  //echo("connection established with vaps server: %s(%s)", vaps, inet4_ntop(&tcpsa.sin_addr));
#if 1 //dax
  ipmode = _ipmode;
  if (ipmode == MULTICAST) {
    char group[GROUP_LEN];
    Channel::getGroup(World::current()->chan, group);
    sprintf(setup_cmd, "setup a=%s p=%d t=%d r=%.2f ",
            group, vaps_port, Channel::getTtl(World::current()->chan), ::g.timer.rate());
  }
#endif
  sprintf(setup_cmd, "setup p=%d r=%.2f ", vaps_port, ::g.timer.rate());
  write(sdtcp, setup_cmd, strlen(setup_cmd));	// setup packet
  return sdtcp;
}

/** Disconnects from VAPS server */
void Humanoid::disconnectVaps()
{
  if (sdtcp > 0) {
    Socket::closeStream(sdtcp);
    if (ipmode == MULTICAST)
      Socket::dropMembership(sdudp, &mreq);
  }
  if (sdudp > 0) Socket::closeDatagram(sdudp);
  sdtcp = sdudp = -1;
  state = INACTIVE;
}

/** Gets a frame from the vaps server */
int Humanoid::readFrame()
{
  fd_set set;
  FD_ZERO(&set);
  FD_SET(sdudp, &set);
  struct timeval delay;
  delay.tv_sec = delay.tv_usec = 0;	// set select passing

  if (select(FD_SETSIZE, &set, NULL, NULL, &delay) == 0) {
    return 0;				// nothing to read
  }

  socklen_t slen = sizeof(struct sockaddr_in);
  int len;
  // receives a bap line
  if ((len = recvfrom(sdudp, bapline, VAPS_BUFSIZ, 0, reinterpret_cast<struct sockaddr *> (&udpsa), &slen)) < 0) {
    error("readFrame: error recvfrom");
    return 0;
  }
  if (! len) {
    return 0;	// end of stream
  }
  return 1;	// bap is present
}

/** Gets current bap line - progresses bapstring */
int Humanoid::getLine()
{
  char k;
  int c;

  memset(bapline, 0, 128);
  for (c = 0; (k = bapstring[c]) != '\n' && k; c++) {
    bapline[c] = k;
  }
  bapline[c] = '\0';
  bapstring += (c + 1);		// points on next line
  //if (k == 0) return 0;	// end of string
  return c;
}

/** System of equations handling permanent motion */
void Humanoid::changePermanent(float lasting)
{
  static float angle = 0;

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

  if ((sdtcp > 0) && readFrame()) {		// from vaps server
    // bap is ready to be played
    //echo("bapline: %s", bapline);
    uint8_t baptype = bap->parse(bapline);	// parse it

    switch (baptype) {
    case TYPE_BAP_V31: case TYPE_BAP_V32: 
      for (int i=1; i <= NUM_BAPS; i++) {
        if (bap->isBap(baptype) && ! bap->isBapMask(i)) continue;
        //echo("playbap: %d (%.0f)", i, bap->get(i));
      }
      body->play();				// plays bapframe
      break;
    case TYPE_FAP_V20: case TYPE_FAP_V21:
      for (int i=1; i <= NUM_FAPS; i++) {
        if (bap->isFap(baptype) && ! bap->isFapMask(i)) continue;
        //echo("playfap: %d (%.0f)", i, bap->get(i));
        body->face->play(i, bap->get(i));	// play fapframe
      }
      break;
    case 0:	// end of frames
      // for tests only !!!
      //body->animHead(+30, 0);		// head flexion front : OK
      //body->animHead(+30, 1);		// head abduct
      //body->animHead(+30, 2);		// head torsion right : OK
      //body->animNeck(+30, 0);		// neck flexion front : OK
      //body->animNeck(-30, 1);		// neck abduct right : OK
      //body->animNeck(-30, 2);		// neck torsion : NO
      //body->animChest(-30, 0);	// chest flexion back : OK
      //body->animChest(+30, 0);	// chest flexion front : OK
      //body->animChest(-30, 1);	// chest abduct right : OK
      //body->animChest(-30, 2);	// chest torsion left : OK
      //for (int i=0; i<30; i++) {
      //  body->animArm(-i, 0, 0);	// arm left flexion : OK
      //  body->animArm(+i, 1, 0);	// arm right flexion : OK
      //  body->animForearm(-2*i, 0, 0);// forearm left flexion front : OK
      //  body->animForearm(+2*i, 1, 0);// forearm right flexion front : OK
      //}
      //body->animLeg(-30, 0, 0);	// leg left front flexion : OK
      //body->animLeg(+30, 0, 1);	// leg left abduct back : OK
      //body->animLeg(-30, 1, 2);	// leg right torsion ext : OK
      //body->animFoot(-30, 1, 2);	// foot right torsion ext : NO
      render();
       state = INACTIVE;
       angle = 0;
      break;
    default:
      break;
    //TODO: predictive interpollation
    }
  }

  else if ((sdtcp > 0) && body->face) {
    body->face->play();		// local animation
  }

  else if (sdtcp <= 0) {	// check if without server VAPS
    //
    // local playing
    //
    // get frame from local bapstring (see gestures.hpp)
    //
    char *p = NULL;
    int len = 0;

    if (! baptype) {	// check if is header line
      //
      // bap hdr
      //
      //if (togfirst == false && togcurr == false) {
        //echo("purge");
        //return;
      //}

      len = getLine();
      if (len == 0) {
        return;
      }
      if (::g.pref.trace) echo("baphdr: %s", bapline);

      p = strrchr(bapline, ' ');
      if (p) {
        bapframes = atoi(++p);
      }
      baptype = bap->parse(bapline);	// warning bapline is now tokenized
      //echo("baptype: %d", baptype);
      bapparams = bap->getParams();
      return;
    }

    //
    // data bap
    //
newbap:
    // we expect a masks line
    len = getLine();
    if (len == 0) {
      error("missing masks");
      goto endbap;
    }

    if (baptype) {
      //
      // masks
      //
      //echo("masks: %s", bapline);
      p = bapline;
      for (int i=1; i <= bapparams; i++) {
        if (p) {
          switch (*p) {
          case '0': bap->setMask(i, false); break;
          case '1': bap->setMask(i, true); break;
          }
          p = strchr(p, ' ');
          if (! p) break;	// no more mask
          p++;			// next mask
        } 
      } 
    } 

    // we expect a frame number + values
    len = getLine();		// frame + values
    if (len == 0) {
      error("missing values");
      goto endbap;
    }

    // frame sequence number
    bapframe = atoi(bapline);
    //echo("bapframe: %d", bapframe);

    // values
    p = strchr(bapline, ' ');
    if (! p) {
      if (::g.pref.trace) echo("empty value");
      goto newbap;		// no more values
    }
    p++;			// points on first value

    for (int i=1; i <= bapparams; i++) {
      float value = 0;
      if (bap->isBap() && ! bap->isBapMask(i)) continue;	// no bap mask
      if (bap->isFap() && ! bap->isFapMask(i)) continue;	// no fap mask
      //
      // play bap/fap param
      //
      if (p) {
        value = static_cast<float> (atof(p));
      }
      else {
        if (::g.pref.trace) error("bad value");
        goto newbap;
      }
      if (bap->isBap(baptype)) {
        bap->set(i, value);			// set bap value
        //echo("playbap: %d: %d (%.0f)", bapframe, i, bap->get(i));
        body->play();				// plays bap
      }
      if (bap->isFap(baptype)) {
        bap->set(i, value);			// set fap value
        //echo("playfap: %d: %d (%.0f)", bapframe, i, bap->get(i));
        if (body->face) {
          body->face->play(i, bap->get(i));	// plays fap
        }
      }
      else {
        //error("bad baptype: %d", baptype);
      }
      p = strchr(p, ' ');	// skip space for next value
      if (! p) {
        //echo("end of values");
      }
      else {
        p++;			// next value
        //echo("next value: %s", p);
      }
    }

#if 0 //dax
    // tempo 20 ms
    struct timeval to;
    to.tv_sec = 0;
    to.tv_usec = 200000;	// ::g.pref.frame_delay; // 10ms -> 100 fps
    select(0, 0, 0, 0, &to);
#endif

endbap:
    if (bapframe + 1 == bapframes) {
      //echo("end of frames");
      bapframes = 0;
      bapparams = 0;
      baptype = 0;
      bapstring = NULL;
      //debug sleep(5);
      state = INACTIVE;
    }
  } // local playing

  // testing
  angle = 10;
  //body->animArm(-angle, 0, 0);	// arm left flexion : OK
  //body->animArm(+angle, 1, 0);	// arm right flexion : OK
  //body->animForearm(-2*angle, 0, 0);	// forearm left flexion front : OK
  //body->animForearm(+2*angle, 1, 0);	// forearm right flexion front : OK
}

/** Sends a play command to the vaps server */
bool Humanoid::sendPlay(const char *bap_name)
{
  char play_cmd[128];

  if (sdtcp <= 0) {
    if (! receiver()) {			// UDP reception
      error("sendPlay: receiver fails");
      return false;
    }
    if (! connectVaps(UNICAST)) {	// opens a TCP connection
      //error("sendPlay: connect fails");
      state = PLAYING;	// local playing
      return false;
    }
  }
  sprintf(play_cmd, "play f=%s", bap_name);
  write(sdtcp, play_cmd, strlen(play_cmd));  // TCP play packet
  state = PLAYING;
  return true;
}

/** Quits */
void Humanoid::quit()
{
  disconnectVaps();
  if (bap) delete bap;
  bap = NULL;
  if (body) delete body;
  body = NULL;
}

/** Resets */
void Humanoid::reset()
{
  disconnectVaps();  // first disconnect
  if (! receiver()) return;
  state = LISTENING;
  if (! connectVaps(MULTICAST)) return;
}

/** Plays pause */
void Humanoid::pause()
{
  sendPlay("pause.bap");
  bapstring = const_cast<char *>(pause_bap);
}

/** Plays hi */
void Humanoid::hi()
{
  sendPlay("hi.bap");
  bapstring = const_cast<char *>(hi_bap);
}

/** Plays bye */
void Humanoid::bye()
{
  sendPlay("bye.bap");
  bapstring = const_cast<char *>(bye_bap);
}

/** Plays ask */
void Humanoid::ask()
{
  sendPlay("ask.bap");
  bapstring = const_cast<char *>(ask_bap);
}

/** Plays sit */
void Humanoid::sit()
{
  sendPlay("sit.bap");
  bapstring = const_cast<char *>(sit_bap);
}

/** Plays show */
void Humanoid::show()
{
  sendPlay("show.bap");
  bapstring = const_cast<char *>(show_bap);
}

/** Plays clap */
void Humanoid::clap()
{
  sendPlay("clap.bap");
  bapstring = const_cast<char *>(clap_bap);
}

/** Plays nak */
void Humanoid::nak()
{
  sendPlay("nak.bap");
  bapstring = const_cast<char *>(nak_bap);
}

/** Plays bof */
void Humanoid::bof()
{
  sendPlay("bof.bap");
  bapstring = const_cast<char *>(bof_bap);
  //sendPlay("rami.bap");
  //bapstring = const_cast<char *>(rami_bap);
}

/** Plays pivot */
void Humanoid::pivot()
{
  sendPlay("pivot.bap");
  bapstring = const_cast<char *>(pivot_bap);
}

/** Plays test */
void Humanoid::test()
{
  sendPlay("test.bap");
  bapstring = const_cast<char *>(test_bap);
}

/** Plays yes */
void Humanoid::yes()
{
  sendPlay("yes.bap");
  bapstring = const_cast<char *>(yes_bap);
}

/** Plays no */
void Humanoid::no()
{
  sendPlay("no.bap");
  bapstring = const_cast<char *>(no_bap);
}

void Humanoid::eyes()
{
  sendPlay("eyes.fap");
  bapstring = const_cast<char *>(eyes_fap);
}

void Humanoid::joy()
{
  sendPlay("joy.fap");
  bapstring = const_cast<char *>(joy_fap);
}

void Humanoid::sad()
{
  sendPlay("sad.fap");
  bapstring = const_cast<char *>(sad_fap);
}

void Humanoid::surp()
{
  sendPlay("surp.fap");
  bapstring = const_cast<char *>(surp_fap);
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

void Humanoid::yes_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->yes();
}

void Humanoid::no_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->no();
}

void Humanoid::bof_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->bof();
}

void Humanoid::pivot_cb(Humanoid *humanoid, void *d, time_t s, time_t u)
{
  humanoid->pivot();
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

/** functions initialization */
void Humanoid::funcs()
{
  setActionFunc(HUMANOID_TYPE, 0, _Action pause_cb, "pause");
  setActionFunc(HUMANOID_TYPE, 1, _Action hi_cb, "hi");
  setActionFunc(HUMANOID_TYPE, 2, _Action bye_cb, "bye");
  setActionFunc(HUMANOID_TYPE, 3, _Action ask_cb, "ask");
  setActionFunc(HUMANOID_TYPE, 4, _Action sit_cb, "sit");
  setActionFunc(HUMANOID_TYPE, 5, _Action show_cb, "show");
  setActionFunc(HUMANOID_TYPE, 6, _Action clap_cb, "clap");
  setActionFunc(HUMANOID_TYPE, 7, _Action nak_cb, "nak");
  setActionFunc(HUMANOID_TYPE, 8, _Action bof_cb, "bof");
  setActionFunc(HUMANOID_TYPE, 9, _Action pivot_cb, "pivot");
  setActionFunc(HUMANOID_TYPE, 10, _Action test_cb, "test");
  setActionFunc(HUMANOID_TYPE, 11, _Action eyes_cb, "eyes");
  setActionFunc(HUMANOID_TYPE, 12, _Action joy_cb, "joy");
  setActionFunc(HUMANOID_TYPE, 13, _Action sad_cb, "sad");
  setActionFunc(HUMANOID_TYPE, 14, _Action surp_cb, "surp");
  setActionFunc(HUMANOID_TYPE, 15, _Action yes_cb, "yes");
  setActionFunc(HUMANOID_TYPE, 16, _Action no_cb, "no");
  setActionFunc(HUMANOID_TYPE, 17, _Action Face::changeFace, "new");
  setActionFunc(HUMANOID_TYPE, 18, _Action Face::EyeL, "eyeL");
  setActionFunc(HUMANOID_TYPE, 19, _Action Face::Smile, "smile");
  setActionFunc(HUMANOID_TYPE, 20, _Action reset_cb, "reset");
}
