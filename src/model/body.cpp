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
// body.cpp
//
// Body articulations
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "body.hpp"
#include "face.hpp"	// Face::SCALE
#include "http.hpp"	// Http
#include "texture.hpp"	// open
#include "format.hpp"	// getModelByUrl
#include "user.hpp"	// localuser
#include "cache.hpp"	// open, close
#include "pref.hpp"	// trace

#define TRACE(s,v) if(::g.pref.trace) echo("%s: %.0f", s,v);


const float Phalanx2::PHALANX2_LEN = 0.012;	// default phalanx2 length
const float Phalanx::PHALANX_LEN = 0.012;	// default phalanx length
const float Finger::FINGER_LEN = 0.0375;	// default finger length
const float Hand::HAND_LEN = 0.10;		// default hand length
const float Forearm::FOREARM_LEN = 0.178;	// default forearm length
const float Forearm::ELBOW_RAD = 0.025;		// default elbow radius
const float Arm::ARM_LEN = 0.165;		// default arm length
const float Shoulder::SHOULDER_RAD = 0.0025;	// default shoulder radius
const float HeadBody::HEAD_RAD = 0.075;		// default head radius
const float Neck::NECK_LEN = 0.05;		// default neck height
const float Foot::FOOT_LEN = 0.12;		// default foot length
const float Shin::SHIN_LEN = 0.265;		// default shin length
const float Thigh::THIGH_LEN = 0.25;		// default thigh length
const float Chest::CHEST_LEN = 0.30;		// default chest length

static const struct sBodyToken btokens[] = {
  { "head",     Body::HEAD },
  { "neck",     Body::NECK },
  { "chest",    Body::CHEST },
  { "hip",      Body::HIPS },
  { "lShldr",   Body::L_ARM },
  { "lForearm", Body::L_FOREARM },
  { "lHand",    Body::L_HAND },
  { "rShldr",   Body::R_ARM },
  { "rForearm", Body::R_FOREARM },
  { "rHand",    Body::R_HAND },
  { "lThigh",   Body::L_THIGH },
  { "lShin",    Body::L_SHIN },
  { "lFoot",    Body::L_FOOT },
  { "rThigh",   Body::R_THIGH },
  { "rShin",    Body::R_SHIN },
  { "rFoot",    Body::R_FOOT },
  { "abdomen",  Body::ABDOMEN },
  { "lCollar",  Body::L_COLLAR },
  { "rCollar",  Body::R_COLLAR },
  { "belt",     Body::BELT },
  { "lThumb",   Body::L_THUMB },
  { "lIndex",   Body::L_INDEX },
  { "lMiddle",  Body::L_MIDDLE },
  { "lRing",    Body::L_RING },
  { "lPinky",   Body::L_PINKY },
  { "rThumb",   Body::R_THUMB },
  { "rIndex",   Body::R_INDEX },
  { "rMiddle",  Body::R_MIDDLE },
  { "rRing",    Body::R_RING },
  { "rPinky",   Body::R_PINKY },
  { "skirt",    Body::SKIRT },
  { "/body",    Body::MAX_PARTS /* -1 */ } // James Addison C++14 compatible
};


/** Phalanx2 */
Phalanx2::Phalanx2()
{
  length = PHALANX2_LEN;
}

int Phalanx2::init()
{
  return 1;
}

/** Phalanx */
Phalanx::Phalanx(Phalanx2 *_phalanx2)
{
  aflexion = 0;
  length = PHALANX_LEN;
  phalanx2 = _phalanx2;
}

int Phalanx::init()
{
  return phalanx2->init();
}

void Phalanx::flexion(float a)
{
  aflexion = a;
  TRACE("phal-f", a);
}

/** Finger */
Finger::Finger(Phalanx *_phalanx)
{
  aflexion = aabduct = 0;
  length = FINGER_LEN;
  phalanx = _phalanx;
}

int Finger::init()
{
  return phalanx->init();
}

void Finger::flexion(float a)
{
  aflexion = a;
  TRACE("fing-f", a);
}

void Finger::abduct(float a)
{
  aabduct = a;
  TRACE("fing-a", a);
}

/** Hand */
Hand::Hand(Finger **_fingers)
{
  aflexion = aabduct = atorsion = 0;
  length = HAND_LEN;
  fingers[0] = _fingers[0];
  fingers[1] = _fingers[1];
  fingers[2] = _fingers[2];
  fingers[3] = _fingers[3];
  fingers[4] = _fingers[4];
}

int Hand::init()
{
  return fingers[4]->init();
}

void Hand::flexion(float a)
{
  aflexion = a;
  TRACE("hand-f", a);
}

void Hand::abduct(float a)
{
  aabduct = a;
  TRACE("hand-a", a);
}

void Hand::torsion(float a)
{
  atorsion = a;
  TRACE("hand-t", a);
}

/** Forearm / Elbow */
Forearm::Forearm(Hand *_hand)
{
  aflexion = atorsion = 0;	// elbow
  length = FOREARM_LEN;
  hand = _hand;
}

int Forearm::init()
{
  return hand->init();
}

void Forearm::flexion(float a)
{
  aflexion = a;
  TRACE("fore-f", a);
}

void Forearm::torsion(float a)
{
  atorsion = a;
  TRACE("fore-t", a);
}

/** Arm */
Arm::Arm(Forearm *_forearm)
{
  aabduct = -90;
  aflexion = 0;
  atorsion = 90;
  aflexion = aabduct = atorsion = 0; //dax
  length = ARM_LEN;
}

int Arm::init()
{
  return forearm->init();
}

void Arm::flexion(float a)
{
  //dax aflexion = -a;
  aflexion = a;
  TRACE("arm-f", a);
}

void Arm::abduct(float a)
{
  //dax aabduct = a - 90;
  aabduct = a;
  TRACE("arm-a", a);
}

void Arm::torsion(float a)
{
  //dax atorsion = a + 90;
  atorsion = a;
  TRACE("arm-t", a);
}

/** Shoulder */
Shoulder::Shoulder(Arm *_arm)
{
  aflexion = aabduct = 0;
  radius = SHOULDER_RAD;
  arm = _arm;
}

int Shoulder::init()
{
  return arm->init();
}

void Shoulder::flexion(float a)
{
  aflexion = a;
  TRACE("shou-f", a);
}

