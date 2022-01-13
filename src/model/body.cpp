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


const GLfloat Phalanx2::PHALANX2_LEN = 0.012;	// default phalanx length
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
{ length = PHALANX2_LEN; }

int Phalanx2::init()
{ return 1; }

/** Phalanx */
Phalanx::Phalanx(Phalanx2 *_phalanx2)
{
  abend = 0;
  length = PHALANX_LEN;
  phalanx2 = _phalanx2;
}

int Phalanx::init()
{ return phalanx2->init(); }

void Phalanx::bend(GLfloat a)
{ abend = a; }

/** Finger */
Finger::Finger(Phalanx *_phalanx)
{
  abend = araise = 0;
  length = FINGER_LEN;
  phalanx = _phalanx;
}

int Finger::init()
{ return phalanx->init(); }

void Finger::bend(GLfloat a)
{ abend = a; }

void Finger::raise(GLfloat a)
{ araise = a; }

/** Hand */
Hand::Hand(Finger **_fingers)
{
  abend = araise = atwist = 0;
  length = HAND_LEN;
  fingers[0] = _fingers[0];
  fingers[1] = _fingers[1];
  fingers[2] = _fingers[2];
  fingers[3] = _fingers[3];
  fingers[4] = _fingers[4];
}

int Hand::init()
{ return fingers[4]->init(); }

void Hand::bend(GLfloat a)
{ abend = a; }

void Hand::raise(GLfloat a)
{ araise = a; }

void Hand::twist(GLfloat a)
{ atwist = a; }

/** Forearm / Elbow */
Forearm::Forearm(Hand *_hand)
{
  abend = atwist = 0;	// elbow
  length = FOREARM_LEN;
  hand = _hand;
}

int Forearm::init()
{ return hand->init(); }

void Forearm::bend(GLfloat a)
{ abend = a; }

void Forearm::twist(GLfloat a)
{ atwist = a; }

/** Arm */
Arm::Arm(Forearm *_forearm)
{
  araise = -90;
  abend = 0;
  atwist = 90;
  length = ARM_LEN;
}

int Arm::init()
{ return forearm->init(); }

void Arm::raise(GLfloat a)
{ araise = a - 90; }

void Arm::bend(GLfloat a)
{ abend = -a; }

void Arm::twist(GLfloat a)
{ atwist = a + 90; }

/** Shoulder */
Shoulder::Shoulder(Arm *_arm)
{
  aavance = araise = 0;
  radius = SHOULDER_RAD;
  arm = _arm;
}

int Shoulder::init()
{ return arm->init(); }

void Shoulder::avance(GLfloat a)
{ aavance = a; }

void Shoulder::raise(GLfloat a)
{ araise = a; }

/** HeadBody */
HeadBody::HeadBody()
{
  abend = araise = atwist = 0;
  radius = HEAD_RAD;
}

int HeadBody::init()
{ return 1; }

void HeadBody::bend(GLfloat a)
{ abend = a; }

void HeadBody::raise(GLfloat a)
{ araise = a; }

void HeadBody::twist(GLfloat a)
{ atwist = a; }

/** Neck */
Neck::Neck(HeadBody *_head)
{
  abend = araise = atwist = 0;
  length = NECK_LEN;
  head = _head;
}

int Neck::init()
{ return head->init(); }

void Neck::bend(GLfloat a)
{ abend = a; }

void Neck::raise(GLfloat a)
{ araise = a; }

void Neck::twist(GLfloat a)
{ atwist = a; }

/** Foot / Ankle */
Foot::Foot()
{
  length = FOOT_LEN;
  abend = araise = atwist = 0;
}

int Foot::init()
{ return 1; }

void Foot::bend(GLfloat a)
{ abend = -a; }

void Foot::raise(GLfloat a)
{ araise = -a; }

void Foot::twist(GLfloat a)
{ atwist = -a; }

/** Shin / Knee */
Shin::Shin(Foot *_foot)
{
  abend = 0;	// knee
  length = SHIN_LEN;
  foot = _foot;
}

