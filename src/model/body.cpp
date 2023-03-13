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
#include "body.hpp"
#include "face.hpp"	// Face::SCALE
#include "http.hpp"	// Http
#include "texture.hpp"	// open
#include "format.hpp"	// getModelByUrl
#include "user.hpp"	// localuser
#include "cache.hpp"	// open, close


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
}

void Finger::abduct(float a)
{
  aabduct = a;
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
}

void Hand::abduct(float a)
{
  aabduct = a;
}

void Hand::torsion(float a)
{
  atorsion = a;
}

float Hand::a_flexion() const
{
  return aflexion;
}

float Hand::a_abduct() const
{
  return aabduct;
}

float Hand::a_torsion() const
{
  return atorsion;
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
}

void Forearm::torsion(float a)
{
  atorsion = a;
}

float Forearm::a_flexion() const
{
  return aflexion;
}

float Forearm::a_torsion() const
{
  return atorsion;
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
}

void Arm::abduct(float a)
{
  //dax aabduct = a - 90;
  aabduct = a;
}

void Arm::torsion(float a)
{
  //dax atorsion = a + 90;
  atorsion = a;
}

float Arm::a_flexion() const
{
  return aflexion;
}

float Arm::a_abduct() const
{
  return aabduct;
}

float Arm::a_torsion() const
{
  return atorsion;
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
}

void Shoulder::abduct(float a)
{
  aabduct = a;
}

float Shoulder::a_flexion() const
{
  return aflexion;
}

float Shoulder::a_abduct() const
{
  return aabduct;
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
}

void HeadBody::abduct(float a)
{
  aabduct = a;
}

void HeadBody::torsion(float a)
{
  atorsion = a;
}

float HeadBody::a_flexion() const
{
  return aflexion;
}

float HeadBody::a_abduct() const
{
  return aabduct;
}

float HeadBody::a_torsion() const
{
  return atorsion;
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
}

void Neck::abduct(float a)
{
  aabduct = a;
}

void Neck::torsion(float a)
{
  atorsion = a;
}

float Neck::a_flexion() const
{
  return aflexion;
}

float Neck::a_abduct() const
{
  return aabduct;
}

float Neck::a_torsion() const
{
  return atorsion;
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
}

void Foot::abduct(float a)
{
  aabduct = -a;
}

void Foot::torsion(float a)
{
  atorsion = -a;
}

float Foot::a_flexion() const
{
  return aflexion;
}

float Foot::a_abduct() const
{
  return aabduct;
}

float Foot::a_torsion() const
{
  return atorsion;
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
}

void Shin::torsion(float a)
{
  atorsion = -a;
}

float Shin::a_flexion() const
{
  return aflexion;
}

float Shin::a_torsion() const
{
  return atorsion;
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
}

void Thigh::abduct(float a)
{
  aabduct = a;
}

void Thigh::torsion(float a)
{
  atorsion = a;
}

float Thigh::a_flexion() const
{
  return aflexion;
}

float Thigh::a_abduct() const
{
  return aabduct;
}

float Thigh::a_torsion() const
{
  return atorsion;
}

/** Chest */
Chest::Chest()
{
  aflexion = aabduct = atorsion = 0;
}

void Chest::flexion(float a)
{
  aflexion = a;
}

void Chest::abduct(float a)
{
  aabduct = a;
}

void Chest::torsion(float a)
{
  atorsion = a;
}

float Chest::a_flexion() const
{
  return aflexion;
}

float Chest::a_abduct() const
{
  return aabduct;
}

float Chest::a_torsion() const
{
  return atorsion;
}

/** Body */
Body::Body()
{
  init();
}

Body::Body(const char *_url)
{
  init();
  load(_url);
}

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

  tx = ty = tz = rx = ry = rz = 0;
  bscale = B_SCALE;

  for (int i=0; i<10 ; i++) {
    phalanx2[i] = new Phalanx2();
    phalanx[i] = new Phalanx(phalanx2[i]);
  }
  for (int i=0; i<5 ; i++) {
    fingers_r[i] = new Finger(phalanx[i]);
    fingers_l[i] = new Finger(phalanx[i+5]);
  }
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

  for (int i=0; i<3 ; i++) {
    skin[i] = 1;
    cloth[i] = .5;
  }
}

Body::~Body()
{
  for (int i=0; i<10 ; i++) {
    if (phalanx2[i]) delete phalanx2[i];
    if (phalanx[i])  delete phalanx[i];
  }
  for (int i=0; i<5 ; i++) {
    if (fingers_l[i]) delete fingers_l[i];
    if (fingers_r[i]) delete fingers_r[i];
  }
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
    if (!strcmp(ptab->name, t)) {
      *tok = ptab->num;
      return l;
    }
  }
  error("getTok: unknown \"%s\" in %s", t, l);
  return l;
}

void Body::load(const char *_url)
{
  url = new char[URL_LEN];
  strcpy(url, _url);
  Http::httpOpen(url, reader, this, 0);
}