void Shoulder::abduct(float a)
{
  aabduct = a;
  TRACE("shou-a", a);
}

/** HeadBody */
HeadBody::HeadBody()
{
  aflexion = aabduct = atorsion = 0;
  radius = HEAD_RAD;
}

int HeadBody::init()
{
  return 1;
}

void HeadBody::flexion(float a)
{
  aflexion = a;
  TRACE("head-f", a);
}

void HeadBody::abduct(float a)
{
  aabduct = a;
  TRACE("head-a", a);
}

void HeadBody::torsion(float a)
{
  atorsion = a;
  TRACE("head-t", a);
}

/** Neck */
Neck::Neck(HeadBody *_head)
{
  aflexion = aabduct = atorsion = 0;
  length = NECK_LEN;
  head = _head;
}

int Neck::init()
{
  return head->init();
}

void Neck::flexion(float a)
{
  aflexion = a;
  TRACE("neck-f", a);
}

void Neck::abduct(float a)
{
  aabduct = a;
  TRACE("neck-a", a);
}

void Neck::torsion(float a)
{
  atorsion = a;
  TRACE("neck-t", a);
}

/** Foot / Ankle */
Foot::Foot()
{
  aflexion = aabduct = atorsion = 0;
  length = FOOT_LEN;
}

int Foot::init()
{
  return 1;
}

void Foot::flexion(float a)
{
  aflexion = -a;
  TRACE("foot-f", a);
}

void Foot::abduct(float a)
{
  aabduct = -a;
  TRACE("foot-a", a);
}

void Foot::torsion(float a)
{
  atorsion = -a;
  TRACE("foot-t", a);
}

/** Shin / Knee */
Shin::Shin(Foot *_foot)
{
  aflexion = atorsion = 0;	// knee
  length = SHIN_LEN;
  foot = _foot;
}

int Shin::init()
{
  return foot->init();
}

void Shin::flexion(float a)
{
  aflexion = -a;
  TRACE("shin-f", a);
}

void Shin::torsion(float a)
{
  atorsion = -a;
  TRACE("shin-t", a);
}

/** Thigh / Hip */
Thigh::Thigh(Shin *_shin)
{
  aabduct = aflexion = atorsion = 0;
  length = THIGH_LEN;
  shin = _shin;
}

int Thigh::init()
{
  return shin->init();
}

void Thigh::flexion(float a)
{
  aflexion = a;
  TRACE("thig-f", a);
}

void Thigh::abduct(float a)
{
  aabduct = a;
  TRACE("thig-a", a);
}

void Thigh::torsion(float a)
{
  atorsion = a;
  TRACE("thig-t", a);
}

/** Chest */
Chest::Chest()
{
  aflexion = aabduct = atorsion = 0;
}

void Chest::flexion(float a)
{
  aflexion = a;
  TRACE("ches-f", a);
}

void Chest::abduct(float a)
{
  aabduct = a;
  TRACE("ches-a", a);
}

void Chest::torsion(float a)
{
  atorsion = a;
  TRACE("ches-t", a);
}

/** Body */
Body::Body()
{
  init();
}

/** Constructor */
Body::Body(const char *_url)
{
  init();
  load(_url);
}

/** Initialization */
void Body::init()
{
  drawparts = 0;
  bodyparts = NULL;
  url = NULL;
  face = NULL;
  jp.x = NULL;
  jp.y = NULL;
  jp.z = NULL;
  model = 0;

  tx = ty = tz = 0;	// position in space
  rx = ry = rz = 0;	// orientation in space
  bscale = B_SCALE;	// scale factor

  //
  // articulation instances
  //
  head =       new HeadBody();
  neck =       new Neck(head);
  hand_l =     new Hand(fingers_l);
  hand_r =     new Hand(fingers_r);
  forearm_l =  new Forearm(hand_l);
  forearm_r =  new Forearm(hand_r);
  arm_l =      new Arm(forearm_l);
  arm_r =      new Arm(forearm_r);
  shoulder_l = new Shoulder(arm_l);
  shoulder_r = new Shoulder(arm_r);
  foot_l =     new Foot();
  foot_r =     new Foot();
  shin_l =     new Shin(foot_l);
  shin_r =     new Shin(foot_r);
  thigh_l =    new Thigh(shin_l);
  thigh_r =    new Thigh(shin_r);
  chest =      new Chest();
  for (int i=0; i<5 ; i++) {
    fingers_r[i] = new Finger(phalanx[i]);
    fingers_l[i] = new Finger(phalanx[i+5]);
  }
  for (int i=0; i<10 ; i++) {
    phalanx2[i] = new Phalanx2();
    phalanx[i] = new Phalanx(phalanx2[i]);
  }

  for (int i=0; i<3 ; i++) {
    skin[i] = 1;
    cloth[i] = .5;
  }
}

/** Destructor */
Body::~Body()
{
  if (hand_l) delete hand_l;
  if (hand_r) delete hand_r;
  if (forearm_l) delete forearm_l;
  if (forearm_r) delete forearm_r;
  if (arm_l) delete arm_l;
  if (arm_r) delete arm_r;
  if (shoulder_l) delete shoulder_l;
  if (shoulder_r) delete shoulder_r;
  if (head) delete head;
  if (neck) delete neck;
  if (chest) delete chest;
  if (thigh_l) delete thigh_l;
  if (thigh_r) delete thigh_r;
  if (shin_l) delete shin_l;
  if (shin_r) delete shin_r;
  if (foot_l) delete foot_l;
  if (foot_r) delete foot_r;
  for (int i=0; i<10 ; i++) {
    if (phalanx2[i]) delete phalanx2[i];
    if (phalanx[i])  delete phalanx[i];
  }
  for (int i=0; i<5 ; i++) {
    if (fingers_l[i]) delete fingers_l[i];
    if (fingers_r[i]) delete fingers_r[i];
  }

  if (dlist > 0) glDeleteLists(dlist, drawparts);
  if (bodyparts) delete[] bodyparts;
  if (jp.x) delete[] jp.x;
  if (jp.y) delete[] jp.y;
  if (jp.z) delete[] jp.z;
  if (face) delete face;
  if (url) delete[] url;
}

char * Body::skipEqual(char *p)
{
  if (p && (p = strchr(p, '='))) p++;
  return p;
}

