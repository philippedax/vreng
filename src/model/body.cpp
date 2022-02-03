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
#include "cache.hpp"	// openCache
#include "file.hpp"	// closeFile


const GLfloat Phalanx2::PHALANX2_LEN = 0.012;	// default phalanx2 length
const GLfloat Phalanx::PHALANX_LEN = 0.012;	// default phalanx length
const GLfloat Finger::FINGER_LEN = 0.0375;	// default finger length
const GLfloat Hand::HAND_LEN = 0.10;		// default hand length
const GLfloat Forearm::FOREARM_LEN = 0.178;	// default forearm length
const GLfloat Forearm::ELBOW_RAD = 0.025;	// default elbow radius
const GLfloat Arm::ARM_LEN = 0.165;		// default arm length
const GLfloat Shoulder::SHOULDER_RAD = 0.0025;	// default shoulder radius
const GLfloat HeadBody::HEAD_RAD = 0.075;	// default head radius
const GLfloat Neck::NECK_LEN = 0.05;		// default neck height
const GLfloat Foot::FOOT_LEN = 0.12;		// default foot length
const GLfloat Shin::SHIN_LEN = 0.265;		// default shin length
const GLfloat Thigh::THIGH_LEN = 0.25;		// default thigh length
const GLfloat Chest::CHEST_LEN = 0.30;		// default chest length

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
  { "/body", -1 }
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

void Phalanx::flexion(GLfloat a)
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

void Finger::flexion(GLfloat a)
{
  aflexion = a;
}

void Finger::abduct(GLfloat a)
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

void Hand::flexion(GLfloat a)
{
  aflexion = a;
}

void Hand::abduct(GLfloat a)
{
  aabduct = a;
}