int Shin::init()
{ return foot->init(); }

void Shin::bend(GLfloat a)
{ abend = -a; }

void Shin::twist(GLfloat a)
{ atwist = -a; }

/** Thigh / Hip */
Thigh::Thigh(Shin *_shin)
{
  araise = abend = atwist = 0;
  length = THIGH_LEN;
  shin = _shin;
}

int Thigh::init()
{ return shin->init(); }

void Thigh::raise(GLfloat a)
{ araise = a; }

void Thigh::bend(GLfloat a)
{ abend = a; }

void Thigh::twist(GLfloat a)
{ atwist = a; }

/** Chest */
Chest::Chest()
{}

void Chest::bend(GLfloat a)
{ abend = a; }

void Chest::raise(GLfloat a)
{ araise = a; }

void Chest::twist(GLfloat a)
{ atwist = a; }

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
  hand_r =     new Hand(fingers_r);
  hand_l =     new Hand(fingers_l);
  forearm_r =  new Forearm(hand_r);
  forearm_l =  new Forearm(hand_l);
  arm_r =      new Arm(forearm_r);
  arm_l =      new Arm(forearm_l);
  shoulder_r = new Shoulder(arm_r);
  shoulder_l = new Shoulder(arm_l);
  foot_r =     new Foot();
  foot_l =     new Foot();
  shin_r =     new Shin(foot_r);
  shin_l =     new Shin(foot_l);
  thigh_r =    new Thigh(shin_r);
  thigh_l =    new Thigh(shin_l);
  chest =      new Chest();

  for (int i=0; i<3 ; i++) {
    skin[i] = 1;
    cloth[i] = .5;
  }
}