char * Body::getTok(char *l, int *tok)
{
  char *t = l;
  const struct sBodyToken *ptab;

  *tok = -1;
  l = skipEqual(l);
  if (l) *(l-1) = '\0';  // end of token '=', replaced by null terminated
  for (ptab = btokens; *ptab->name != -1 ; ptab++) {
    if (! strcmp(ptab->name, t)) {
      *tok = ptab->num;
      return l;
    }
  }
  error("getTok: unknown \"%s\" in %s", t, l);
  return l;
}

/** Loads body url */
void Body::load(const char *_url)
{
  url = new char[URL_LEN];
  strcpy(url, _url);
  Http::httpOpen(url, reader, this, 0);
}

/** Reads joint points (static) */
void Body::reader(void *_body, Http *http)
{
  Body *body = static_cast<Body *>(_body);
  if (! body) return;

  Cache *cache = new Cache();
  FILE *f = cache->open(http->url, http);
  if (! f) {
    error("Body: can't open %s", http->url);
    delete cache;
    return;
  }
  body->loadBodyParts(f);
  cache->close();
  delete cache;
}

/** Loads body's parts */
void Body::loadBodyParts(FILE *f)
{
  char *l, jpline[256], tex[128];

  tex[0] = '\0';
  // alloc bodyparts and joint-points arrays
  bodyparts = new sBodyParts[MAX_PARTS];
  jp.x = new float[MAX_PARTS];
  jp.y = new float[MAX_PARTS];
  jp.z = new float[MAX_PARTS];

  // init bodyparts
  for (int i=0; i < MAX_PARTS; i++) {
    bodyparts[i].loaded = false;
    bodyparts[i].scale = 1;
    for (int j=0; j<3 ; j++) bodyparts[i].scales[j] = 1;
    for (int j=0; j<3 ; j++) bodyparts[i].color[j] = cloth[j];
    bodyparts[i].texid = 0;
    bodyparts[i].off = NULL;
    bodyparts[i].obj = NULL;
    bodyparts[i].url[0] = '\0';
    bodyparts[i].texurl[0] = '\0';
  }
  // set default color
  for (int j=0; j<3 ; j++) {
    bodyparts[HEAD].color[j] = skin[j];
    bodyparts[NECK].color[j] = skin[j];
    bodyparts[L_FOREARM].color[j] = skin[j];
    bodyparts[R_FOREARM].color[j] = skin[j];
    bodyparts[L_HAND].color[j] = skin[j];
    bodyparts[R_HAND].color[j] = skin[j];
  }

  // parse first line of <body ...>
  if (! fgets(jpline, sizeof(jpline), f)) return;
  jpline[strlen(jpline) - 1] = '\0';
  if (! stringcmp(jpline, "<body")) {
    l = strtok(jpline, TOK_SEP);	// tokenize jpline
    l = strtok(NULL, TOK_SEP);		// next token
    while (l) {
      if      (! stringcmp(l, "scale="))
        l = wobject->parse()->parseFloat(l, &bscale, "scale");
      else if (! stringcmp(l, "tex="))
        l = wobject->parse()->parseString(l, tex, "tex");
    }
  }

  if (*tex) {
    for (int i=0; i < MAX_PARTS; i++) {
      strcpy(bodyparts[i].texurl, tex);
    }
  }

  // get body joint points and urls
  for (int i=0; i < MAX_PARTS; i++) {
    if (! fgets(jpline, sizeof(jpline), f)) break;
    jpline[strlen(jpline) - 1] = '\0';
    //echo("loadBodyParts: %s", jpline);

    int bpindex = 0;
    char *p = jpline;
    if (jpline[0] == '#') continue;
    else if ((p = strchr(jpline, '<'))) {
      l = strtok(p, TOK_SEP);
      l = getTok(l, &bpindex);
      if (bpindex < 0)
        goto endloadbody;  // eof
      l = strtok(NULL, TOK_SEP);

      // parse others lines of <body ...> ... </body>
      begin_while_parse(l) {
        if (! stringcmp(l, "jp=")) {
          l = skipEqual(l);
          l++;  // "
          jp.x[bpindex] = static_cast<float>(atof(l) / bscale); l = strtok(NULL, TOK_SEP);
          jp.y[bpindex] = static_cast<float>(atof(l) / bscale); l = strtok(NULL, TOK_SEP);
          jp.z[bpindex] = static_cast<float>(atof(l) / bscale); l = strtok(NULL, TOK_SEP);
        }
        else if (*l == '/') break;  // eol
        else if (! stringcmp(l, "url=")) {
          l = wobject->parse()->parseString(l, bodyparts[i].url, "url");
          drawparts++;
        }
        else if (! stringcmp(l, "scale=")) {
          l = wobject->parse()->parseFloat(l, &bodyparts[i].scale, "scale");
          bodyparts[i].scales[0] = bodyparts[i].scale;
          bodyparts[i].scales[1] = bodyparts[i].scale;
          bodyparts[i].scales[2] = bodyparts[i].scale;
        }
        else if (! stringcmp(l, "scalex="))
          l = wobject->parse()->parseFloat(l, &bodyparts[i].scales[0],"scalex");
        else if (! stringcmp(l, "scaley="))
          l = wobject->parse()->parseFloat(l, &bodyparts[i].scales[1],"scaley");
        else if (! stringcmp(l, "scalez="))
          l = wobject->parse()->parseFloat(l, &bodyparts[i].scales[2],"scalez");
        else if (! stringcmp(l, "color="))
          l = wobject->parse()->parseVector3f(l, bodyparts[i].color, "color");
        else if (! stringcmp(l, "tex="))
          l = wobject->parse()->parseString(l, bodyparts[i].texurl, "tex");
      } end_while_parse(l);
    }
  }

endloadbody:
  // now we can download all parts
  for (int i=0; i < MAX_PARTS; i++) {
    if (bodyparts[i].url[0]) {  // if url exist
      bodyparts[i].model = Format::getModelByUrl(bodyparts[i].url);

      switch (bodyparts[i].model) {
      case MODEL_OFF:
        bodyparts[i].off = new Off(bodyparts[i].url);
        Http::httpOpen(bodyparts[i].url, Off::reader, bodyparts[i].off, 0);
        break;
      case MODEL_OBJ:
        bodyparts[i].obj = new Obj(bodyparts[i].url, 1);
        Http::httpOpen(bodyparts[i].url, Obj::reader, bodyparts[i].obj, 0);
        bodyparts[i].obj->setScale(bscale * bodyparts[i].scale);
        if (bodyparts[i].texurl[0]) { // if url exist
          bodyparts[i].texid = Texture::open(bodyparts[i].texurl);
        }
        break;
      }
      bodyparts[i].loaded = true;
    }
  }
  model = bodyparts[0].model;  // keep model used
}

