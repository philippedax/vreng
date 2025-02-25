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
#include "parse.hpp"	// errorAtLine

// macro
#define TRACE(s,v) if(::g.pref.trace) echo("%s: %.0f", s,v);


// default const
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

/////////////////////////////////////////////////////////
//
// syntax of data/body/avatar.body :
//
// <body>
// <part jp="x y z" url="/mdl/part.mdl"/>
// < ... />
// </body>
//
// see examples: data/body/man.body, data/body/woman.body
//
/////////////////////////////////////////////////////////

static const struct sBodyToken btokens[] = {
  { "head",		Body::BD_HEAD },
  { "neck",		Body::BD_NECK },
  { "chest",		Body::BD_CHEST },
  { "hip",		Body::BD_PELVIC },
  { "shoulder-l",	Body::BD_ARM_L },
  { "forearm-l",	Body::BD_FARM_L },
  { "hand-l",		Body::BD_HAND_L },
  { "shoulder-r",	Body::BD_ARM_R },
  { "forearm-r",	Body::BD_FARM_R },
  { "hand-r",		Body::BD_HAND_R },
  { "thigh-l",		Body::BD_THIGH_L },
  { "shin-l",		Body::BD_SHIN_L },
  { "foot-l",		Body::BD_FOOT_L },
  { "thigh-r",		Body::BD_THIGH_R },
  { "shin-r",		Body::BD_SHIN_R },
  { "foot-r",		Body::BD_FOOT_R },
  { "abdomen",		Body::BD_ABDOMEN },
  { "collar-l",		Body::BD_COLLAR_L },
  { "collar-r",		Body::BD_COLLAR_R },
  { "belt",		Body::BD_BELT },
  { "thumb-l",		Body::BD_THUMB_L },
  { "index-l",		Body::BD_INDEX_L },
  { "middle-l",		Body::BD_MIDDLE_L },
  { "ring-l",		Body::BD_RING_L },
  { "pinky-l",		Body::BD_PINKY_L },
  { "thumb-r",		Body::BD_THUMB_R },
  { "index-r",		Body::BD_INDEX_R },
  { "middle-r",		Body::BD_MIDDLE_R },
  { "ring-r",		Body::BD_RING_R },
  { "pinky-r",		Body::BD_PINKY_R },
  { "skirt",		Body::BD_SKIRT },
  //
  // compatibility with old syntax
  // in the case where exist in cache
  //
  { "lShldr",		Body::BD_ARM_L },
  { "lForearm",		Body::BD_FARM_L },
  { "lHand",		Body::BD_HAND_L },
  { "rShldr",		Body::BD_ARM_R },
  { "rForearm",		Body::BD_FARM_R },
  { "rHand",		Body::BD_HAND_R },
  { "lThigh",		Body::BD_THIGH_L },
  { "lShin",		Body::BD_SHIN_L },
  { "lFoot",		Body::BD_FOOT_L },
  { "rThigh",		Body::BD_THIGH_R },
  { "rShin",		Body::BD_SHIN_R },
  { "rFoot",		Body::BD_FOOT_R },
  { "lCollar",		Body::BD_COLLAR_L },
  { "rCollar",		Body::BD_COLLAR_R },
  { "lThumb",		Body::BD_THUMB_L },
  { "lIndex",		Body::BD_INDEX_L },
  { "lMiddle",		Body::BD_MIDDLE_L },
  { "lRing",		Body::BD_RING_L },
  { "lPinky",		Body::BD_PINKY_L },
  { "rThumb",		Body::BD_THUMB_R },
  { "rIndex",		Body::BD_INDEX_R },
  { "rMiddle",		Body::BD_MIDDLE_R },
  { "rRing",		Body::BD_RING_R },
  { "rPinky",		Body::BD_PINKY_R },
  { "/body",		Body::BD_PARTS /* bad -1 */ } // James Addison C++14 compatible
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
  bd_parts = NULL;
  bd_draws = 0;
  bd_model = 0;
  url = NULL;
  face = NULL;
  jp.x = NULL;
  jp.y = NULL;
  jp.z = NULL;
  bd_tx = bd_ty = bd_tz = 0;	// position in space
  bd_rx = bd_ry = bd_rz = 0;	// orientation in space
  bd_scale = BD_SCALE;		// scale factor

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
    bd_skin[i] = 1;
    bd_cloth[i] = .5;
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

  if (bd_dlist > 0) glDeleteLists(bd_dlist, bd_draws);
  if (bd_parts) delete[] bd_parts;
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
  bd_parts = new sBodyParts[BD_PARTS];
  jp.x = new float[BD_PARTS];
  jp.y = new float[BD_PARTS];
  jp.z = new float[BD_PARTS];

  // init bodyparts
  for (int i=0; i < BD_PARTS; i++) {
    bd_parts[i].loaded = false;
    bd_parts[i].scale = 1;
    for (int j=0; j<3 ; j++) bd_parts[i].scales[j] = 1;
    for (int j=0; j<3 ; j++) bd_parts[i].color[j] = bd_cloth[j];
    bd_parts[i].texid = 0;
    bd_parts[i].off = NULL;
    bd_parts[i].obj = NULL;
    bd_parts[i].url[0] = '\0';
    bd_parts[i].texurl[0] = '\0';
  }
  // set default color
  for (int j=0; j<3 ; j++) {
    bd_parts[BD_HEAD].color[j] = bd_skin[j];
    bd_parts[BD_NECK].color[j] = bd_skin[j];
    bd_parts[BD_FARM_L].color[j] = bd_skin[j];
    bd_parts[BD_FARM_R].color[j] = bd_skin[j];
    bd_parts[BD_HAND_L].color[j] = bd_skin[j];
    bd_parts[BD_HAND_R].color[j] = bd_skin[j];
  }

  // parse first line of <body ...>
  if (! fgets(jpline, sizeof(jpline), f)) return;
  jpline[strlen(jpline) - 1] = '\0';
  if (! stringcmp(jpline, "<body")) {
    l = strtok(jpline, TOK_SEP);	// tokenize jpline
    l = strtok(NULL, TOK_SEP);		// next token
    while (l) {
      if      (! stringcmp(l, "scale="))
        l = object->parse()->parseFloat(l, &bd_scale, "scale");
      else if (! stringcmp(l, "tex="))
        l = object->parse()->parseString(l, tex, "tex");
    }
  }

  if (*tex) {
    for (int i=0; i < BD_PARTS; i++) {
      strcpy(bd_parts[i].texurl, tex);
    }
  }

  // get body joint points and urls
  for (int i=0; i < BD_PARTS; i++) {
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
          jp.x[bpindex] = static_cast<float>(atof(l) / bd_scale); l = strtok(NULL, TOK_SEP);
          jp.y[bpindex] = static_cast<float>(atof(l) / bd_scale); l = strtok(NULL, TOK_SEP);
          jp.z[bpindex] = static_cast<float>(atof(l) / bd_scale); l = strtok(NULL, TOK_SEP);
        }
        else if (*l == '/') break;  // eol
        else if (! stringcmp(l, "url=")) {
          l = object->parse()->parseString(l, bd_parts[i].url, "url");
          bd_draws++;
        }
        else if (! stringcmp(l, "scale=")) {
          l = object->parse()->parseFloat(l, &bd_parts[i].scale, "scale");
          bd_parts[i].scales[0] = bd_parts[i].scale;
          bd_parts[i].scales[1] = bd_parts[i].scale;
          bd_parts[i].scales[2] = bd_parts[i].scale;
        }
        else if (! stringcmp(l, "scalex="))
          l = object->parse()->parseFloat(l, &bd_parts[i].scales[0],"scalex");
        else if (! stringcmp(l, "scaley="))
          l = object->parse()->parseFloat(l, &bd_parts[i].scales[1],"scaley");
        else if (! stringcmp(l, "scalez="))
          l = object->parse()->parseFloat(l, &bd_parts[i].scales[2],"scalez");
        else if (! stringcmp(l, "color="))
          l = object->parse()->parseVector3f(l, bd_parts[i].color, "color");
        else if (! stringcmp(l, "tex="))
          l = object->parse()->parseString(l, bd_parts[i].texurl, "tex");
        else {
          object->parse()->errorAtLine(l);
          l = object->parse()->nextToken();
          break;
        }
      }
      end_while_parse(l);
    }
  }