Body::~Body()
{
  for (int i=0; i<10 ; i++) {
    delete phalanx2[i];
    delete phalanx[i];
  }
  for (int i=0; i<5 ; i++) {
    delete fingers_l[i];
    delete fingers_r[i];
  }
  delete hand_r;
  delete hand_l;
  delete forearm_r;
  delete forearm_l;
  delete arm_r;
  delete arm_l;
  delete shoulder_r;
  delete shoulder_l;
  delete head;
  delete neck;
  delete chest;
  delete thigh_r;
  delete thigh_l;
  delete shin_r;
  delete shin_l;
  delete foot_r;
  delete foot_l;

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
    //if ((p = strstr(jpline, "</body>"))) continue;
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

void Body::setJointPoint(uint8_t ind, float *_jp)
{
  jp.x[ind] = _jp[0];
  jp.y[ind] = _jp[1];
  jp.z[ind] = _jp[2];
}

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

  if (bap->is(PELVIC_TILT))        chest->raise(bap->get(PELVIC_TILT));
  if (bap->is(PELVIC_TORSION))     chest->twist(bap->get(PELVIC_TORSION));
  if (bap->is(PELVIC_ROLL))        chest->bend(bap->get(PELVIC_ROLL));

  if (bap->is(L_HIP_FLEXION))      thigh_l->bend(bap->get(L_HIP_FLEXION));
  if (bap->is(R_HIP_FLEXION))      thigh_r->bend(bap->get(R_HIP_FLEXION));
  if (bap->is(L_HIP_ABDUCT))       thigh_l->raise(bap->get(L_HIP_ABDUCT));
  if (bap->is(R_HIP_ABDUCT))       thigh_r->raise(bap->get(R_HIP_ABDUCT));
  if (bap->is(L_HIP_TWIST))        thigh_l->twist(bap->get(L_HIP_TWIST));
  if (bap->is(R_HIP_TWIST))        thigh_r->twist(bap->get(R_HIP_TWIST));

  if (bap->is(L_KNEE_FLEXION))     shin_l->bend(bap->get(L_KNEE_FLEXION));
  if (bap->is(R_KNEE_FLEXION))     shin_r->bend(bap->get(R_KNEE_FLEXION));
  if (bap->is(L_KNEE_TWIST))       shin_l->twist(bap->get(L_KNEE_TWIST));
  if (bap->is(R_KNEE_TWIST))       shin_r->twist(bap->get(R_KNEE_TWIST));

  if (bap->is(L_ANKLE_FLEXION))    foot_l->bend(bap->get(L_ANKLE_FLEXION));
  if (bap->is(R_ANKLE_FLEXION))    foot_r->bend(bap->get(R_ANKLE_FLEXION));
  if (bap->is(L_ANKLE_TWIST))      foot_l->twist(bap->get(L_ANKLE_TWIST));
  if (bap->is(R_ANKLE_TWIST))      foot_r->twist(bap->get(R_ANKLE_TWIST));

  switch (model) {
  case MODEL_OFF:
    if (bap->is(L_SHOULDER_FLEXION)) arm_l->bend(bap->get(L_SHOULDER_FLEXION));
    if (bap->is(R_SHOULDER_FLEXION)) arm_r->bend(bap->get(R_SHOULDER_FLEXION));
    if (bap->is(L_SHOULDER_ABDUCT))  arm_l->raise(bap->get(L_SHOULDER_ABDUCT));
    if (bap->is(R_SHOULDER_ABDUCT))  arm_r->raise(bap->get(R_SHOULDER_ABDUCT));
    break;
  case MODEL_OBJ:
    if (bap->is(L_SHOULDER_FLEXION)) arm_l->bend(bap->get(L_SHOULDER_FLEXION));
    if (bap->is(R_SHOULDER_FLEXION)) arm_r->bend(bap->get(R_SHOULDER_FLEXION));
    if (bap->is(L_SHOULDER_ABDUCT))  arm_l->raise(bap->get(L_SHOULDER_ABDUCT));
    if (bap->is(R_SHOULDER_ABDUCT))  arm_r->raise(bap->get(R_SHOULDER_ABDUCT));
    break;
  }
  if (bap->is(L_SHOULDER_TWIST))   arm_l->twist(bap->get(L_SHOULDER_TWIST));
  if (bap->is(R_SHOULDER_TWIST))   arm_r->twist(bap->get(R_SHOULDER_TWIST));

  if (bap->is(L_ELBOW_FLEXION))    forearm_l->bend(bap->get(L_ELBOW_FLEXION));
  if (bap->is(R_ELBOW_FLEXION))    forearm_r->bend(bap->get(R_ELBOW_FLEXION));

  if (bap->is(L_WRIST_FLEXION))    hand_l->raise(bap->get(L_WRIST_FLEXION));
  if (bap->is(R_WRIST_FLEXION))    hand_r->raise(bap->get(R_WRIST_FLEXION));
  if (bap->is(L_WRIST_PIVOT))      hand_l->bend(bap->get(L_WRIST_PIVOT));
  if (bap->is(R_WRIST_PIVOT))      hand_r->bend(bap->get(R_WRIST_PIVOT));
  if (bap->is(L_WRIST_TWIST))      hand_l->twist(bap->get(L_WRIST_TWIST));
  if (bap->is(R_WRIST_TWIST))      hand_r->twist(bap->get(R_WRIST_TWIST));

  if (bap->is(L_THUMB_FLEXION1))   fingers_l[0]->raise(bap->get(L_THUMB_FLEXION1));
  if (bap->is(R_THUMB_FLEXION1))   fingers_l[0]->raise(bap->get(R_THUMB_FLEXION1));
  if (bap->is(L_INDEX_FLEXION1))   fingers_l[1]->raise(bap->get(L_INDEX_FLEXION1));
  if (bap->is(R_INDEX_FLEXION1))   fingers_l[1]->raise(bap->get(R_INDEX_FLEXION1));
  if (bap->is(L_MIDDLE_FLEXION1))  fingers_l[2]->raise(bap->get(L_MIDDLE_FLEXION1));
  if (bap->is(R_MIDDLE_FLEXION1))  fingers_l[2]->raise(bap->get(R_MIDDLE_FLEXION1));
  if (bap->is(L_RING_FLEXION1))    fingers_l[3]->raise(bap->get(L_RING_FLEXION1));
  if (bap->is(R_RING_FLEXION1))    fingers_l[3]->raise(bap->get(R_RING_FLEXION1));
  if (bap->is(L_PINKY_FLEXION1))   fingers_l[4]->raise(bap->get(L_PINKY_FLEXION1));
  if (bap->is(R_PINKY_FLEXION1))   fingers_r[4]->raise(bap->get(R_PINKY_FLEXION1));

  if (bap->is(C1_ROLL))            head->bend(bap->get(C1_ROLL));
  if (bap->is(C1_TORSION))         head->twist(bap->get(C1_TORSION));
  if (bap->is(C1_TILT))            head->raise(bap->get(C1_TILT));

  if (bap->is(C4_ROLL))            neck->bend(bap->get(C4_ROLL));
  if (bap->is(C4_TORSION))         neck->twist(bap->get(C4_TORSION));
  if (bap->is(C4_TILT))            neck->raise(bap->get(C4_TILT));

  if (bap->is(T1_ROLL))            neck->raise(bap->get(T1_ROLL));
  if (bap->is(T1_TORSION))         neck->twist(bap->get(T1_TORSION));
  if (bap->is(T1_TILT))            neck->bend(bap->get(T1_TILT));

  if (bap->is(TR_VERTICAL))        tz = bap->get(TR_VERTICAL) / TR_DIV;
  if (bap->is(TR_LATERAL))         ty = bap->get(TR_LATERAL) / TR_DIV;
  if (bap->is(TR_FRONTAL))         tx = bap->get(TR_FRONTAL) / TR_DIV;

  if (bap->is(RT_BODY_TURN))       rz = bap->get(RT_BODY_TURN);
  if (bap->is(RT_BODY_ROLL))       rx = bap->get(RT_BODY_ROLL);
  if (bap->is(RT_BODY_TILT))       ry = bap->get(RT_BODY_TILT);
}