/** Draws body parts */
void Body::draw()
{
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

  dlist = glGenLists(drawparts);
  for (int i=0; i < drawparts && bodyparts[i].loaded ; i++) {
    glNewList(dlist + i, GL_COMPILE);
    switch (bodyparts[i].model) {
      case MODEL_OFF: bodyparts[i].off->draw(); break;
      case MODEL_OBJ: bodyparts[i].obj->draw(); break;
    }
    glEndList();
  }
}

/** Sets skin and cloth colors - called from humanoid */
void Body::setColors(float *_skin, float *_cloth)
{
  for (int i=0; i<3; i++) {
    skin[i] = _skin[i];
    cloth[i] = _cloth[i];
    color[i] = _cloth[i];
  }
}

/** Animates all body's articulations - called from humanoid.cpp */
void Body::play()
{
  if (! bap) return;

#if 0 //notused
  if (bap->isBapMask(PELVIC_TILT))	chest->abduct(bap->get(PELVIC_TILT));
  if (bap->isBapMask(PELVIC_TORS))	chest->torsion(bap->get(PELVIC_TORS));
  if (bap->isBapMask(PELVIC_ROLL))	chest->flexion(bap->get(PELVIC_ROLL));

  if (bap->isBapMask(L_HIP_FLEX))	thigh_l->flexion(bap->get(L_HIP_FLEX));
  if (bap->isBapMask(R_HIP_FLEX))	thigh_r->flexion(bap->get(R_HIP_FLEX));
  if (bap->isBapMask(L_HIP_ABDU))	thigh_l->abduct(bap->get(L_HIP_ABDU));
  if (bap->isBapMask(R_HIP_ABDU))	thigh_r->abduct(bap->get(R_HIP_ABDU));
  if (bap->isBapMask(L_HIP_TORS))	thigh_l->torsion(bap->get(L_HIP_TORS));
  if (bap->isBapMask(R_HIP_TORS))	thigh_r->torsion(bap->get(R_HIP_TORS));

  if (bap->isBapMask(L_KNEE_FLEX))	shin_l->flexion(bap->get(L_KNEE_FLEX));
  if (bap->isBapMask(R_KNEE_FLEX))	shin_r->flexion(bap->get(R_KNEE_FLEX));
  if (bap->isBapMask(L_KNEE_TORS))	shin_l->torsion(bap->get(L_KNEE_TORS));
  if (bap->isBapMask(R_KNEE_TORS))	shin_r->torsion(bap->get(R_KNEE_TORS));

  if (bap->isBapMask(L_ANKLE_FLEX))	foot_l->flexion(bap->get(L_ANKLE_FLEX));
  if (bap->isBapMask(R_ANKLE_FLEX))	foot_r->flexion(bap->get(R_ANKLE_FLEX));
  if (bap->isBapMask(L_ANKLE_TORS))	foot_l->torsion(bap->get(L_ANKLE_TORS));
  if (bap->isBapMask(R_ANKLE_TORS))	foot_r->torsion(bap->get(R_ANKLE_TORS));
#endif //notused

  if (bap->isBapMask(L_SHOULDER_FLEX))	arm_l->flexion(bap->get(L_SHOULDER_FLEX));
  if (bap->isBapMask(R_SHOULDER_FLEX))	arm_r->flexion(bap->get(R_SHOULDER_FLEX));
  if (bap->isBapMask(L_SHOULDER_ABDU))	arm_l->abduct(bap->get(L_SHOULDER_ABDU));
  if (bap->isBapMask(R_SHOULDER_ABDU))	arm_r->abduct(bap->get(R_SHOULDER_ABDU));
  if (bap->isBapMask(L_SHOULDER_TORS))	arm_l->torsion(bap->get(L_SHOULDER_TORS));
  if (bap->isBapMask(R_SHOULDER_TORS))	arm_r->torsion(bap->get(R_SHOULDER_TORS));

  if (bap->isBapMask(L_ELBOW_FLEX))	forearm_l->flexion(bap->get(L_ELBOW_FLEX));
  if (bap->isBapMask(R_ELBOW_FLEX))	forearm_r->flexion(bap->get(R_ELBOW_FLEX));

  if (bap->isBapMask(L_WRIST_FLEX))	hand_l->abduct(bap->get(L_WRIST_FLEX));
  if (bap->isBapMask(R_WRIST_FLEX))	hand_r->abduct(bap->get(R_WRIST_FLEX));
  if (bap->isBapMask(L_WRIST_PIVOT))	hand_l->flexion(bap->get(L_WRIST_PIVOT));
  if (bap->isBapMask(R_WRIST_PIVOT))	hand_r->flexion(bap->get(R_WRIST_PIVOT));
  if (bap->isBapMask(L_WRIST_TORS))	hand_l->torsion(bap->get(L_WRIST_TORS));
  if (bap->isBapMask(R_WRIST_TORS))	hand_r->torsion(bap->get(R_WRIST_TORS));

  if (bap->isBapMask(L_THUMB1_FLEX))	fingers_l[0]->flexion(bap->get(L_THUMB1_FLEX));
  if (bap->isBapMask(R_THUMB1_FLEX))	fingers_r[0]->flexion(bap->get(R_THUMB1_FLEX));
  if (bap->isBapMask(L_INDEX1_FLEX))	fingers_l[1]->flexion(bap->get(L_INDEX1_FLEX));
  if (bap->isBapMask(R_INDEX1_FLEX))	fingers_r[1]->flexion(bap->get(R_INDEX1_FLEX));
  if (bap->isBapMask(L_MIDDLE1_FLEX))	fingers_l[2]->flexion(bap->get(L_MIDDLE1_FLEX));
  if (bap->isBapMask(R_MIDDLE1_FLEX))	fingers_r[2]->flexion(bap->get(R_MIDDLE1_FLEX));
  if (bap->isBapMask(L_RING1_FLEX))	fingers_l[3]->flexion(bap->get(L_RING1_FLEX));
  if (bap->isBapMask(R_RING1_FLEX))	fingers_r[3]->flexion(bap->get(R_RING1_FLEX));
  if (bap->isBapMask(L_PINKY1_FLEX))	fingers_l[4]->flexion(bap->get(L_PINKY1_FLEX));
  if (bap->isBapMask(R_PINKY1_FLEX))	fingers_r[4]->flexion(bap->get(R_PINKY1_FLEX));

#if 0 //notused
  if (bap->isBapMask(C1_ROLL))		head->flexion(bap->get(C1_ROLL));
  if (bap->isBapMask(C1_TORS))		head->torsion(bap->get(C1_TORS));
  if (bap->isBapMask(C1_TILT))		head->abduct(bap->get(C1_TILT));

  if (bap->isBapMask(C4_ROLL))		neck->flexion(bap->get(C4_ROLL));
  if (bap->isBapMask(C4_TORS))		neck->torsion(bap->get(C4_TORS));
  if (bap->isBapMask(C4_TILT))		neck->abduct(bap->get(C4_TILT));

  if (bap->isBapMask(T1_ROLL))		neck->flexion(bap->get(T1_ROLL));
  if (bap->isBapMask(T1_TORS))		neck->torsion(bap->get(T1_TORS));
  if (bap->isBapMask(T1_TILT))		neck->abduct(bap->get(T1_TILT));
#endif //notused

  if (bap->isBapMask(TR_VERTICAL))	{tz = bap->get(TR_VERTICAL); TRACE("tv", tz);}
  if (bap->isBapMask(TR_LATERAL))	{ty = bap->get(TR_LATERAL); TRACE("tl", ty);}
  if (bap->isBapMask(TR_FRONTAL))	{tx = bap->get(TR_FRONTAL); TRACE("tf", tx);}

  if (bap->isBapMask(RT_BODY_TURN))	{rz = bap->get(RT_BODY_TURN); TRACE("tu", rz);}
  if (bap->isBapMask(RT_BODY_ROLL))	{rx = bap->get(RT_BODY_ROLL); TRACE("ro", rx);}
  if (bap->isBapMask(RT_BODY_TILT))	{ry = bap->get(RT_BODY_TILT); TRACE("ti", ry);}
}