endloadbody:
  // now we can download all parts
  for (int i=0; i < BD_PARTS; i++) {
    if (bd_parts[i].url[0]) {  // if url exist
      bd_parts[i].model = Format::getModelByUrl(bd_parts[i].url);

      switch (bd_parts[i].model) {
      case MODEL_OFF:
        bd_parts[i].off = new Off(bd_parts[i].url);
        Http::httpOpen(bd_parts[i].url, Off::reader, bd_parts[i].off, 0);
        break;
      case MODEL_OBJ:
        bd_parts[i].obj = new Obj(bd_parts[i].url, 1);
        Http::httpOpen(bd_parts[i].url, Obj::reader, bd_parts[i].obj, 0);
        bd_parts[i].obj->setScale(bd_scale * bd_parts[i].scale);
        if (bd_parts[i].texurl[0]) { // if url exist
          bd_parts[i].texid = Texture::open(bd_parts[i].texurl);
        }
        break;
      }
      bd_parts[i].loaded = true;
    }
  }
  bd_model = bd_parts[0].model;  // keep model used
}

/** Draws body parts */
void Body::draw()
{
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

  bd_dlist = glGenLists(bd_draws);
  for (int i=0; i < bd_draws && bd_parts[i].loaded ; i++) {
    glNewList(bd_dlist + i, GL_COMPILE);
    switch (bd_parts[i].model) {
      case MODEL_OFF: bd_parts[i].off->draw(); break;
      case MODEL_OBJ: bd_parts[i].obj->draw(); break;
    }
    glEndList();
  }
}