void Hand::torsion(GLfloat a)
{
  atorsion = a;
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

void Forearm::flexion(GLfloat a)
{
  aflexion = a;
}

void Forearm::torsion(GLfloat a)
{
  atorsion = a;
}

/** Arm */
Arm::Arm(Forearm *_forearm)
{
  aabduct = -90;
  aflexion = 0;
  atorsion = 90;
  length = ARM_LEN;
}

int Arm::init()
{
  return forearm->init();
}

void Arm::abduct(GLfloat a)
{
  aabduct = a - 90;
}

void Arm::flexion(GLfloat a)
{
  aflexion = -a;
}

void Arm::torsion(GLfloat a)
{
  atorsion = a + 90;
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

void Shoulder::flexion(GLfloat a)
{
  aflexion = a;
}

void Shoulder::abduct(GLfloat a)
{
  aabduct = a;
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

void HeadBody::flexion(GLfloat a)
{
  aflexion = a;
}

void HeadBody::abduct(GLfloat a)
{
  aabduct = a;
}

void HeadBody::torsion(GLfloat a)
{
  atorsion = a;
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

void Neck::flexion(GLfloat a)
{
  aflexion = a;
}

void Neck::abduct(GLfloat a)
{
  aabduct = a;
}

void Neck::torsion(GLfloat a)
{
  atorsion = a;
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

void Foot::flexion(GLfloat a)
{
  aflexion = -a;
}

void Foot::abduct(GLfloat a)
{
  aabduct = -a;
}

void Foot::torsion(GLfloat a)
{
  atorsion = -a;
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

void Shin::flexion(GLfloat a)
{
  aflexion = -a;
}

void Shin::torsion(GLfloat a)
{
  atorsion = -a;
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

void Thigh::abduct(GLfloat a)
{
  aabduct = a;
}

void Thigh::flexion(GLfloat a)
{
  aflexion = a;
}

void Thigh::torsion(GLfloat a)
{
  atorsion = a;
}

/** Chest */
Chest::Chest()
{
  aflexion = aabduct = atorsion = 0;
}

void Chest::flexion(GLfloat a)
{
  aflexion = a;
}

void Chest::abduct(GLfloat a)
{
  aabduct = a;
}

void Chest::torsion(GLfloat a)
{
  atorsion = a;
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
  Http::httpOpen(url, httpReader, this, 0);
}

char * Body::getUrl() const
{
  return (char *) url;
}

/** load joint points (static) */
void Body::httpReader(void *_body, Http *http)
{
  Body *body = (Body *) _body;

  if (! body) return;

  char *tmpurl = new char[URL_LEN];
  strcpy(tmpurl, body->getUrl());

  FILE *f = Cache::openCache(tmpurl, http);
  if (! f) {
    error("Body: can't open %s", tmpurl);
  }
  else {
    body->loadBodyParts(f);
  }
  delete[] tmpurl;
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
        Http::httpOpen(bodyparts[i].url, Off::httpReader, bodyparts[i].off, 0);
        break;
      case MODEL_OBJ:
        bodyparts[i].obj = new Obj(bodyparts[i].url, 1);
        Http::httpOpen(bodyparts[i].url, Obj::httpReader, bodyparts[i].obj, 0);
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
  if (f) File::closeFile(f);
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
  if (bap->isMask(T1_ROLL))		neck->abduct(bap->getBap(T1_ROLL));
  if (bap->isMask(T1_TORSION))		neck->torsion(bap->getBap(T1_TORSION));
  if (bap->isMask(T1_TILT))		neck->flexion(bap->getBap(T1_TILT));
  if (bap->isMask(TR_VERTICAL))		tz = bap->getBap(TR_VERTICAL) / TR_DIV;
  if (bap->isMask(TR_LATERAL))		ty = bap->getBap(TR_LATERAL) / TR_DIV;
  if (bap->isMask(TR_FRONTAL))		tx = bap->getBap(TR_FRONTAL) / TR_DIV;
  if (bap->isMask(RT_BODY_TURN))	rz = bap->getBap(RT_BODY_TURN);
  if (bap->isMask(RT_BODY_ROLL))	rx = bap->getBap(RT_BODY_ROLL);
  if (bap->isMask(RT_BODY_TILT))	ry = bap->getBap(RT_BODY_TILT);

  //if (tx || ty || tz) error("t: %.1f %.1f %.1f", tx,ty,tz);
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
  case C1_TORSION:		head->torsion(bap->getBap(C1_TORSION)); break;
  case C1_TILT:			head->abduct(bap->getBap(C1_TILT)); break;

  case C4_ROLL:			neck->flexion(bap->getBap(C4_ROLL)); break;
  case C4_TORSION:		neck->torsion(bap->getBap(C4_TORSION)); break;
  case C4_TILT:			neck->abduct(bap->getBap(C4_TILT)); break;

  case T1_ROLL:			neck->abduct(bap->getBap(T1_ROLL)); break;
  case T1_TORSION:		neck->torsion(bap->getBap(T1_TORSION)); break;
  case T1_TILT:			neck->flexion(bap->getBap(T1_TILT)); break;

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

void Body::jpTP(uint8_t part)
{
  if (part < MAX_PARTS) {
    if (model == MODEL_OBJ)
      glTranslatef(jp.y[part], jp.x[part], jp.z[part]);
    else
      glTranslatef(jp.x[part], jp.y[part], jp.z[part]);
  }
}

void Body::jpTN(uint8_t part)
{
  if (part < MAX_PARTS) {
    if (model == MODEL_OBJ)
      glTranslatef(-jp.y[part], -jp.x[part], -jp.z[part]);
    else
      glTranslatef(-jp.x[part], -jp.y[part], -jp.z[part]);
  }
}

void Body::jpRX(int param, uint8_t model)
{
  int sign = (param >= 0) ?1:-1;
  switch (model) {
  case MODEL_OFF: glRotatef( sign * bap->getBap(abs(param)), 1,0,0); break;
  case MODEL_OBJ: glRotatef(-sign * bap->getBap(abs(param)), 0,1,0); break;
  }
}

void Body::jpRY(int param, uint8_t model)
{
  int sign = (param >= 0) ?1:-1;
  switch (model) {
  case MODEL_OFF: glRotatef(sign * bap->getBap(abs(param)), 0,1,0); break;
  case MODEL_OBJ: glRotatef(sign * bap->getBap(abs(param)), 1,0,0); break;
  }
}

void Body::jpRZ(int param, uint8_t model)
{
  int sign = (param >= 0) ?1:-1;
  switch (model) {
  case MODEL_OFF: glRotatef(sign * bap->getBap(abs(param)), 0,0,1); break;
  case MODEL_OBJ: glRotatef(sign * bap->getBap(abs(param)), 0,0,1); break;
  }
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
   jpTP(PELVIC);
   jpRX(PELVIC_TILT, model);
   jpRY(PELVIC_ROLL, model);
   jpRZ(PELVIC_TORSION, model);
   jpTN(PELVIC);
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
    jpTP(SPINAL);
    jpRX(T5_TILT, model);
    jpRY(T5_ROLL, model);
    jpRZ(T5_TORSION, model);
    jpTN(SPINAL);
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
     jpTP(LOWER_NECK);
     jpRX(C4_TILT, model);
     jpRY(C4_ROLL, model);
     jpRZ(C4_TORSION, model);
     jpTN(LOWER_NECK);
     display(NECK);
     // Head
     if (! face)
       display(HEAD);

     // Upper Neck
     glPushMatrix();	//  Upper Neck -> Head (cervical level 1)
      jpTP(UPPER_NECK);
      jpRX(C1_TILT, model);
      jpRY(C1_ROLL, model);
      jpRZ(C1_TORSION, model);
      if (face) {
        glScalef(Face::SCALE, Face::SCALE, Face::SCALE);
        glTranslatef(0, 0.9, -0.9);
        glRotatef(90, 1,0,0);
        face->render();	// YR
      }
      jpTP(UPPER_NECK);
      jpRX(C1_TILT, model);
      jpRY(C1_ROLL, model);
      jpRZ(C1_TORSION, model);
      jpTN(UPPER_NECK);
     glPopMatrix(); 	// head
    glPopMatrix(); 	// neck

    // Left arm
    glPushMatrix();	//  Left Shoulder -> Left Arm
     jpTP(L_SHOULDER);
     if (model == MODEL_OFF)
       glRotatef(-90, 0,0,1);  //FIXME
     jpRX(L_SHOULDER_ABDUCT, model);
     jpRY(-L_SHOULDER_FLEXION, model);	// -
     jpRZ(-L_SHOULDER_TORSION, model);	// -
     jpTN(L_SHOULDER);
     display(L_ARM);

     // Left forearm
     glPushMatrix();	//  Left Elbow -> Left Forearm
      jpTP(L_ELBOW);
      jpRY(-L_ELBOW_FLEXION, model);	// -
      jpRZ(+L_ELBOW_TORSION, model);	// -
      jpTN(L_ELBOW);
      display(L_FOREARM);

      // Left hand
      glPushMatrix();	//  Left Wrist -> Left Hand
       jpTP(L_WRIST);
       jpRX(L_WRIST_FLEXION, model);
       jpRY(L_WRIST_PIVOT, model);
       jpRZ(L_WRIST_TORSION, model);
       jpTN(L_WRIST);
       display(L_HAND);

      glPopMatrix();
     glPopMatrix();
    glPopMatrix();	// l_shoulder

    // Right arm
    glPushMatrix();	//  Right Shoulder -> Right Arm
     jpTP(R_SHOULDER);
     if (model == MODEL_OFF)
       glRotatef(90, 0,0,1);	//OK but FIXME
     jpRX(-R_SHOULDER_ABDUCT, model);	// -
     jpRY(R_SHOULDER_FLEXION, model);
     jpRZ(-R_SHOULDER_TORSION, model);	// -
     jpTN(R_SHOULDER);
     display(R_ARM);

     // Right forearm
     glPushMatrix();	//  Right Elbow -> Right Forearm
      jpTP(R_ELBOW);
      jpRY(R_ELBOW_FLEXION, model);
      jpRZ(+R_ELBOW_TORSION, model);	// -
      jpTN(R_ELBOW);
      display(R_FOREARM);

      // Right hand
      glPushMatrix();	//  Right Wrist -> Right Hand
       jpTP(R_WRIST);
       jpRX(R_WRIST_FLEXION, model);
       jpRY(R_WRIST_PIVOT, model);
       jpRZ(R_WRIST_TORSION, model);
       jpTN(R_WRIST);
       display(R_HAND);

#if 0 //dax fingers
       glPushMatrix();	//  Right fingers
       glPushMatrix();	//  Right thumb
        jpTP(R_THUMB);
        jpRX(R_THUMB_FLEXION1, model);
        jpTN(R_THUMB);
        //display(R_THUMB);
       glPopMatrix();
       glPushMatrix();	//  Right index
        jpTP(R_INDEX);
        jpRX(R_INDEX_FLEXION1, model);
        jpTN(R_INDEX);
        //display(R_INDEX);
       glPopMatrix();
       glPushMatrix();	//  Right middle
        jpTP(R_MIDDLE);
        jpRX(R_MIDDLE_FLEXION1, model);
        jpTN(R_MIDDLE);
        //display(R_MIDDLE);
       glPopMatrix();
       glPushMatrix();	//  Right ring
        jpTP(R_RING);
        jpRX(R_RING_FLEXION1, model);
        jpTN(R_RING);
        //display(R_RING);
       glPopMatrix();
       glPushMatrix();	//  Right pinky
        jpTP(R_PINKY);
        jpRX(R_PINKY_FLEXION1, model);
        jpTN(R_PINKY);
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
   jpTP(L_HIP);
   jpRX(L_HIP_FLEXION, model);
   jpRY(L_HIP_ABDUCT, model);
   jpRZ(L_HIP_TORSION, model);
   jpTN(L_HIP);
   display(L_THIGH);

   // Left shin
   glPushMatrix();	//  Left Knee -> Left Shin
    jpTP(L_KNEE);
    jpRX(L_KNEE_FLEXION, model);
    jpRZ(L_KNEE_TORSION, model);
    jpTN(L_KNEE);
    display(L_SHIN);

    // Left foot
    glPushMatrix();	//  Left Ankle -> Left Foot
     jpTP(L_ANKLE);
     jpRX(L_ANKLE_FLEXION, model);
     jpRZ(L_ANKLE_TORSION, model);
     jpTN(L_ANKLE);
     display(L_FOOT);
    glPopMatrix();
   glPopMatrix();
  glPopMatrix();	// l_thigh

  // Right thigh
  glPushMatrix();	//  Right Hip -> Right Thigh
   jpTP(R_HIP);
   jpRX(R_HIP_FLEXION, model);
   jpRY(R_HIP_ABDUCT, model);
   jpRZ(R_HIP_TORSION, model);
   jpTN(R_HIP);
   display(R_THIGH);

   // Right shin
   glPushMatrix();	//  Right Knee -> Right Shin
    jpTP(R_KNEE);
    jpRX(R_KNEE_FLEXION, model);
    jpRZ(R_KNEE_TORSION, model);
    jpTN(R_KNEE);
    display(R_SHIN);

    // Right foot
    glPushMatrix();	//  Right Ankle -> Right Foot
     jpTP(R_ANKLE);
     jpRX(R_ANKLE_FLEXION, model);
     jpRZ(R_ANKLE_TORSION, model);
     jpTN(R_ANKLE);
     display(R_FOOT);
    glPopMatrix();
   glPopMatrix();
  glPopMatrix();	// r_thigh
}

void Body::render(Pos& pos)
{
  if (!bap) return;

  float dtz = (bscale != 1) ?  B_HEIGHT/2 : 0;
  const GLfloat color[] = {.4,.4,.4,1};

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