#if 0 //notused
void Body::animReset()
{
  animHead(0, 0);
  animHead(0, 1);
  animHead(0, 2);
  animNeck(0, 0);
  animNeck(0, 1);
  animNeck(0, 2);
  animChest(0, 0);
  animChest(0, 1);
  animChest(0, 2);
  animLeg(0, 0, 0);
  animLeg(0, 1, 0);
  animLeg(0, 0, 1);
  animLeg(0, 1, 1);
  animLeg(0, 0, 2);
  animLeg(0, 1, 2);
  animShin(0, 0, 1);
  animShin(0, 1, 1);
  animShin(0, 0, 2);
  animShin(0, 1, 2);
  animFoot(0, 0, 0);
  animFoot(0, 1, 0);
  animFoot(0, 0, 2);
  animFoot(0, 1, 2);
  animArm(0, 0, 0);
  animArm(0, 1, 0);
  animArm(0, 0, 1);
  animArm(0, 1, 1);
  animArm(0, 0, 2);
  animArm(0, 1, 2);
  animForearm(0, 0, 0);
  animForearm(0, 1, 0);
  animForearm(0, 0, 2);
  animForearm(0, 1, 2);
  animHand(0, 0, 0);
  animHand(0, 1, 0);
  animHand(0, 0, 1);
  animHand(0, 1, 1);
  animHand(0, 0, 2);
  animHand(0, 1, 2);
}

void Body::animHead(float a, uint8_t axis)
{
  echo("Head: %d %.0f", axis, a);
  switch (axis) {
  case 0: head->flexion(a); break;
  case 1: head->abduct(a); break;
  case 2: head->torsion(a); break;
  }
}

void Body::animNeck(float a, uint8_t axis)
{
  echo("Neck: %d %.0f", axis, a);
  switch (axis) {
  case 0: neck->flexion(a); break;
  case 1: neck->abduct(a); break;
  case 2: neck->torsion(a); break;
  }
}

void Body::animChest(float a, uint8_t axis)
{
  echo("Chest: %d %.0f", axis, a);
  switch (axis) {
  case 0: chest->flexion(a); break;
  case 1: chest->abduct(a); break;
  case 2: chest->torsion(a); break;
  }
}

void Body::animLeg(float a, uint8_t side, uint8_t axis)
{
  echo("Leg: %d %d %.0f", side, axis, a);
  switch (side) {
  case 0:
    switch (axis) {
    case 0: thigh_l->flexion(a); break;
    case 1: thigh_l->abduct(a); break;
    case 2: thigh_l->torsion(a); break;
    }
  case 1:
    switch (axis) {
    case 0: thigh_r->flexion(a); break;
    case 1: thigh_r->abduct(a); break;
    case 2: thigh_r->torsion(a); break;
    }
  }
}

void Body::animShin(float a, uint8_t side, uint8_t axis)
{
  echo("Shin: %d %d %.0f", side, axis, a);
  switch (side) {
  case 0:
    switch (axis) {
    case 1: shin_l->flexion(a); break;
    case 2: shin_l->torsion(a); break;
    }
  case 1:
    switch (axis) {
    case 1: shin_r->flexion(a); break;
    case 2: shin_r->torsion(a); break;
    }
  }
}

void Body::animFoot(float a, uint8_t side, uint8_t axis)
{
  echo("Foot: %d %d %.0f", side, axis, a);
  switch (side) {
  case 0:
    switch (axis) {
    case 0: foot_l->flexion(a); break;
    case 2: foot_l->torsion(a); break;
    }
  case 1:
    switch (axis) {
    case 0: foot_r->flexion(a); break;
    case 2: foot_r->torsion(a); break;
    }
  }
}