char * Body::getUrl() const
{
  return (char *) url;
}

/** load joint points (static) */
void Body::reader(void *_body, Http *http)
{
  Body *body = (Body *) _body;

  if (! body) return;

  char *tmpurl = new char[URL_LEN];
  strcpy(tmpurl, body->getUrl());

  Cache *cache = new Cache();
  FILE *f = cache->open(tmpurl, http);
  if (! f) {
    error("Body: can't open %s", tmpurl);
  }
  else {
    body->loadBodyParts(f);
  }
  delete[] tmpurl;
  if (f) {
    cache->close();
    delete cache;
  }
}

/** load body's parts */
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
    l = strtok(jpline, SEP);	// tokenize jpline
    l = strtok(NULL, SEP);	// next token
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
    trace(DBG_MAN, "loadBodyParts: %s", jpline);

    int bpindex = 0;
    char *p = jpline;
    if (jpline[0] == '#') continue;
    else if ((p = strchr(jpline, '<'))) {
      l = strtok(p, SEP);
      l = getTok(l, &bpindex);
      if (bpindex < 0)
        goto endparse;  // eof
      l = strtok(NULL, SEP);

      // parse others lines of <body ...> ... </body>
      begin_while_parse(l) {
        if (! stringcmp(l, "jp=")) {
          l = skipEqual(l);
          l++;  // "
          jp.x[bpindex] = (float)atof(l) / bscale; l = strtok(NULL, SEP);
          jp.y[bpindex] = (float)atof(l) / bscale; l = strtok(NULL, SEP);
          jp.z[bpindex] = (float)atof(l) / bscale; l = strtok(NULL, SEP);
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

endparse:
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

#if 0 //dax notused
void Body::setJointPoint(uint8_t ind, float *_jp)
{
  jp.x[ind] = _jp[0];
  jp.y[ind] = _jp[1];
  jp.z[ind] = _jp[2];
}
#endif

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

/* called from humanoid */
void Body::setColors(float *_skin, float *_cloth)
{
  for (int i=0; i<3; i++) {
    skin[i] = _skin[i];
    cloth[i] = _cloth[i];
    color[i] = _cloth[i];
  }
}

/** Animates all body's articulations */
void Body::animate()
{
  if (!bap) return;

  if (bap->isMask(PELVIC_TILT))		chest->abduct(bap->getBap(PELVIC_TILT));
  if (bap->isMask(PELVIC_TORSION))	chest->torsion(bap->getBap(PELVIC_TORSION));
  if (bap->isMask(PELVIC_ROLL))		chest->flexion(bap->getBap(PELVIC_ROLL));
  if (bap->isMask(L_HIP_FLEXION))	thigh_l->flexion(bap->getBap(L_HIP_FLEXION));
  if (bap->isMask(R_HIP_FLEXION))	thigh_r->flexion(bap->getBap(R_HIP_FLEXION));
  if (bap->isMask(L_HIP_ABDUCT))	thigh_l->abduct(bap->getBap(L_HIP_ABDUCT));
  if (bap->isMask(R_HIP_ABDUCT))	thigh_r->abduct(bap->getBap(R_HIP_ABDUCT));
  if (bap->isMask(L_HIP_TORSION))       thigh_l->torsion(bap->getBap(L_HIP_TORSION));
  if (bap->isMask(R_HIP_TORSION))       thigh_r->torsion(bap->getBap(R_HIP_TORSION));
  if (bap->isMask(L_KNEE_FLEXION))	shin_l->flexion(bap->getBap(L_KNEE_FLEXION));
  if (bap->isMask(R_KNEE_FLEXION))	shin_r->flexion(bap->getBap(R_KNEE_FLEXION));
  if (bap->isMask(L_KNEE_TORSION))      shin_l->torsion(bap->getBap(L_KNEE_TORSION));
  if (bap->isMask(R_KNEE_TORSION))      shin_r->torsion(bap->getBap(R_KNEE_TORSION));
  if (bap->isMask(L_ANKLE_FLEXION))	foot_l->flexion(bap->getBap(L_ANKLE_FLEXION));
  if (bap->isMask(R_ANKLE_FLEXION))	foot_r->flexion(bap->getBap(R_ANKLE_FLEXION));
  if (bap->isMask(L_ANKLE_TORSION))     foot_l->torsion(bap->getBap(L_ANKLE_TORSION));
  if (bap->isMask(R_ANKLE_TORSION))     foot_r->torsion(bap->getBap(R_ANKLE_TORSION));
  if (bap->isMask(L_SHOULDER_FLEXION))	arm_l->flexion(bap->getBap(L_SHOULDER_FLEXION));
  if (bap->isMask(R_SHOULDER_FLEXION))	arm_r->flexion(bap->getBap(R_SHOULDER_FLEXION));
  if (bap->isMask(L_SHOULDER_ABDUCT))	arm_l->abduct(bap->getBap(L_SHOULDER_ABDUCT));
  if (bap->isMask(R_SHOULDER_ABDUCT))	arm_r->abduct(bap->getBap(R_SHOULDER_ABDUCT));
  if (bap->isMask(L_SHOULDER_TORSION))  arm_l->torsion(bap->getBap(L_SHOULDER_TORSION));
  if (bap->isMask(R_SHOULDER_TORSION))  arm_r->torsion(bap->getBap(R_SHOULDER_TORSION));
  if (bap->isMask(L_ELBOW_FLEXION))	forearm_l->flexion(bap->getBap(L_ELBOW_FLEXION));
  if (bap->isMask(R_ELBOW_FLEXION))	forearm_r->flexion(bap->getBap(R_ELBOW_FLEXION));
  if (bap->isMask(L_WRIST_FLEXION))	hand_l->abduct(bap->getBap(L_WRIST_FLEXION));
  if (bap->isMask(R_WRIST_FLEXION))	hand_r->abduct(bap->getBap(R_WRIST_FLEXION));
  if (bap->isMask(L_WRIST_PIVOT))	hand_l->flexion(bap->getBap(L_WRIST_PIVOT));
  if (bap->isMask(R_WRIST_PIVOT))	hand_r->flexion(bap->getBap(R_WRIST_PIVOT));
  if (bap->isMask(L_WRIST_TORSION))     hand_l->torsion(bap->getBap(L_WRIST_TORSION));
  if (bap->isMask(R_WRIST_TORSION))     hand_r->torsion(bap->getBap(R_WRIST_TORSION));
  if (bap->isMask(L_THUMB_FLEXION1))	fingers_l[0]->abduct(bap->getBap(L_THUMB_FLEXION1));
  if (bap->isMask(R_THUMB_FLEXION1))	fingers_l[0]->abduct(bap->getBap(R_THUMB_FLEXION1));
  if (bap->isMask(L_INDEX_FLEXION1))	fingers_l[1]->abduct(bap->getBap(L_INDEX_FLEXION1));
  if (bap->isMask(R_INDEX_FLEXION1))	fingers_l[1]->abduct(bap->getBap(R_INDEX_FLEXION1));
  if (bap->isMask(L_MIDDLE_FLEXION1))	fingers_l[2]->abduct(bap->getBap(L_MIDDLE_FLEXION1));
  if (bap->isMask(R_MIDDLE_FLEXION1))	fingers_l[2]->abduct(bap->getBap(R_MIDDLE_FLEXION1));
  if (bap->isMask(L_RING_FLEXION1))	fingers_l[3]->abduct(bap->getBap(L_RING_FLEXION1));
  if (bap->isMask(R_RING_FLEXION1))	fingers_l[3]->abduct(bap->getBap(R_RING_FLEXION1));
  if (bap->isMask(L_PINKY_FLEXION1))	fingers_l[4]->abduct(bap->getBap(L_PINKY_FLEXION1));
  if (bap->isMask(R_PINKY_FLEXION1))	fingers_r[4]->abduct(bap->getBap(R_PINKY_FLEXION1));
  if (bap->isMask(C1_ROLL))		head->flexion(bap->getBap(C1_ROLL));
  if (bap->isMask(C1_TORSION))		head->torsion(bap->getBap(C1_TORSION));
  if (bap->isMask(C1_TILT))		head->abduct(bap->getBap(C1_TILT));
  if (bap->isMask(C4_ROLL))		neck->flexion(bap->getBap(C4_ROLL));
  if (bap->isMask(C4_TORSION))		neck->torsion(bap->getBap(C4_TORSION));
  if (bap->isMask(C4_TILT))		neck->abduct(bap->getBap(C4_TILT));
  if (bap->isMask(T1_ROLL))		neck->flexion(bap->getBap(T1_ROLL));
  if (bap->isMask(T1_TORSION))		neck->torsion(bap->getBap(T1_TORSION));
  if (bap->isMask(T1_TILT))		neck->abduct(bap->getBap(T1_TILT));
  if (bap->isMask(TR_VERTICAL))		tz = bap->getBap(TR_VERTICAL) / TR_DIV;
  if (bap->isMask(TR_LATERAL))		ty = bap->getBap(TR_LATERAL) / TR_DIV;
  if (bap->isMask(TR_FRONTAL))		tx = bap->getBap(TR_FRONTAL) / TR_DIV;
  if (bap->isMask(RT_BODY_TURN))	rz = bap->getBap(RT_BODY_TURN);
  if (bap->isMask(RT_BODY_ROLL))	rx = bap->getBap(RT_BODY_ROLL);
  if (bap->isMask(RT_BODY_TILT))	ry = bap->getBap(RT_BODY_TILT);
}

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

void Body::animHead(float deg, uint8_t axis)
{
  echo("Head: %d %.0f", axis, deg);
  switch (axis) {
  case 0: head->flexion(deg); break;
  case 1: head->abduct(deg); break;
  case 2: head->torsion(deg); break;
  }
}

void Body::animNeck(float deg, uint8_t axis)
{
  echo("Neck: %d %.0f", axis, deg);
  switch (axis) {
  case 0: neck->flexion(deg); break;
  case 1: neck->abduct(deg); break;
  case 2: neck->torsion(deg); break;
  }
}

void Body::animChest(float deg, uint8_t axis)
{
  echo("Chest: %d %.0f", axis, deg);
  switch (axis) {
  case 0: chest->flexion(deg); break;
  case 1: chest->abduct(deg); break;
  case 2: chest->torsion(deg); break;
  }
}

void Body::animLeg(float deg, uint8_t side, uint8_t axis)
{
  echo("Leg: %d %d %.0f", side, axis, deg);
  switch (side) {
  case 0:
    switch (axis) {
    case 0: thigh_l->flexion(deg); break; //bap->setBap(L_HIP_FLEXION, deg);
    case 1: thigh_l->abduct(deg); break;
    case 2: thigh_l->torsion(deg); break;
    }
  case 1:
    switch (axis) {
    case 0: thigh_r->flexion(deg); break;
    case 1: thigh_r->abduct(deg); break;
    case 2: thigh_r->torsion(deg); break;
    }
  }
}

void Body::animShin(float deg, uint8_t side, uint8_t axis)
{
  echo("Shin: %d %d %.0f", side, axis, deg);
  switch (side) {
  case 0:
    switch (axis) {
    case 1: shin_l->flexion(deg); break;
    case 2: shin_l->torsion(deg); break;
    }
  case 1:
    switch (axis) {
    case 1: shin_r->flexion(deg); break;
    case 2: shin_r->torsion(deg); break;
    }
  }
}

void Body::animFoot(float deg, uint8_t side, uint8_t axis)
{
  echo("Foot: %d %d %.0f", side, axis, deg);
  switch (side) {
  case 0:
    switch (axis) {
    case 0: foot_l->flexion(deg); break;
    case 2: foot_l->torsion(deg); break;
    }
  case 1:
    switch (axis) {
    case 0: foot_r->flexion(deg); break;
    case 2: foot_r->torsion(deg); break;
    }
  }
}

void Body::animArm(float deg, uint8_t side, uint8_t axis)
{
  echo("Arm: %d %d %.0f", side, axis, deg);
  switch (side) {
  case 0:
    switch (axis) {
    case 0: arm_l->flexion(deg); break;
    case 1: arm_l->abduct(deg); break;
    case 2: arm_l->torsion(deg); break;
    }
  case 1:
    switch (axis) {
    case 0: arm_r->flexion(deg); break;
    case 1: arm_r->abduct(deg); break;
    case 2: arm_r->torsion(deg); break;
    }
  }
}

void Body::animForearm(float deg, uint8_t side, uint8_t axis)
{
  echo("Forearm: %d %d %.0f", side, axis, deg);
  switch (side) {
  case 0:
    switch (axis) {
    case 0: forearm_l->flexion(deg); break;
    case 2: forearm_l->torsion(deg); break;
    }
  case 1:
    switch (axis) {
    case 0: forearm_r->flexion(deg); break;
    case 2: forearm_r->torsion(deg); break;
    }
  }
}

void Body::animHand(float deg, uint8_t side, uint8_t axis)
{
  echo("Hand: %d %d %.0f", side, axis, deg);
  switch (side) {
  case 0:
    switch (axis) {
    case 0: hand_l->flexion(deg); break;
    case 1: hand_l->abduct(deg); break;
    case 2: hand_l->torsion(deg); break;
    }
  case 1:
    switch (axis) {
    case 0: hand_r->flexion(deg); break;
    case 1: hand_r->abduct(deg); break;
    case 2: hand_r->torsion(deg); break;
    }
  }
}

#if 1 //dax
/** Animates body articulations */
void Body::anim(int param)
{
  switch (param) {
  case PELVIC_TILT:		chest->abduct(bap->getBap(PELVIC_TILT)); break;
  case PELVIC_TORSION:		chest->torsion(bap->getBap(PELVIC_TORSION)); break;
  case PELVIC_ROLL:		chest->flexion(bap->getBap(PELVIC_ROLL)); break;

  case L_HIP_FLEXION:		thigh_l->flexion(bap->getBap(L_HIP_FLEXION)); break;
  case R_HIP_FLEXION:		thigh_r->flexion(bap->getBap(R_HIP_FLEXION)); break;
  case L_HIP_ABDUCT:		thigh_l->abduct(bap->getBap(L_HIP_ABDUCT)); break;
  case R_HIP_ABDUCT:		thigh_r->abduct(bap->getBap(R_HIP_ABDUCT)); break;
  case L_HIP_TORSION:		thigh_l->torsion(bap->getBap(L_HIP_TORSION)); break;
  case R_HIP_TORSION:		thigh_r->torsion(bap->getBap(R_HIP_TORSION)); break;

  case L_KNEE_FLEXION:		shin_l->flexion(bap->getBap(L_KNEE_FLEXION)); break;
  case R_KNEE_FLEXION:		shin_r->flexion(bap->getBap(R_KNEE_FLEXION)); break;
  case L_KNEE_TORSION:		shin_l->torsion(bap->getBap(L_KNEE_TORSION)); break;
  case R_KNEE_TORSION:		shin_r->torsion(bap->getBap(R_KNEE_TORSION)); break;

  case L_ANKLE_FLEXION:		foot_l->flexion(bap->getBap(L_ANKLE_FLEXION)); break;
  case R_ANKLE_FLEXION:		foot_r->flexion(bap->getBap(R_ANKLE_FLEXION)); break;
  case L_ANKLE_TORSION:		foot_l->torsion(bap->getBap(L_ANKLE_TORSION)); break;
  case R_ANKLE_TORSION:		foot_r->torsion(bap->getBap(R_ANKLE_TORSION)); break;

  case L_SHOULDER_FLEXION:	arm_l->flexion(bap->getBap(L_SHOULDER_FLEXION)); break;
  case R_SHOULDER_FLEXION:	arm_r->flexion(bap->getBap(R_SHOULDER_FLEXION)); break;
  case L_SHOULDER_ABDUCT:	arm_l->abduct(bap->getBap(L_SHOULDER_ABDUCT)); break;
  case R_SHOULDER_ABDUCT:	arm_r->abduct(bap->getBap(R_SHOULDER_ABDUCT)); break;
  case L_SHOULDER_TORSION:	arm_l->torsion(bap->getBap(L_SHOULDER_TORSION)); break;
  case R_SHOULDER_TORSION:	arm_r->torsion(bap->getBap(R_SHOULDER_TORSION)); break;

  case L_ELBOW_FLEXION:		forearm_l->flexion(bap->getBap(L_ELBOW_FLEXION)); break;
  case R_ELBOW_FLEXION:		forearm_r->flexion(bap->getBap(R_ELBOW_FLEXION)); break;

  case L_WRIST_FLEXION:		hand_l->abduct(bap->getBap(L_WRIST_FLEXION)); break;
  case R_WRIST_FLEXION:		hand_r->abduct(bap->getBap(R_WRIST_FLEXION)); break;
  case L_WRIST_PIVOT:		hand_l->flexion(bap->getBap(L_WRIST_PIVOT)); break;
  case R_WRIST_PIVOT:		hand_r->flexion(bap->getBap(R_WRIST_PIVOT)); break;
  case L_WRIST_TORSION:		hand_l->torsion(bap->getBap(L_WRIST_TORSION)); break;
  case R_WRIST_TORSION:		hand_r->torsion(bap->getBap(R_WRIST_TORSION)); break;

  case C1_ROLL:			head->flexion(bap->getBap(C1_ROLL)); break;
  case C1_TILT:			head->abduct(bap->getBap(C1_TILT)); break;
  case C1_TORSION:		head->torsion(bap->getBap(C1_TORSION)); break;

  case C4_ROLL:			neck->flexion(bap->getBap(C4_ROLL)); break;
  case C4_TILT:			neck->abduct(bap->getBap(C4_TILT)); break;
  case C4_TORSION:		neck->torsion(bap->getBap(C4_TORSION)); break;

  case T1_ROLL:			neck->flexion(bap->getBap(T1_ROLL)); break;
  case T1_TILT:			neck->abduct(bap->getBap(T1_TILT)); break;
  case T1_TORSION:		neck->torsion(bap->getBap(T1_TORSION)); break;

  case TR_VERTICAL:		tz = bap->getBap(TR_VERTICAL) / TR_DIV; break;
  case TR_LATERAL:		ty = bap->getBap(TR_LATERAL) / TR_DIV; break;
  case TR_FRONTAL:		tx = bap->getBap(TR_FRONTAL) / TR_DIV; break;

  case RT_BODY_TURN:		rz = bap->getBap(RT_BODY_TURN); break;
  case RT_BODY_ROLL:		rx = bap->getBap(RT_BODY_ROLL); break;
  case RT_BODY_TILT:		ry = bap->getBap(RT_BODY_TILT); break;
  }
}
#endif

bool Body::isLoaded(uint8_t part)
{
  if (part < MAX_PARTS)
    return bodyparts[part].loaded;
  return false;
}

void Body::transP(uint8_t part)
{
  if (part < MAX_PARTS) {
    if (model == MODEL_OBJ)
      glTranslatef(jp.y[part], jp.x[part], jp.z[part]);
    else
      glTranslatef(jp.x[part], jp.y[part], jp.z[part]);
  }
}

void Body::transN(uint8_t part)
{
  if (part < MAX_PARTS) {
    if (model == MODEL_OBJ)
      glTranslatef(-jp.y[part], -jp.x[part], -jp.z[part]);
    else
      glTranslatef(-jp.x[part], -jp.y[part], -jp.z[part]);
  }
}

void Body::rotX(int param, uint8_t model)
{
  int sign = (param >= 0) ?1:-1;
  switch (model) {
  case MODEL_OFF: glRotatef( sign * bap->getBap(abs(param)), 1,0,0); break;
  case MODEL_OBJ: glRotatef(-sign * bap->getBap(abs(param)), 0,1,0); break;
  }
}

void Body::rotY(int param, uint8_t model)
{
  int sign = (param >= 0) ?1:-1;
  switch (model) {
  case MODEL_OFF: glRotatef(sign * bap->getBap(abs(param)), 0,1,0); break;
  case MODEL_OBJ: glRotatef(sign * bap->getBap(abs(param)), 1,0,0); break;
  }
}

void Body::rotZ(int param, uint8_t model)
{
  int sign = (param >= 0) ?1:-1;
  switch (model) {
  case MODEL_OFF: glRotatef(sign * bap->getBap(abs(param)), 0,0,1); break;
  case MODEL_OBJ: glRotatef(sign * bap->getBap(abs(param)), 0,0,1); break;
  }
}

void Body::rotX(int param, float angle)
{
  int sign = (param >= 0) ?1:-1;
  glRotatef(sign * angle, 1,0,0);
}

void Body::rotY(int param, float angle)
{
  int sign = (param >= 0) ?1:-1;
  glRotatef(sign * angle, 0,1,0);
}

void Body::rotZ(int param, float angle)
{
  int sign = (param >= 0) ?1:-1;
  glRotatef(sign * angle, 0,0,1);
}

/** display a part of the body */
void Body::display(uint8_t part)
{
  if (part < MAX_PARTS) {
    glMaterialfv(GL_FRONT, GL_DIFFUSE, bodyparts[part].color);
    if (bodyparts[part].texid) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, bodyparts[part].texid);
    }
    else {
      glColor3fv(bodyparts[part].color);
    }

    glCallList(dlist + part);

    glDisable(GL_TEXTURE_2D);
  }
}

/** display body and face */
void Body::display()
{
  // Hips
  glPushMatrix();	//  Pelvic (Body Bottom)
   transP(PELVIC);
   //rotX(PELVIC_TILT, model);
   //rotY(PELVIC_ROLL, model);
   //rotZ(PELVIC_TORSION, model);
   //rotX(PELVIC_TILT, chest->a_flexion());
   //rotY(PELVIC_ROLL, chest->a_abduct());
   //rotZ(PELVIC_TORSION, chest->a_torsion());
   transN(PELVIC);
   display(HIPS);

   // Abdomen
   if (isLoaded(ABDOMEN)) {
     glPushMatrix();
     display(ABDOMEN);
     glPopMatrix();
   }
   // Skirt
   if (isLoaded(SKIRT)) {
     glPushMatrix();
     display(SKIRT);
     glPopMatrix();
   }
   // Belt
   if (isLoaded(BELT)) {
     glPushMatrix();
     display(BELT);
     glPopMatrix();
   }
   // Chest
   glPushMatrix();	//  Spinal -> Chest (thoracic level 5)
    transP(SPINAL);
    //rotX(T5_TILT, model);
    //rotY(T5_ROLL, model);
    //rotZ(T5_TORSION, model);
    rotX(T5_TILT, chest->a_flexion());
    rotY(T5_ROLL, chest->a_abduct());
    rotZ(T5_TORSION, chest->a_torsion());
    transN(SPINAL);
    display(CHEST);
    // Collar
    if (isLoaded(L_COLLAR)) {
      glPushMatrix();
      display(L_COLLAR);
      glPopMatrix();
    }
    if (isLoaded(R_COLLAR)) {
      glPushMatrix();
      display(R_COLLAR);
      glPopMatrix();
    }
    // Lower Neck
    glPushMatrix();	//  Lower Neck (cervical level 4)
     transP(LOWER_NECK);
     //rotX(C4_TILT, model);
     //rotY(C4_ROLL, model);
     //rotZ(C4_TORSION, model);
     rotX(C4_TILT, neck->a_flexion());
     rotY(C4_ROLL, neck->a_abduct());
     rotZ(C4_TORSION, neck->a_torsion());
     transN(LOWER_NECK);
     display(NECK);
     // Head
     if (! face) {
       //echo("head render");
       display(HEAD);
     }

     // Upper Neck
     glPushMatrix();	//  Upper Neck -> Head (cervical level 1)
      transP(UPPER_NECK);
      //rotX(C1_TILT, model);
      //rotY(C1_ROLL, model);
      //rotZ(C1_TORSION, model);
      rotX(C1_TILT, head->a_flexion());
      rotY(C1_ROLL, head->a_abduct());
      rotZ(C1_TORSION, head->a_torsion());
      if (face) {
        glScalef(Face::SCALE, Face::SCALE, Face::SCALE);
        glTranslatef(0, 0.9, -0.9);
        glRotatef(90, 1,0,0);
        //echo("face render");
        face->render();	// YR
      }
      transP(UPPER_NECK);
      //rotX(C1_TILT, model);
      //rotY(C1_ROLL, model);
      //rotZ(C1_TORSION, model);
      rotX(C1_TILT, head->a_flexion());
      rotY(C1_ROLL, head->a_abduct());
      rotZ(C1_TORSION, head->a_torsion());
      transN(UPPER_NECK);
     glPopMatrix(); 	// head
    glPopMatrix(); 	// neck

    // Left arm
    glPushMatrix();	//  Left Shoulder -> Left Arm
     transP(L_SHOULDER);
     //if (model == MODEL_OFF)
      // glRotatef(-90, 0,0,1);  //FIXME
     //rotX(L_SHOULDER_ABDUCT, model);
     //rotY(-L_SHOULDER_FLEXION, model);	// -
     //rotZ(-L_SHOULDER_TORSION, model);	// -
     //echo("L f:%.0f a:%.0f t:%.0f",arm_l->a_flexion(),arm_l->a_abduct(),arm_l->a_torsion());
     rotX(L_SHOULDER_ABDUCT, arm_l->a_abduct());
     rotY(+L_SHOULDER_FLEXION,  arm_l->a_flexion());	// -
     rotZ(+L_SHOULDER_TORSION, arm_l->a_torsion());	// -
     transN(L_SHOULDER);
     display(L_ARM);

     // Left forearm
     glPushMatrix();	//  Left Elbow -> Left Forearm
      transP(L_ELBOW);
      //rotY(-L_ELBOW_FLEXION, model);	// -
      //rotZ(+L_ELBOW_TORSION, model);	// -
      rotY(+L_ELBOW_FLEXION,  forearm_l->a_flexion());	// -
      rotZ(+L_ELBOW_TORSION, forearm_l->a_torsion());	// -
      transN(L_ELBOW);
      display(L_FOREARM);

      // Left hand
      glPushMatrix();	//  Left Wrist -> Left Hand
       transP(L_WRIST);
       //rotX(L_WRIST_FLEXION, model);
       //rotY(L_WRIST_PIVOT, model);
       //rotZ(L_WRIST_TORSION, model);
       rotX(L_WRIST_FLEXION,  hand_l->a_flexion());
       rotY(L_WRIST_PIVOT, hand_l->a_abduct());
       rotZ(L_WRIST_TORSION, hand_l->a_torsion());
       transN(L_WRIST);
       display(L_HAND);

      glPopMatrix();
     glPopMatrix();
    glPopMatrix();	// l_shoulder

    // Right arm
    glPushMatrix();	//  Right Shoulder -> Right Arm
     transP(R_SHOULDER);
     //dax if (model == MODEL_OFF)
       //dax glRotatef(90, 0,0,1);	//OK but FIXME
     //rotX(+R_SHOULDER_ABDUCT, model);	// -
     //rotY(R_SHOULDER_FLEXION, model);
     //rotZ(+R_SHOULDER_TORSION, model);	// -
     //echo("R f:%.0f a:%.0f t:%.0f",arm_r->a_flexion(),arm_r->a_abduct(),arm_r->a_torsion());
     rotX(+R_SHOULDER_ABDUCT,  arm_r->a_abduct());	// -
     rotY(R_SHOULDER_FLEXION, arm_r->a_flexion());
     rotZ(+R_SHOULDER_TORSION, arm_r->a_torsion());	// -
     transN(R_SHOULDER);
     display(R_ARM);

     // Right forearm
     glPushMatrix();	//  Right Elbow -> Right Forearm
      transP(R_ELBOW);
      //rotY(R_ELBOW_FLEXION, model);
      //rotZ(+R_ELBOW_TORSION, model);	// -
      rotY(L_ELBOW_FLEXION,  forearm_r->a_flexion());
      rotZ(L_ELBOW_TORSION, forearm_r->a_torsion());
      transN(R_ELBOW);
      display(R_FOREARM);

      // Right hand
      glPushMatrix();	//  Right Wrist -> Right Hand
       transP(R_WRIST);
       //rotX(R_WRIST_FLEXION, model);
       //rotY(R_WRIST_PIVOT, model);
       //rotZ(R_WRIST_TORSION, model);
       rotX(R_WRIST_FLEXION,  hand_r->a_flexion());
       rotX(R_WRIST_PIVOT,  hand_r->a_abduct());
       rotZ(R_WRIST_TORSION, hand_r->a_torsion());
       transN(R_WRIST);
       display(R_HAND);

#if 0 //dax fingers
       glPushMatrix();	//  Right fingers
       glPushMatrix();	//  Right thumb
        transP(R_THUMB);
        rotX(R_THUMB_FLEXION1, model);
        transN(R_THUMB);
        //display(R_THUMB);
       glPopMatrix();
       glPushMatrix();	//  Right index
        transP(R_INDEX);
        rotX(R_INDEX_FLEXION1, model);
        transN(R_INDEX);
        //display(R_INDEX);
       glPopMatrix();
       glPushMatrix();	//  Right middle
        transP(R_MIDDLE);
        rotX(R_MIDDLE_FLEXION1, model);
        transN(R_MIDDLE);
        //display(R_MIDDLE);
       glPopMatrix();
       glPushMatrix();	//  Right ring
        transP(R_RING);
        rotX(R_RING_FLEXION1, model);
        transN(R_RING);
        //display(R_RING);
       glPopMatrix();
       glPushMatrix();	//  Right pinky
        transP(R_PINKY);
        rotX(R_PINKY_FLEXION1, model);
        transN(R_PINKY);
        //ddisplay(R_PINKY);
       glPopMatrix();
       glPopMatrix();
#endif

      glPopMatrix();
     glPopMatrix();
    glPopMatrix();	// r_shoulder
   glPopMatrix();	// spinal
  glPopMatrix();	// pelvic

  // Left thigh
  glPushMatrix();	//  Left Hip -> Left Thigh
   transP(L_HIP);
   //rotX(L_HIP_FLEXION, model);
   //rotY(L_HIP_ABDUCT, model);
   //rotZ(L_HIP_TORSION, model);
   rotX(L_HIP_FLEXION, thigh_l->a_flexion());
   rotY(L_HIP_ABDUCT, thigh_l->a_abduct());
   rotZ(L_HIP_TORSION, thigh_l->a_torsion());
   transN(L_HIP);
   display(L_THIGH);

   // Left shin
   glPushMatrix();	//  Left Knee -> Left Shin
    transP(L_KNEE);
    //rotX(L_KNEE_FLEXION, model);
    //rotZ(L_KNEE_TORSION, model);
    rotX(-L_KNEE_FLEXION, shin_l->a_flexion());	// -
    rotZ(L_KNEE_TORSION, shin_l->a_torsion());
    transN(L_KNEE);
    display(L_SHIN);

    // Left foot
    glPushMatrix();	//  Left Ankle -> Left Foot
     transP(L_ANKLE);
     //rotX(L_ANKLE_FLEXION, model);
     //rotZ(L_ANKLE_TORSION, model);
     rotX(L_ANKLE_FLEXION, foot_l->a_flexion());
     rotZ(L_ANKLE_TORSION, foot_l->a_torsion());
     transN(L_ANKLE);
     display(L_FOOT);
    glPopMatrix();
   glPopMatrix();
  glPopMatrix();	// l_thigh

  // Right thigh
  glPushMatrix();	//  Right Hip -> Right Thigh
   transP(R_HIP);
   //rotX(R_HIP_FLEXION, model);
   //rotY(R_HIP_ABDUCT, model);
   //rotZ(R_HIP_TORSION, model);
   rotX(L_HIP_FLEXION, thigh_r->a_flexion());
   rotY(L_HIP_ABDUCT, thigh_r->a_abduct());
   rotZ(L_HIP_TORSION, thigh_r->a_torsion());
   transN(R_HIP);
   display(R_THIGH);

   // Right shin
   glPushMatrix();	//  Right Knee -> Right Shin
    transP(R_KNEE);
    //rotX(R_KNEE_FLEXION, model);
    //rotZ(R_KNEE_TORSION, model);
    rotX(-R_KNEE_FLEXION, shin_r->a_flexion());	// -
    rotZ(R_KNEE_TORSION, shin_r->a_torsion());
    transN(R_KNEE);
    display(R_SHIN);

    // Right foot
    glPushMatrix();	//  Right Ankle -> Right Foot
     transP(R_ANKLE);
     //rotX(R_ANKLE_FLEXION, model);
     //rotZ(R_ANKLE_TORSION, model);
     rotX(R_ANKLE_FLEXION, foot_r->a_flexion());
     rotZ(R_ANKLE_TORSION, foot_r->a_torsion());
     transN(R_ANKLE);
     display(R_FOOT);
    glPopMatrix();
   glPopMatrix();
  glPopMatrix();	// r_thigh
}

void Body::render(Pos& pos)
{
  //dax if (!bap) return;

  float dtz = (bscale != 1) ?  B_HEIGHT/2 : 0;
  const float color[] = {.4,.4,.4,1};

  glPushMatrix();
   glTranslatef(pos.x + tx, pos.y + ty, pos.z + tz + dtz);
   switch (model) {
   case MODEL_OBJ:
     glRotatef(rx, 0,1,0);
     glRotatef(ry, 1,0,0);
     glRotatef(RAD2DEG(pos.az) + rz - 90, 0,0,1);
     break;
   case MODEL_OFF:
   default:
     glRotatef(rx, 1,0,0);
     glRotatef(ry, 0,1,0);
     glRotatef(RAD2DEG(pos.az) + rz + 180, 0,0,1);
     break;
   }
   glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
   glColorMaterial(GL_FRONT, GL_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);

   display();

   glDisable(GL_COLOR_MATERIAL);
  glPopMatrix();
}