#if 0 //dax
/** Animates body articulations */
void Body::anim(int param)
{
  switch (param) {
  case PELVIC_TILT:    chest->raise(bap->get(PELVIC_TILT)); break;
  case PELVIC_TORSION: chest->twist(bap->get(PELVIC_TORSION)); break;
  case PELVIC_ROLL:    chest->bend(bap->get(PELVIC_ROLL)); break;

  case L_HIP_FLEXION: thigh_l->bend(bap->get(L_HIP_FLEXION)); break;
  case R_HIP_FLEXION: thigh_r->bend(bap->get(R_HIP_FLEXION)); break;
  case L_HIP_ABDUCT:  thigh_l->raise(bap->get(L_HIP_ABDUCT)); break;
  case R_HIP_ABDUCT:  thigh_r->raise(bap->get(R_HIP_ABDUCT)); break;
  case L_HIP_TWIST:   thigh_l->twist(bap->get(L_HIP_TWIST)); break;
  case R_HIP_TWIST:   thigh_r->twist(bap->get(R_HIP_TWIST)); break;

  case L_KNEE_FLEXION: shin_l->bend(bap->get(L_KNEE_FLEXION)); break;
  case R_KNEE_FLEXION: shin_r->bend(bap->get(R_KNEE_FLEXION)); break;
  case L_KNEE_TWIST:   shin_l->twist(bap->get(L_KNEE_TWIST)); break;
  case R_KNEE_TWIST:   shin_r->twist(bap->get(R_KNEE_TWIST)); break;

  case L_ANKLE_FLEXION: foot_l->bend(bap->get(L_ANKLE_FLEXION)); break;
  case R_ANKLE_FLEXION: foot_r->bend(bap->get(R_ANKLE_FLEXION)); break;
  case L_ANKLE_TWIST:   foot_l->twist(bap->get(L_ANKLE_TWIST)); break;
  case R_ANKLE_TWIST:   foot_r->twist(bap->get(R_ANKLE_TWIST)); break;

  case L_SHOULDER_FLEXION: arm_l->bend(bap->get(L_SHOULDER_FLEXION)); break;
  case R_SHOULDER_FLEXION: arm_r->bend(bap->get(R_SHOULDER_FLEXION)); break;
  case L_SHOULDER_ABDUCT:  arm_l->raise(bap->get(L_SHOULDER_ABDUCT)); break;
  case R_SHOULDER_ABDUCT:  arm_r->raise(bap->get(R_SHOULDER_ABDUCT)); break;
  case L_SHOULDER_TWIST:   arm_l->twist(bap->get(L_SHOULDER_TWIST)); break;
  case R_SHOULDER_TWIST:   arm_r->twist(bap->get(R_SHOULDER_TWIST)); break;

  case L_ELBOW_FLEXION: forearm_l->bend(bap->get(L_ELBOW_FLEXION)); break;
  case R_ELBOW_FLEXION: forearm_r->bend(bap->get(R_ELBOW_FLEXION)); break;

  case L_WRIST_FLEXION: hand_l->raise(bap->get(L_WRIST_FLEXION)); break;
  case R_WRIST_FLEXION: hand_r->raise(bap->get(R_WRIST_FLEXION)); break;
  case L_WRIST_PIVOT:   hand_l->bend(bap->get(L_WRIST_PIVOT)); break;
  case R_WRIST_PIVOT:   hand_r->bend(bap->get(R_WRIST_PIVOT)); break;
  case L_WRIST_TWIST:   hand_l->twist(bap->get(L_WRIST_TWIST)); break;
  case R_WRIST_TWIST:   hand_r->twist(bap->get(R_WRIST_TWIST)); break;

  case C1_ROLL:    head->bend(bap->get(C1_ROLL)); break;
  case C1_TORSION: head->twist(bap->get(C1_TORSION)); break;
  case C1_TILT:    head->raise(bap->get(C1_TILT)); break;

  case C4_ROLL:    neck->bend(bap->get(C4_ROLL)); break;
  case C4_TORSION: neck->twist(bap->get(C4_TORSION)); break;
  case C4_TILT:    neck->raise(bap->get(C4_TILT)); break;

  case T1_ROLL:    neck->raise(bap->get(T1_ROLL)); break;
  case T1_TORSION: neck->twist(bap->get(T1_TORSION)); break;
  case T1_TILT:    neck->bend(bap->get(T1_TILT)); break;

  case TR_VERTICAL:  tz = bap->get(TR_VERTICAL) / TR_DIV; break;
  case TR_LATERAL:   ty = bap->get(TR_LATERAL) / TR_DIV; break;
  case TR_FRONTAL:   tx = bap->get(TR_FRONTAL) / TR_DIV; break;

  case RT_BODY_TURN: rz = bap->get(RT_BODY_TURN); break;
  case RT_BODY_ROLL: rx = bap->get(RT_BODY_ROLL); break;
  case RT_BODY_TILT: ry = bap->get(RT_BODY_TILT); break;
  }
}
#endif