void Body::animArm(float a, uint8_t side, uint8_t axis)
{
  echo("Arm: %d %d %.0f", side, axis, a);
  switch (side) {
  case 0:
    switch (axis) {
    case 0: arm_l->flexion(a); break;
    case 1: arm_l->abduct(a); break;
    case 2: arm_l->torsion(a); break;
    }
  case 1:
    switch (axis) {
    case 0: arm_r->flexion(a); break;
    case 1: arm_r->abduct(a); break;
    case 2: arm_r->torsion(a); break;
    }
  }
}

void Body::animForearm(float a, uint8_t side, uint8_t axis)
{
  echo("Forearm: %d %d %.0f", side, axis, a);
  switch (side) {
  case 0:
    switch (axis) {
    case 0: forearm_l->flexion(a); break;
    case 2: forearm_l->torsion(a); break;
    }
  case 1:
    switch (axis) {
    case 0: forearm_r->flexion(a); break;
    case 2: forearm_r->torsion(a); break;
    }
  }
}

void Body::animHand(float a, uint8_t side, uint8_t axis)
{
  echo("Hand: %d %d %.0f", side, axis, a);
  switch (side) {
  case 0:
    switch (axis) {
    case 0: hand_l->flexion(a); break;
    case 1: hand_l->abduct(a); break;
    case 2: hand_l->torsion(a); break;
    }
  case 1:
    switch (axis) {
    case 0: hand_r->flexion(a); break;
    case 1: hand_r->abduct(a); break;
    case 2: hand_r->torsion(a); break;
    }
  }
}

/** Animates body bap articulations - called from humanoid.cpp without server */
void Body::anim(int param)
{
  switch (param) {
  case PELVIC_TILT:		chest->abduct(bap->get(PELVIC_TILT)); break;
  case PELVIC_TORS:		chest->torsion(bap->get(PELVIC_TORS)); break;
  case PELVIC_ROLL:		chest->flexion(bap->get(PELVIC_ROLL)); break;

  case L_HIP_FLEX:		thigh_l->flexion(bap->get(L_HIP_FLEX)); break;
  case R_HIP_FLEX:		thigh_r->flexion(bap->get(R_HIP_FLEX)); break;
  case L_HIP_ABDU:		thigh_l->abduct(bap->get(L_HIP_ABDU)); break;
  case R_HIP_ABDU:		thigh_r->abduct(bap->get(R_HIP_ABDU)); break;
  case L_HIP_TORS:		thigh_l->torsion(bap->get(L_HIP_TORS)); break;
  case R_HIP_TORS:		thigh_r->torsion(bap->get(R_HIP_TORS)); break;

  case L_KNEE_FLEX:		shin_l->flexion(bap->get(L_KNEE_FLEX)); break;
  case R_KNEE_FLEX:		shin_r->flexion(bap->get(R_KNEE_FLEX)); break;
  case L_KNEE_TORS:		shin_l->torsion(bap->get(L_KNEE_TORS)); break;
  case R_KNEE_TORS:		shin_r->torsion(bap->get(R_KNEE_TORS)); break;

  case L_ANKLE_FLEX:		foot_l->flexion(bap->get(L_ANKLE_FLEX)); break;
  case R_ANKLE_FLEX:		foot_r->flexion(bap->get(R_ANKLE_FLEX)); break;
  case L_ANKLE_TORS:		foot_l->torsion(bap->get(L_ANKLE_TORS)); break;
  case R_ANKLE_TORS:		foot_r->torsion(bap->get(R_ANKLE_TORS)); break;

  case L_SHOULDER_FLEX:		arm_l->flexion(bap->get(L_SHOULDER_FLEX)); break;
  case R_SHOULDER_FLEX:		arm_r->flexion(bap->get(R_SHOULDER_FLEX)); break;
  case L_SHOULDER_ABDU:		arm_l->abduct(bap->get(L_SHOULDER_ABDU)); break;
  case R_SHOULDER_ABDU:		arm_r->abduct(bap->get(R_SHOULDER_ABDU)); break;
  case L_SHOULDER_TORS:		arm_l->torsion(bap->get(L_SHOULDER_TORS)); break;
  case R_SHOULDER_TORS:		arm_r->torsion(bap->get(R_SHOULDER_TORS)); break;

  case L_ELBOW_FLEX:		forearm_l->flexion(bap->get(L_ELBOW_FLEX)); break;
  case R_ELBOW_FLEX:		forearm_r->flexion(bap->get(R_ELBOW_FLEX)); break;

  case L_WRIST_FLEX:		hand_l->abduct(bap->get(L_WRIST_FLEX)); break;
  case R_WRIST_FLEX:		hand_r->abduct(bap->get(R_WRIST_FLEX)); break;
  case L_WRIST_PIVOT:		hand_l->flexion(bap->get(L_WRIST_PIVOT)); break;
  case R_WRIST_PIVOT:		hand_r->flexion(bap->get(R_WRIST_PIVOT)); break;
  case L_WRIST_TORS:		hand_l->torsion(bap->get(L_WRIST_TORS)); break;
  case R_WRIST_TORS:		hand_r->torsion(bap->get(R_WRIST_TORS)); break;

  case C1_ROLL:			head->flexion(bap->get(C1_ROLL)); break;
  case C1_TILT:			head->abduct(bap->get(C1_TILT)); break;
  case C1_TORS:			head->torsion(bap->get(C1_TORS)); break;

  case C4_ROLL:			neck->flexion(bap->get(C4_ROLL)); break;
  case C4_TILT:			neck->abduct(bap->get(C4_TILT)); break;
  case C4_TORS:			neck->torsion(bap->get(C4_TORS)); break;

  case T1_ROLL:			neck->flexion(bap->get(T1_ROLL)); break;
  case T1_TILT:			neck->abduct(bap->get(T1_TILT)); break;
  case T1_TORS:			neck->torsion(bap->get(T1_TORS)); break;

  case TR_VERTICAL:		tz = bap->get(TR_VERTICAL); break;
  case TR_LATERAL:		ty = bap->get(TR_LATERAL); break;
  case TR_FRONTAL:		tx = bap->get(TR_FRONTAL); break;

  case RT_BODY_TURN:		rz = bap->get(RT_BODY_TURN); break;
  case RT_BODY_ROLL:		rx = bap->get(RT_BODY_ROLL); break;
  case RT_BODY_TILT:		ry = bap->get(RT_BODY_TILT); break;
  }
}
#endif //notused