/** Sets skin and cloth colors - called from humanoid */
void Body::setColors(float *_skin, float *_cloth)
{
  for (int i=0; i<3; i++) {
    bd_skin[i] = _skin[i];
    bd_cloth[i] = _cloth[i];
    bd_color[i] = _cloth[i];
  }
}

/** Animates all body's articulations - called from humanoid.cpp
 * usually not well exploited, should be notused !!!
 */
void Body::play()
{
  if (! bap) return;

  if (bap->isBapMask(PELVIC_TILT))	chest->abduct(bap->get(PELVIC_TILT));
  if (bap->isBapMask(PELVIC_TORS))	chest->torsion(bap->get(PELVIC_TORS));
  if (bap->isBapMask(PELVIC_ROLL))	chest->flexion(bap->get(PELVIC_ROLL));

  if (bap->isBapMask(HIP_L_FLEX))	thigh_l->flexion(bap->get(HIP_L_FLEX));
  if (bap->isBapMask(HIP_R_FLEX))	thigh_r->flexion(bap->get(HIP_R_FLEX));
  if (bap->isBapMask(HIP_L_ABDU))	thigh_l->abduct(bap->get(HIP_L_ABDU));
  if (bap->isBapMask(HIP_R_ABDU))	thigh_r->abduct(bap->get(HIP_R_ABDU));
  if (bap->isBapMask(HIP_L_TORS))	thigh_l->torsion(bap->get(HIP_L_TORS));
  if (bap->isBapMask(HIP_R_TORS))	thigh_r->torsion(bap->get(HIP_R_TORS));

  if (bap->isBapMask(KNEE_L_FLEX))	shin_l->flexion(bap->get(KNEE_L_FLEX));
  if (bap->isBapMask(KNEE_R_FLEX))	shin_r->flexion(bap->get(KNEE_R_FLEX));
  if (bap->isBapMask(KNEE_L_TORS))	shin_l->torsion(bap->get(KNEE_L_TORS));
  if (bap->isBapMask(KNEE_R_TORS))	shin_r->torsion(bap->get(KNEE_R_TORS));

  if (bap->isBapMask(ANKLE_L_FLEX))	foot_l->flexion(bap->get(ANKLE_L_FLEX));
  if (bap->isBapMask(ANKLE_R_FLEX))	foot_r->flexion(bap->get(ANKLE_R_FLEX));
  if (bap->isBapMask(ANKLE_L_TORS))	foot_l->torsion(bap->get(ANKLE_L_TORS));
  if (bap->isBapMask(ANKLE_R_TORS))	foot_r->torsion(bap->get(ANKLE_R_TORS));

  if (bap->isBapMask(SHOULDER_L_FLEX))	arm_l->flexion(bap->get(SHOULDER_L_FLEX));
  if (bap->isBapMask(SHOULDER_R_FLEX))	arm_r->flexion(bap->get(SHOULDER_R_FLEX));
  if (bap->isBapMask(SHOULDER_L_ABDU))	arm_l->abduct(bap->get(SHOULDER_L_ABDU));
  if (bap->isBapMask(SHOULDER_R_ABDU))	arm_r->abduct(bap->get(SHOULDER_R_ABDU));
  if (bap->isBapMask(SHOULDER_L_TORS))	arm_l->torsion(bap->get(SHOULDER_L_TORS));
  if (bap->isBapMask(SHOULDER_R_TORS))	arm_r->torsion(bap->get(SHOULDER_R_TORS));

  if (bap->isBapMask(ELBOW_L_FLEX))	forearm_l->flexion(bap->get(ELBOW_L_FLEX));
  if (bap->isBapMask(ELBOW_R_FLEX))	forearm_r->flexion(bap->get(ELBOW_R_FLEX));

  if (bap->isBapMask(WRIST_L_FLEX))	hand_l->abduct(bap->get(WRIST_L_FLEX));
  if (bap->isBapMask(WRIST_R_FLEX))	hand_r->abduct(bap->get(WRIST_R_FLEX));
  if (bap->isBapMask(WRIST_L_PIVOT))	hand_l->flexion(bap->get(WRIST_L_PIVOT));
  if (bap->isBapMask(WRIST_R_PIVOT))	hand_r->flexion(bap->get(WRIST_R_PIVOT));
  if (bap->isBapMask(WRIST_L_TORS))	hand_l->torsion(bap->get(WRIST_L_TORS));
  if (bap->isBapMask(WRIST_R_TORS))	hand_r->torsion(bap->get(WRIST_R_TORS));

  if (bap->isBapMask(THUMB1_L_FLEX))	fingers_l[0]->flexion(bap->get(THUMB1_L_FLEX));
  if (bap->isBapMask(THUMB1_R_FLEX))	fingers_r[0]->flexion(bap->get(THUMB1_R_FLEX));
  if (bap->isBapMask(INDEX1_L_FLEX))	fingers_l[1]->flexion(bap->get(INDEX1_L_FLEX));
  if (bap->isBapMask(INDEX1_R_FLEX))	fingers_r[1]->flexion(bap->get(INDEX1_R_FLEX));
  if (bap->isBapMask(MIDDLE1_L_FLEX))	fingers_l[2]->flexion(bap->get(MIDDLE1_L_FLEX));
  if (bap->isBapMask(MIDDLE1_R_FLEX))	fingers_r[2]->flexion(bap->get(MIDDLE1_R_FLEX));
  if (bap->isBapMask(RING1_L_FLEX))	fingers_l[3]->flexion(bap->get(RING1_L_FLEX));
  if (bap->isBapMask(RING1_R_FLEX))	fingers_r[3]->flexion(bap->get(RING1_R_FLEX));
  if (bap->isBapMask(PINKY1_L_FLEX))	fingers_l[4]->flexion(bap->get(PINKY1_L_FLEX));
  if (bap->isBapMask(PINKY1_R_FLEX))	fingers_r[4]->flexion(bap->get(PINKY1_R_FLEX));

  if (bap->isBapMask(C1_ROLL))		head->flexion(bap->get(C1_ROLL));
  if (bap->isBapMask(C1_TORS))		head->torsion(bap->get(C1_TORS));
  if (bap->isBapMask(C1_TILT))		head->abduct(bap->get(C1_TILT));

  if (bap->isBapMask(C4_ROLL))		neck->flexion(bap->get(C4_ROLL));
  if (bap->isBapMask(C4_TORS))		neck->torsion(bap->get(C4_TORS));
  if (bap->isBapMask(C4_TILT))		neck->abduct(bap->get(C4_TILT));

  if (bap->isBapMask(T1_ROLL))		neck->flexion(bap->get(T1_ROLL));
  if (bap->isBapMask(T1_TORS))		neck->torsion(bap->get(T1_TORS));
  if (bap->isBapMask(T1_TILT))		neck->abduct(bap->get(T1_TILT));

  if (bap->isBapMask(TR_VERTICAL))	{bd_tz = bap->get(TR_VERTICAL); TRACE("tv", bd_tz);}
  if (bap->isBapMask(TR_LATERAL))	{bd_ty = bap->get(TR_LATERAL); TRACE("tl", bd_ty);}
  if (bap->isBapMask(TR_FRONTAL))	{bd_tx = bap->get(TR_FRONTAL); TRACE("tf", bd_tx);}

  if (bap->isBapMask(RT_BODY_TURN))	{bd_rz = bap->get(RT_BODY_TURN); TRACE("tu", bd_rz);}
  if (bap->isBapMask(RT_BODY_ROLL))	{bd_rx = bap->get(RT_BODY_ROLL); TRACE("ro", bd_rx);}
  if (bap->isBapMask(RT_BODY_TILT))	{bd_ry = bap->get(RT_BODY_TILT); TRACE("ti", bd_ry);}
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
#endif //notused

/** Checks if body part is loaded */
bool Body::isPart(uint8_t part)
{
  if (part < BD_PARTS)
    return bd_parts[part].loaded;
  return false;
}

/** Translates */
void Body::jpT(int part)
{
  int sign = (part >= 0) ?1:-1;
  if (abs(part) < BD_PARTS) {
    if (bd_model == MODEL_OBJ)
      glTranslatef(sign * jp.y[abs(part)], sign * jp.x[abs(part)], sign * jp.z[abs(part)]);
    else
      glTranslatef(sign * jp.x[abs(part)], sign * jp.y[abs(part)], sign * jp.z[abs(part)]);
  }
}

/** Rotates around X axis */
void Body::jpRX(int param)
{
  int sign = (param >= 0) ?1:-1;
  switch (bd_model) {
  case MODEL_OFF: glRotatef( sign * bap->get(abs(param)), 1,0,0); break;
  case MODEL_OBJ: glRotatef(+sign * bap->get(abs(param)), 0,1,0); break;
  }
}

/** Rotates around Y axis */
void Body::jpRY(int param)
{
  int sign = (param >= 0) ?1:-1;
  switch (bd_model) {
  case MODEL_OFF: glRotatef(sign * bap->get(abs(param)), 0,1,0); break;
  case MODEL_OBJ: glRotatef(sign * bap->get(abs(param)), 1,0,0); break;
  }
}

/** Rotates around Z axis */
void Body::jpRZ(int param)
{
  int sign = (param >= 0) ?1:-1;
  glRotatef(sign * bap->get(abs(param)) +0, 0,0,1);
}

/** Renders a part of the body */
void Body::render(uint8_t part)
{
  if (! isPart(part)) return;

  glPushMatrix();
  glMaterialfv(GL_FRONT, GL_DIFFUSE, bd_parts[part].color);
  if (bd_parts[part].texid) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bd_parts[part].texid);
  }
  else {
    glColor3fv(bd_parts[part].color);
  }

  glCallList(bd_dlist + part);	// displays this part

  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