bool Body::isLoaded(uint8_t part)
{
  if (part < MAX_PARTS)
    return bodyparts[part].loaded;
  return false;
}

void Body::jpGo(uint8_t part)
{
  if (part < MAX_PARTS) {
    if (model == MODEL_OBJ) glTranslatef(jp.y[part], jp.x[part], jp.z[part]);
    else                    glTranslatef(jp.x[part], jp.y[part], jp.z[part]);
  }
}

void Body::jpBack(uint8_t part)
{
  if (part < MAX_PARTS) {
    if (model == MODEL_OBJ) glTranslatef(-jp.y[part], -jp.x[part], -jp.z[part]);
    else                    glTranslatef(-jp.x[part], -jp.y[part], -jp.z[part]);
  }
}

void Body::display(uint8_t part)
{
  if (part < MAX_PARTS) {
    glMaterialfv(GL_FRONT, GL_DIFFUSE, bodyparts[part].color);
    if (bodyparts[part].texid) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, bodyparts[part].texid);
    }
    else
      glColor3fv(bodyparts[part].color);

    glCallList(dlist + part);

    glDisable(GL_TEXTURE_2D);
  }
}

/** display body and face */
void Body::display()
{
  glPushMatrix();	//  Pelvic (Body Bottom)
   jpGo(PELVIC);
   bap->jpRX(PELVIC_TILT, model);
   bap->jpRY(PELVIC_ROLL, model);
   bap->jpRZ(PELVIC_TORSION, model);
   jpBack(PELVIC);
   display(HIPS);

   if (isLoaded(ABDOMEN)) {
     glPushMatrix();
     display(ABDOMEN);
     glPopMatrix();
   }

   if (isLoaded(SKIRT)) {
     glPushMatrix();
     display(SKIRT);
     glPopMatrix();
   }
   if (isLoaded(BELT)) {
     glPushMatrix();
     display(BELT);
     glPopMatrix();
   }

   glPushMatrix();	//  Spinal -> Chest (thoracic level 5)
    jpGo(SPINAL);
    bap->jpRX(T5_TILT, model);
    bap->jpRY(T5_ROLL, model);
    bap->jpRZ(T5_TORSION, model);
    jpBack(SPINAL);
    display(CHEST);

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

    glPushMatrix();	//  Lower Neck (cervical level 4)
     jpGo(LOWER_NECK);
     bap->jpRX(C4_TILT, model);
     bap->jpRY(C4_ROLL, model);
     bap->jpRZ(C4_TORSION, model);
     jpBack(LOWER_NECK);
     display(NECK);

     if (! face) display(HEAD);

     glPushMatrix();	//  Upper Neck -> Head (cervical level 1)
      jpGo(UPPER_NECK);
      bap->jpRX(C1_TILT, model);
      bap->jpRY(C1_ROLL, model);
      bap->jpRZ(C1_TORSION, model);
      if (face) {
        glScalef(Face::SCALE, Face::SCALE, Face::SCALE);
        glTranslatef(0, 0.9, -0.9);
        glRotatef(90, 1,0,0);
        face->render();	// YR
      }
      jpGo(UPPER_NECK);
      bap->jpRX(C1_TILT, model);
      bap->jpRY(C1_ROLL, model);
      bap->jpRZ(C1_TORSION, model);
      jpBack(UPPER_NECK);
     glPopMatrix(); 	// head
    glPopMatrix(); 	// neck

    glPushMatrix();	//  Left Shoulder -> Left Arm
     jpGo(L_SHOULDER);
     if (model == MODEL_OFF)
       glRotatef(-90, 0,0,1);  //FIXME
     bap->jpRX(L_SHOULDER_ABDUCT, model);
     bap->jpRY(-L_SHOULDER_FLEXION, model);
     bap->jpRZ(-L_SHOULDER_TWIST, model);
     jpBack(L_SHOULDER);
     display(L_ARM);

     glPushMatrix();	//  Left Elbow -> Left Forearm
      jpGo(L_ELBOW);
      bap->jpRY(-L_ELBOW_FLEXION, model);
      bap->jpRZ(-L_ELBOW_TWIST, model);
      jpBack(L_ELBOW);
      display(L_FOREARM);

      glPushMatrix();	//  Left Wrist -> Left Hand
       jpGo(L_WRIST);
       bap->jpRX(L_WRIST_FLEXION, model);
       bap->jpRY(L_WRIST_PIVOT, model);
       bap->jpRZ(L_WRIST_TWIST, model);
       jpBack(L_WRIST);
       display(L_HAND);
      glPopMatrix();
     glPopMatrix();
    glPopMatrix();	// l_shoulder

    glPushMatrix();	//  Right Shoulder -> Right Arm
     jpGo(R_SHOULDER);
     if (model == MODEL_OFF)
       glRotatef(90, 0,0,1);	//OK but FIXME
     bap->jpRX(-R_SHOULDER_ABDUCT, model);
     bap->jpRY(R_SHOULDER_FLEXION, model);
     bap->jpRZ(-R_SHOULDER_TWIST, model);
     jpBack(R_SHOULDER);
     display(R_ARM);

     glPushMatrix();	//  Right Elbow -> Right Forearm
      jpGo(R_ELBOW);
      bap->jpRY(R_ELBOW_FLEXION, model);
      bap->jpRZ(-R_ELBOW_TWIST, model);
      jpBack(R_ELBOW);
      display(R_FOREARM);

      glPushMatrix();	//  Right Wrist -> Right Hand
       jpGo(R_WRIST);
       bap->jpRX(R_WRIST_FLEXION, model);
       bap->jpRY(R_WRIST_PIVOT, model);
       bap->jpRZ(R_WRIST_TWIST, model);
       jpBack(R_WRIST);

#if 0 //dax fingers
       glPushMatrix();	//  Right fingers
       glPushMatrix();	//  Right thumb
        jpGo(R_THUMB);
        bap->jpRX(R_THUMB_FLEXION1, model);
        jpBack(R_THUMB);
        //display(R_THUMB);
       glPopMatrix();
       glPushMatrix();	//  Right index
        jpGo(R_INDEX);
        bap->jpRX(R_INDEX_FLEXION1, model);
        jpBack(R_INDEX);
        //display(R_INDEX);
       glPopMatrix();
       glPushMatrix();	//  Right middle
        jpGo(R_MIDDLE);
        bap->jpRX(R_MIDDLE_FLEXION1, model);
        jpBack(R_MIDDLE);
        //display(R_MIDDLE);
       glPopMatrix();
       glPushMatrix();	//  Right ring
        jpGo(R_RING);
        bap->jpRX(R_RING_FLEXION1, model);
        jpBack(R_RING);
        //display(R_RING);
       glPopMatrix();
       glPushMatrix();	//  Right pinky
        jpGo(R_PINKY);
        bap->jpRX(R_PINKY_FLEXION1, model);
        jpBack(R_PINKY);
        //display(R_PINKY);
        display(R_HAND);
       glPopMatrix();
       glPopMatrix();
#else
       display(R_HAND);
#endif
      glPopMatrix();
     glPopMatrix();
    glPopMatrix();	// r_shoulder
   glPopMatrix();	// spinal
  glPopMatrix();	// pelvic

  glPushMatrix();	//  Left Hip -> Left Thigh
   jpGo(L_HIP);
   bap->jpRX(L_HIP_FLEXION, model);
   bap->jpRY(L_HIP_ABDUCT, model);
   bap->jpRZ(L_HIP_TWIST, model);
   jpBack(L_HIP);
   display(L_THIGH);

   glPushMatrix();	//  Left Knee -> Left Shin
    jpGo(L_KNEE);
    bap->jpRX(L_KNEE_FLEXION, model);
    bap->jpRZ(L_KNEE_TWIST, model);
    jpBack(L_KNEE);
    display(L_SHIN);

    glPushMatrix();	//  Left Ankle -> Left Foot
     jpGo(L_ANKLE);
     bap->jpRX(L_ANKLE_FLEXION, model);
     bap->jpRZ(L_ANKLE_TWIST, model);
     jpBack(L_ANKLE);
     display(L_FOOT);
    glPopMatrix();
   glPopMatrix();
  glPopMatrix();	// l_thigh

  glPushMatrix();	//  Right Hip -> Right Thigh
   jpGo(R_HIP);
   bap->jpRX(R_HIP_FLEXION, model);
   bap->jpRY(R_HIP_ABDUCT, model);
   bap->jpRZ(R_HIP_TWIST, model);
   jpBack(R_HIP);
   display(R_THIGH);

   glPushMatrix();	//  Right Knee -> Right Shin
    jpGo(R_KNEE);
    bap->jpRX(R_KNEE_FLEXION, model);
    bap->jpRZ(R_KNEE_TWIST, model);
    jpBack(R_KNEE);
    display(R_SHIN);

    glPushMatrix();	//  Right Ankle -> Right Foot
     jpGo(R_ANKLE);
     bap->jpRX(R_ANKLE_FLEXION, model);
     bap->jpRZ(R_ANKLE_TWIST, model);
     jpBack(R_ANKLE);
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