/** Checks if body part is loaded */
bool Body::isLoaded(uint8_t part)
{
  if (part < MAX_PARTS)
    return bodyparts[part].loaded;
  return false;
}

/** Translates positive */
void Body::transP(uint8_t part)
{
  if (part < MAX_PARTS) {
    if (model == MODEL_OBJ)
      glTranslatef(jp.y[part], jp.x[part], jp.z[part]);
    else
      glTranslatef(jp.x[part], jp.y[part], jp.z[part]);
  }
}

/** Translates negative */
void Body::transN(uint8_t part)
{
  if (part < MAX_PARTS) {
    if (model == MODEL_OBJ)
      glTranslatef(-jp.y[part], -jp.x[part], -jp.z[part]);
    else
      glTranslatef(-jp.x[part], -jp.y[part], -jp.z[part]);
  }
}

/** Rotates around X axis */
void Body::rotatX(int param)
{
  int sign = (param >= 0) ?1:-1;
  switch (model) {
  case MODEL_OFF: glRotatef( sign * bap->get(abs(param)), 1,0,0); break;
  case MODEL_OBJ: glRotatef(+sign * bap->get(abs(param)), 0,1,0); break;
  }
}

/** Rotates around Y axis */
void Body::rotatY(int param)
{
  int sign = (param >= 0) ?1:-1;
  switch (model) {
  case MODEL_OFF: glRotatef(sign * bap->get(abs(param)), 0,1,0); break;
  case MODEL_OBJ: glRotatef(sign * bap->get(abs(param)), 1,0,0); break;
  }
}

/** Rotates around Z axis */
void Body::rotatZ(int param)
{
  int sign = (param >= 0) ?1:-1;
  glRotatef(sign * bap->get(abs(param)), 0,0,1);
}

/** Renders a part of the body */
void Body::render(uint8_t part)
{
  glPushMatrix();
  if (part < MAX_PARTS) {
    glMaterialfv(GL_FRONT, GL_DIFFUSE, bodyparts[part].color);
    if (bodyparts[part].texid) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, bodyparts[part].texid);
    }
    else {
      glColor3fv(bodyparts[part].color);
    }

    glCallList(dlist + part);	// displays this part

    glDisable(GL_TEXTURE_2D);
  }
  glPopMatrix();
}