/** Renders body and face */
void Body::render(Pos& pos)
{
  if (! bap) return;

  const float color[] = {.4,.4,.4,1};

  glPushMatrix();	// body
   glTranslatef(pos.x + bd_tx, pos.y + bd_ty, pos.z + bd_tz);
   switch (bd_model) {
   case MODEL_OBJ:
     glRotatef(RAD2DEG(pos.ax) + bd_rx, 0,1,0);
     glRotatef(RAD2DEG(pos.ay) + bd_ry, 1,0,0);
     glRotatef(RAD2DEG(pos.az) + bd_rz - 90, 0,0,1);
     break;
   case MODEL_OFF:
   default:
     glRotatef(RAD2DEG(pos.ax) + bd_rx, 1,0,0);
     glRotatef(RAD2DEG(pos.ay) + bd_ry, 0,1,0);
     glRotatef(RAD2DEG(pos.az) + bd_rz + 180, 0,0,1);
     break;
   }
   glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
   glColorMaterial(GL_FRONT, GL_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);

   // Pelvic
   glPushMatrix();	// pelvic
    jpT(JP_PELVIC);
     jpRX(PELVIC_TILT);
     jpRY(PELVIC_ROLL);
     jpRZ(PELVIC_TORS);
    jpT(-JP_PELVIC);
    render(BD_PELVIC);

    // Abdomen
    render(BD_ABDOMEN);
    // Skirt
    render(BD_SKIRT);
    // Belt
    render(BD_BELT);

    // Chest
    glPushMatrix();	// spinal -> chest (thoracic level 5)
     jpT(JP_SPINAL);
      jpRX(T5_TILT);
      jpRY(T5_ROLL);
      jpRZ(T5_TORS);
     jpT(-JP_SPINAL);
     render(BD_CHEST);

     // Collar
     render(BD_COLLAR_L);
     render(BD_COLLAR_R);

     // Lower Neck
     glPushMatrix();	// lower neck (cervical level 4)
      jpT(JP_LOWER_NECK);
       jpRX(C4_TILT);
       jpRY(C4_ROLL);
       jpRZ(C4_TORS);
      jpT(-JP_LOWER_NECK);
      render(BD_NECK);

      // Head
      if (! face) {
        //echo("head render");
        render(BD_HEAD);
      }

      // Head - Upper Neck
      glPushMatrix();	// upper neck -> head (cervical level 1)
       jpT(JP_UPPER_NECK);
        jpRX(C1_TILT);
        jpRY(C1_ROLL);
        jpRZ(C1_TORS);
       if (face) {
         glPushMatrix();	// face
          glScalef(Face::SCALE, Face::SCALE, Face::SCALE);
          glTranslatef(0, 0.9, -0.9);
          glRotatef(90, 1,0,0);
          //echo("face render");
          face->render();	// (YR)
         glPopMatrix(); 	// face
       }
       jpT(-JP_UPPER_NECK);
      glPopMatrix(); 	// neck

      //
      // Left side
      //

      glPushMatrix();	// left shoulder -> left arm
       jpT(JP_SHOULDER_L);
       if (bd_model == MODEL_OFF)
         glRotatef(-90, 0,0,1);  //OK but FIXME
        jpRX(SHOULDER_L_ABDU);
        jpRY(-SHOULDER_L_FLEX);	// - why ???
        jpRZ(-SHOULDER_L_TORS);	// - why ???
       jpT(-JP_SHOULDER_L);
       render(BD_ARM_L);

       // Left forearm
       glPushMatrix();	// left elbow -> left forearm
        jpT(JP_ELBOW_L);
         jpRY(-ELBOW_L_FLEX);	// - why ???
         jpRZ(ELBOW_L_TORS);
        jpT(-JP_ELBOW_L);
        render(BD_FARM_L);

        // Left hand
        glPushMatrix();	// left wrist -> left hand
         jpT(JP_WRIST_L);
          jpRX(-WRIST_L_FLEX);	// - why ???
          jpRY(WRIST_L_PIVOT);
          jpRZ(WRIST_L_TORS);
         jpT(-JP_WRIST_L);
         render(BD_HAND_L);

#if 1 // left fingers
          glPushMatrix();	// left thumb
           jpT(JP_WRIST_L);
            jpRX(THUMB1_L_FLEX);
           jpT(-JP_WRIST_L);
           render(BD_THUMB_L);
          glPopMatrix();
          glPushMatrix();	// left index
           jpT(JP_WRIST_L);
            jpRX(INDEX1_L_FLEX);
           jpT(-JP_WRIST_L);
           render(BD_INDEX_L);
          glPopMatrix();
          glPushMatrix();	// left middle
           jpT(JP_WRIST_L);
            jpRX(MIDDLE1_L_FLEX);
           jpT(-JP_WRIST_L);
           render(BD_MIDDLE_L);
          glPopMatrix();
          glPushMatrix();	// left ring
           jpT(JP_WRIST_L);
            jpRX(RING1_L_FLEX);
           jpT(-JP_WRIST_L);
           render(BD_RING_L);
          glPopMatrix();
          glPushMatrix();	// left pinky
           jpT(JP_WRIST_L);
	    jpRX(PINKY1_L_FLEX);
           jpT(-JP_WRIST_L);
           render(BD_PINKY_L);
          glPopMatrix();	// left pinky
#endif // left fingers

        glPopMatrix();	// l_wrist
       glPopMatrix();	// l_elbow
      glPopMatrix();	// l_shoulder

      //
      // Right side
      //

      glPushMatrix();	// right shoulder -> right arm
       jpT(JP_SHOULDER_R);
       if (bd_model == MODEL_OFF)
         glRotatef(90, 0,0,1);	//OK but FIXME
        jpRX(-SHOULDER_R_ABDU);	// - why ???
        jpRY(SHOULDER_R_FLEX);
        jpRZ(-SHOULDER_R_TORS);	// - why ???
       jpT(-JP_SHOULDER_R);
       render(BD_ARM_R);

       // Right forearm
       glPushMatrix();	// right elbow -> right forearm
        jpT(JP_ELBOW_R);
         jpRY(ELBOW_R_FLEX);
         jpRZ(-ELBOW_R_TORS);	// - why ???
        jpT(-JP_ELBOW_R);
        render(BD_FARM_R);

        // Right hand
        glPushMatrix();	// right wrist -> right hand
         jpT(JP_WRIST_R);
          jpRX(-WRIST_R_FLEX);	// - why ???
          jpRY(WRIST_R_PIVOT);
          jpRZ(WRIST_R_TORS);
         jpT(-JP_WRIST_R);
         render(BD_HAND_R);

#if 1 // right fingers
          glPushMatrix();	// right thumb
           jpT(JP_WRIST_R);
            jpRX(THUMB1_R_FLEX);
           jpT(-JP_WRIST_R);
           render(BD_THUMB_R);
          glPopMatrix();
          glPushMatrix();	// right index
           jpT(JP_WRIST_R);
            jpRX(INDEX1_R_FLEX);
           jpT(-JP_WRIST_R);
           render(BD_INDEX_R);
          glPopMatrix();
          glPushMatrix();	// right middle
           jpT(JP_WRIST_R);
            jpRX(MIDDLE1_R_FLEX);
           jpT(-JP_WRIST_R);
           render(BD_MIDDLE_R);
          glPopMatrix();
          glPushMatrix();	// right ring
           jpT(JP_WRIST_R);
            jpRX(RING1_R_FLEX);
           jpT(-JP_WRIST_R);
           render(BD_RING_R);
          glPopMatrix();
          glPushMatrix();	// right pinky
           jpT(JP_WRIST_R);
            jpRX(PINKY1_R_FLEX);
           jpT(-JP_WRIST_R);
           render(BD_PINKY_R);
          glPopMatrix();	// right pinky
#endif // right fingers

        glPopMatrix();	// right wrist
       glPopMatrix();	// r_elbow
      glPopMatrix();	// r_shoulder
     glPopMatrix();	// spinal

     //
     // bottom
     //

     // Left side
     glPushMatrix();	// left hip -> left thigh
      jpT(JP_HIP_L);
       jpRX(HIP_L_FLEX);
       jpRY(HIP_L_ABDU);
       jpRZ(HIP_L_TORS);
      jpT(-JP_HIP_L);
      render(BD_THIGH_L);

      glPushMatrix();	// left knee -> left shin
       jpT(JP_KNEE_L);
        jpRX(KNEE_L_FLEX);
        jpRZ(KNEE_L_TORS);
       jpT(-JP_KNEE_L);
       render(BD_SHIN_L);

       glPushMatrix();	// left ankle -> left foot
        jpT(JP_ANKLE_L);
         jpRX(ANKLE_L_FLEX);
         jpRZ(ANKLE_L_TORS);
        jpT(-JP_ANKLE_L);
        render(BD_FOOT_L);
       glPopMatrix();	// left ankle
      glPopMatrix();	// left knee
     glPopMatrix();	// left hip

     // Right side
     glPushMatrix();	// right hip -> right thigh
      jpT(JP_HIP_R);
       jpRX(HIP_R_FLEX);
       jpRY(HIP_R_ABDU);
       jpRZ(HIP_R_TORS);
      jpT(-JP_HIP_R);
      render(BD_THIGH_R);

      glPushMatrix();	// right knee -> right shin
       jpT(JP_KNEE_R);
        jpRX(KNEE_R_FLEX);
        jpRZ(KNEE_R_TORS);
       jpT(-JP_KNEE_R);
       render(BD_SHIN_R);

       glPushMatrix();	// right ankle -> right foot
        jpT(JP_ANKLE_R);
         jpRX(ANKLE_R_FLEX);
         jpRZ(ANKLE_R_TORS);
        jpT(-JP_ANKLE_R);
        render(BD_FOOT_R);
       glPopMatrix();	// right ankle
      glPopMatrix();	// right knee
     glPopMatrix();	// right hip
    glPopMatrix();	// chest
   glPopMatrix();	// pelvic

   glDisable(GL_COLOR_MATERIAL);
  glPopMatrix();	// body
}