/** Renders body and face */
void Body::render(Pos& pos)
{
  if (! bap) return;

  const float color[] = {.4,.4,.4,1};

  glPushMatrix();	// general
   glTranslatef(pos.x + tx, pos.y + ty, pos.z + tz);
   switch (model) {
   case MODEL_OBJ:
     glRotatef(RAD2DEG(pos.ax) + rx, 0,1,0);
     glRotatef(RAD2DEG(pos.ay) + ry, 1,0,0);
     glRotatef(RAD2DEG(pos.az) + rz - 90, 0,0,1);
     break;
   case MODEL_OFF:
   default:
     glRotatef(RAD2DEG(pos.ax) + rx, 1,0,0);
     glRotatef(RAD2DEG(pos.ay) + ry, 0,1,0);
     glRotatef(RAD2DEG(pos.az) + rz + 180, 0,0,1);
     break;
   }
   glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
   glColorMaterial(GL_FRONT, GL_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);

   // Pelvic
   glPushMatrix();	// pelvic
    transP(PELVIC);
    rotatX(PELVIC_TILT);
    rotatY(PELVIC_ROLL);
    rotatZ(PELVIC_TORS);
    transN(PELVIC);
    render(HIPS);

    // Abdomen
    if (isLoaded(ABDOMEN)) {
      glPushMatrix();
      render(ABDOMEN);
      glPopMatrix();
    }
    // Skirt
    if (isLoaded(SKIRT)) {
      glPushMatrix();
      render(SKIRT);
      glPopMatrix();
    }
    // Belt
    if (isLoaded(BELT)) {
      glPushMatrix();
      render(BELT);
      glPopMatrix();
    }
    // Chest
    glPushMatrix();	//  Spinal -> Chest (thoracic level 5)
     transP(SPINAL);
     rotatX(T5_TILT);
     rotatY(T5_ROLL);
     rotatZ(T5_TORS);
     transN(SPINAL);
     render(CHEST);
     // Collar
     if (isLoaded(L_COLLAR)) {
       glPushMatrix();
       render(L_COLLAR);
       glPopMatrix();
     }
     if (isLoaded(R_COLLAR)) {
       glPushMatrix();
       render(R_COLLAR);
       glPopMatrix();
     }
     // Lower Neck
     glPushMatrix();	//  Lower Neck (cervical level 4)
      transP(LOWER_NECK);
      rotatX(C4_TILT);
      rotatY(C4_ROLL);
      rotatZ(C4_TORS);
      transN(LOWER_NECK);
      render(NECK);
      // Head
      if (! face) {
        //echo("head render");
        render(HEAD);
      }

      // Upper Neck
      glPushMatrix();	//  Upper Neck -> Head (cervical level 1)
       transP(UPPER_NECK);
       rotatX(C1_TILT);
       rotatY(C1_ROLL);
       rotatZ(C1_TORS);
       if (face) {
         glScalef(Face::SCALE, Face::SCALE, Face::SCALE);
         glTranslatef(0, 0.9, -0.9);
         glRotatef(90, 1,0,0);

         //echo("face render");
         face->render();	// YR

       }
       transP(UPPER_NECK);
       rotatX(C1_TILT);
       rotatY(C1_ROLL);
       rotatZ(C1_TORS);
       transN(UPPER_NECK);
      glPopMatrix(); 	// head
     glPopMatrix(); 	// neck

     //
     // Left side
     //

     // Left arm
     glPushMatrix();	//  Left Shoulder -> Left Arm
      transP(L_SHOULDER);
      if (model == MODEL_OFF)
        glRotatef(-90, 0,0,1);  //OK but FIXME
      rotatX(L_SHOULDER_ABDU);
      rotatY(+L_SHOULDER_FLEX);	// - why ???
      rotatZ(L_SHOULDER_TORS);	//
      transN(L_SHOULDER);
      render(L_ARM);

      // Left forearm
      glPushMatrix();	//  Left Elbow -> Left Forearm
       transP(L_ELBOW);
       rotatY(+L_ELBOW_FLEX);	// - why ???
       rotatZ(L_ELBOW_TORS);
       transN(L_ELBOW);
       render(L_FOREARM);

       // Left hand
       glPushMatrix();	//  Left Wrist -> Left Hand
        transP(L_WRIST);
        rotatX(L_WRIST_FLEX);
        rotatY(L_WRIST_PIVOT);
        rotatZ(L_WRIST_TORS);
        transN(L_WRIST);
        render(L_HAND);

#if 0 // left fingers
        glPushMatrix();	//  Left fingers
         glPushMatrix();	//  Left thumb
          transP(L_THUMB);
          rotatX(L_THUMB1_FLEX);
          transN(L_THUMB);
          render(L_THUMB);
         glPopMatrix();
         glPushMatrix();	//  Left index
          transP(L_INDEX);
          rotatX(L_INDEX1_FLEX);
          transN(L_INDEX);
          render(L_INDEX);
         glPopMatrix();
         glPushMatrix();	//  Left middle
          transP(L_MIDDLE);
          rotatX(L_MIDDLE1_FLEX);
          transN(L_MIDDLE);
          render(L_MIDDLE);
         glPopMatrix();
         glPushMatrix();	//  Left ring
          transP(L_RING);
          rotatX(L_RING1_FLEX);
          transN(L_RING);
          render(L_RING);
         glPopMatrix();
         glPushMatrix();	//  Left pinky
          transP(L_PINKY);
	  rotatX(L_PINKY1_FLEX);
          transN(L_PINKY);
          render(L_PINKY);
         glPopMatrix();
        glPopMatrix();	// left fingers
#endif // left fingers

       glPopMatrix();	// l_wrist
      glPopMatrix();	// l_elbow
     glPopMatrix();	// l_shoulder

     //
     // Right side
     //

     // Right arm
     glPushMatrix();	//  Right Shoulder -> Right Arm
      transP(R_SHOULDER);
      if (model == MODEL_OFF)
        glRotatef(90, 0,0,1);	//OK but FIXME
      rotatX(+R_SHOULDER_ABDU);	// -
      rotatY(R_SHOULDER_FLEX);
      rotatZ(+R_SHOULDER_TORS);	// -
      transN(R_SHOULDER);
      render(R_ARM);

      // Right forearm
      glPushMatrix();	//  Right Elbow -> Right Forearm
       transP(R_ELBOW);
       rotatY(R_ELBOW_FLEX);
       rotatZ(+R_ELBOW_TORS);	// -
       transN(R_ELBOW);
       render(R_FOREARM);

       // Right hand
       glPushMatrix();	//  Right Wrist -> Right Hand
        transP(R_WRIST);
        rotatX(R_WRIST_FLEX);
        rotatY(R_WRIST_PIVOT);
        rotatZ(R_WRIST_TORS);
        transN(R_WRIST);
        render(R_HAND);

#if 0 // right fingers
        glPushMatrix();	//  Right fingers
         glPushMatrix();	//  Right thumb
          transP(R_THUMB);
          rotatX(R_THUMB1_FLEX);
          transN(R_THUMB);
          render(R_THUMB);
         glPopMatrix();
         glPushMatrix();	//  Right index
          transP(R_INDEX);
          rotatX(R_INDEX1_FLEX);
          transN(R_INDEX);
          render(R_INDEX);
         glPopMatrix();
         glPushMatrix();	//  Right middle
          transP(R_MIDDLE);
          rotatX(R_MIDDLE1_FLEX);
          transN(R_MIDDLE);
          render(R_MIDDLE);
         glPopMatrix();
         glPushMatrix();	//  Right ring
          transP(R_RING);
          rotatX(R_RING1_FLEX);
          transN(R_RING);
          render(R_RING);
         glPopMatrix();
         glPushMatrix();	//  Right pinky
          transP(R_PINKY);
          rotatX(R_PINKY1_FLEX);
          transN(R_PINKY);
          render(R_PINKY);
         glPopMatrix();
        glPopMatrix();	// right fingers
#endif // right fingers

       glPopMatrix();	// r_wrist
      glPopMatrix();	// r_elbow
     glPopMatrix();	// r_shoulder
    glPopMatrix();	// spinal

    //
    // bottom
    //

    // Left thigh
    glPushMatrix();	//  Left Hip -> Left Thigh
    transP(L_HIP);
    rotatX(L_HIP_FLEX);
    rotatY(L_HIP_ABDU);
    rotatZ(L_HIP_TORS);
    transN(L_HIP);
    render(L_THIGH);

    // Left shin
    glPushMatrix();	//  Left Knee -> Left Shin
     transP(L_KNEE);
     rotatX(L_KNEE_FLEX);
     rotatZ(L_KNEE_TORS);
     transN(L_KNEE);
     render(L_SHIN);

     // Left foot
     glPushMatrix();	//  Left Ankle -> Left Foot
      transP(L_ANKLE);
      rotatX(L_ANKLE_FLEX);
      rotatZ(L_ANKLE_TORS);
      transN(L_ANKLE);
      render(L_FOOT);
     glPopMatrix();	// l_ankle
    glPopMatrix();	// l_knee
   glPopMatrix();	// l_thigh

   // Right thigh
   glPushMatrix();	//  Right Hip -> Right Thigh
    transP(R_HIP);
    rotatX(R_HIP_FLEX);
    rotatY(R_HIP_ABDU);
    rotatZ(R_HIP_TORS);
    transN(R_HIP);
    render(R_THIGH);

    // Right shin
    glPushMatrix();	//  Right Knee -> Right Shin
     transP(R_KNEE);
     rotatX(R_KNEE_FLEX);
     rotatZ(R_KNEE_TORS);
     transN(R_KNEE);
     render(R_SHIN);

     // Right foot
     glPushMatrix();	//  Right Ankle -> Right Foot
      transP(R_ANKLE);
      rotatX(R_ANKLE_FLEX);
      rotatZ(R_ANKLE_TORS);
      transN(R_ANKLE);
      render(R_FOOT);
     glPopMatrix();	// r_ankle
    glPopMatrix();	// r_knee
   glPopMatrix();	// r_hip
  glPopMatrix();	// pelvic

  glDisable(GL_COLOR_MATERIAL);
  glPopMatrix();
}
