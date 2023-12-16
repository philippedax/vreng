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
// ------------------------------------------------------------ //
// Author   : This file has been written by Yann Renard         //
// Copyright: This file is totaly free and you may distribute   //
//            it to anyone you want, without modifying this     //
//            header. If you use it in a commercial project (?) //
//            or in bigger project (!), I would be glad to know //
//            about it :) Please mail me...                     //
//            be glad to know about it, please mail me          //
//                myself_yr@hotmail.com                         //
// ------------------------------------------------------------ //
#include "vreng.hpp"
#include "v3d.hpp"
#include "humanoid.hpp"
#include "fap.hpp"
#include "body.hpp"	// body
#include "http.hpp"	// httpOpen
#include "cache.hpp"	// setCachePath, open
#include "bone.hpp"	// V3d


static const char headRoot[]		= "root";
static const char lipsRoot[]		= "lipsRoot";
static const char eyeLeftRoot[]		= "eyeLeftRoot";
static const char eyeRightRoot[]	= "eyeRightRoot";
static const char eyeLeftTopRoot[]	= "eyeLeftTopRoot";
static const char eyeRightTopRoot[]	= "eyeRightTopRoot";
static const char eyeLeftBotRoot[]	= "eyeLeftBotRoot";
static const char eyeRightBotRoot[]	= "eyeRightBotRoot";
static const char browLeftRoot[]	= "sourcilLeftRoot";
static const char browRightRoot[]	= "sourcilRightRoot";
static const char noseRoot[]		= "noseRoot";

static const char eyeLeftTopL[]		= "eyeLeftTopL";
static const char eyeRightTopL[]	= "eyeRightTopL";
static const char eyeLeftTopR[]		= "eyeLeftTopR";
static const char eyeRightTopR[]	= "eyeRightTopR";
static const char eyeLeftBotL[]		= "eyeLeftBotL";
static const char eyeRightBotL[]	= "eyeRightBotL";
static const char eyeLeftBotR[]		= "eyeLeftBotR";
static const char eyeRightBotR[]	= "eyeRightBotR";
static const char browRightL[]		= "sourcilRightL";
static const char browRightR[]		= "sourcilRightR";
static const char noseLeft[]		= "noseLeft";
static const char noseRight[]		= "noseRight";
static const char lipsTopL[]		= "lipsTopL";
static const char lipsTopR[]		= "lipsTopR";
static const char lipsBotL[]		= "lipsBotL";
static const char lipsBotR[]		= "lipsBotR";

const float V3d::SCALE = 0.075;	///< 3/400


V3d::V3d()
{
  mesh = NULL;
  root = NULL;
  moveYes = true;
  moveNo = true;
  moveMouth = false;
  moveSmile = false;
  moveSulk = false;
  moveEyeL = false;
  moveEyeR = false;
  moveNose = false;
  indexed = false;
  cachefile[0] = '\0';
}

V3d::V3d(const char *urls)
{
  mesh = NULL;
  root = NULL;
  moveYes = true;
  moveNo = true;
  moveMouth = false;
  moveSmile = false;
  moveSulk = false;
  moveEyeL = false;
  moveEyeR = false;
  moveNose = false;
  indexed = true;
  cachefile[0] = '\0';
  urlList.empty();
  Http::httpOpen(urls, v3dsreader, this, 0);
  currentUrl = rand() % urlList.count();
}

V3d::~V3d()
{
  if (mesh) delete mesh;
  mesh = NULL;
  if (root) delete root;
  root = NULL;
}

/** Caching file */
void V3d::reader(void *_url, Http *http)
{
  char *url = (char *) _url;
  if (! http) {
    error("reader: unable to open http connection");
    return;
  }
  Cache *cache = new Cache();
  FILE *f = cache->open(url, http);
}

/** Download list of v3ds url */
void V3d::v3dsreader(void *_v3d, Http *http)
{
  V3d *v3d = (V3d *) _v3d;
  if (! http) {
    error("v3dsreader: unable to open http connection");
    return;
  }

  char line[URL_LEN];

  Cache *cache = new Cache();
  FILE *f = cache->open(http->url, http);
  //http->reset();
  //while (http->getLine(line)) {
  while (cache->nextLine(f, line)) {
    char *v3durl = strdup(line);
    //echo("v3dsreader: add url=%s", v3durl);
    v3d->urlList.addElement(v3durl);
    free(v3durl);
  }
}

void V3d::change()
{
  if (! indexed) return;

  currentUrl++;
  currentUrl %= urlList.count();
  char *urlv3d = urlList.getElemAt(currentUrl);
  echo("change: urlv3d=%s", urlv3d);
  if (! isascii(urlv3d[0])) {
    error("change: BUG! urlv3d=%02x", urlv3d[0]);
    return;
  }
  load(urlv3d);
}

void V3d::load(const char *url)
{
  BoneMesh   *newmesh = new BoneMesh();
  BoneVertex *newroot = new BoneVertex();

  if (Cache::setCachePath(url, cachefile) == 0) {
    error("V3d load: file=%s url=%s", cachefile, url);
    return;
  }
  Http::httpOpen(url, reader, (void *)url, 0);

  readV3Dfile(newmesh, newroot, cachefile);

  bone.registerMesh(newmesh);
  bone.registerSkeleton(newroot);
  bone.generateLinkList();

  if (mesh) delete mesh;
  mesh = newmesh;
  if (root) delete root;
  root = newroot;
}

void V3d::render()
{
  if (! mesh) return;
  if (bone.meshToMove && bone.skeleton) {
    bone.animate();
    bone.render();
  }
}

void V3d::animHead(float angle, int x, int y, int z)
{
  BoneVertex *bone;

  // 0,1,0 pitch (yes)
  // 1,0,0 yaw   (no)
  // 0,0,1 roll  (maybe)
  //echo("animHead: angle=%.2f", angle);
  if ((bone = root->findBone(headRoot)) != NULL)
    bone->setRot(sin(angle/50.0) *10 , x, y, z);
  else
    error("headRoot not found");
}

void V3d::animNose(float angle, const char *_side)
{
  BoneVertex *bone;
  float scale = 1 - cos(angle / 16.) / 4.;

  //echo("animNose: angle=%.2f scale=%.2f", angle, scale);
  if ((bone = root->findBone(noseRoot)) != NULL) {
    if ((bone = root->findBone(_side)) != NULL) {
      bone->resetPos();
      bone->setScale(scale, 1, 1);
    }
  }
  else
    error("noseRoot not found");
}

void V3d::animEyeBall(float angle, const char *_side, int dir)
{
  BoneVertex *bone;
  float scale = 1 - cos(angle / 16.) /* / 2. */;

  //echo("animEyeBall: angle=%.2f scale=%.2f dir=%d", angle, scale, dir);
    if ((bone = root->findBone(_side)) != NULL) {
      bone->resetPos();
      bone->setScale(scale, 1, 1);
      if (dir)
        bone->setRot((1-scale) * 20., 0,1,0);	// pitch
      else
        bone->setRot((1-scale) * 20., 1,0,0);	// yaw
      bone->resetPos();
    }
  else
    error("%s not found", _side);
}

void V3d::animEyeLid(float angle, const char *root1, const char *lid, const char *left, const char *right)
{
  BoneVertex *bone;
  float scale = (1 - cos(angle / 10.)) /* / 2 */;

  if ((bone = root->findBone(root1)) != NULL) {
    if ((bone = root->findBone(lid)) != NULL) {
      int sign;
      if (!strcmp(lid, eyeLeftTopRoot) || !strcmp(lid, eyeRightTopRoot))
        sign = 1;
      else
        sign = -1;
      //echo("animEyeLid: angle=%.2f scale=%.2f rot=%.2f", angle, scale, sign*(1-scale)*20);
      bone->resetPos();
      bone->setScale(1, scale, 1);
      bone->setRot(sign * (1-scale) * 20., 1,0,0);
    }
    else
      error("%s not found", lid);
    if ((bone = root->findBone(left)) != NULL) {
      bone->resetPos();
      bone->setScale(1, scale, 1);
    }
    if ((bone = root->findBone(right)) != NULL) {
      bone->resetPos();
      bone->setScale(1, scale, 1);
    }
  }
  else
    error("%s not found", root1);
}

void V3d::animEyeBrow(float angle, const char *_root, const char *_side)
{
  BoneVertex *bone;
  float scale = cos(angle / 5.0);

  //echo("animEyeBrow: angle=%.2f scale=%.2f", angle, scale);
  if ((bone = root->findBone(_root)) != NULL) {
    bone->resetPos();
    bone->setTrans(0, scale / 25.0, 0);
  }
  else
    error("%s not found", _root);
  if ((bone = root->findBone(_side)) != NULL) {
    bone->resetPos();
    bone->setRot(scale * 10, 0,0,1);
    bone->setTrans(0, scale / 25.0, 0);
  }
  else
    error("%s not found", _side);
}

void V3d::animLip(float angle, const char *_side)
{
  BoneVertex *bone;

  if ((bone = root->findBone(lipsRoot)) != NULL) {
    Vect3D delta(0, cos(angle/ 10.0) / 4., 0);
    float smile = cos(angle / 10.0) * 20;

    //echo("animLip: angle=%.2f smile=%.2f", angle, smile);
    if ((bone = root->findBone(_side)) != NULL) {
      bone->resetPos();
      bone->setTrans(delta);
      //dax bone->setRot(smile, 0,0,1);
      bone->setRot(angle, 0,0,1);
    }
  }
  else
    error("lipsRoot not found");
}

void V3d::animate(int fapn, int a)
{
  //echo("fap: %d %d", fapn, a);
  switch (fapn) {

  case VISEME:			// 1
    //if (a) echo("VISEME %s", e_not_implemented);
    break;
  case EXPRESSION:		// 2
    //if (a) echo("EXPRESSION %s", e_not_implemented);
    break;
  case OPEN_JAW:		// 3
    //if (a) echo("OPEN_JAW %s", e_not_implemented);
    break;
  case LOWER_T_MIDLIP:		// 4
    //if (a) echo("LOWER_T_MIDLIP %s a=%d", e_not_implemented, a);
    break;
  case RAISE_B_MIDLIP:		// 5
    //if (a) echo("RAISE_B_MIDLIP %s a=%d", e_not_implemented, a);
    break;
  case STRETCH_L_CORNERLIP:	// 6
    //if (a) echo("STRETCH_L_CORNERLIP %s a=%d", e_not_implemented, a);
    break;
  case STRETCH_R_CORNERLIP:	// 7
    //if (a) echo("STRETCH_R_CORNERLIP %s a=%d", e_not_implemented, a);
    break;
  case LOWER_T_LIP_LM:		// 8
    if (a) //echo("LOWER_T_LIP_LM a=%d", a);
    animLip(a, lipsTopL);
    break;
  case LOWER_T_LIP_RM:		// 9
    if (a) //echo("LOWER_T_LIP_RM a=%d", a);
    animLip(a, lipsTopR);
    break;
  case RAISE_B_LIP_LM:		// 10
    if (a) //echo("LOWER_B_LIP_LM a=%d", a);
    animLip(a, lipsBotL);
    break;
  case RAISE_B_LIP_RM:		// 11
    if (a) //echo("LOWER_B_LIP_RM a=%d", a);
    animLip(a, lipsBotR);
    break;
  case RAISE_L_CORNERLIP:	// 12
  case RAISE_R_CORNERLIP:	// 13
    //if (a) echo("RAISE_CORNERLIP %s a=%d", e_not_implemented, a);
    break;
  case THRUST_JAW:		// 14
    //if (a) echo("THRUST_JAW %s a=%d", e_not_implemented, a);
    break;
  case SHIFT_JAW:		// 15
    //if (a) echo("SHIFT_JAW %s a=%d", e_not_implemented, a);
    break;
  case PUSH_B_LIP:		// 16
    //if (a) echo("PUSH_B_LIP %s a=%d", e_not_implemented, a);
    break;
  case PUSH_T_LIP:		// 17
    //if (a) echo("PUSH_T_LIP %s a=%d", e_not_implemented, a);
    break;
  case DEPRESS_CHIN:		// 18 menton
    //if (a) echo("DEPRESS_CHIN %s a=%d", e_not_implemented, a);
    break;
  case CLOSE_T_L_EYELID:	// 19
    animEyeLid(a, eyeLeftRoot, eyeLeftTopRoot, eyeLeftTopL, eyeLeftTopR);
    break;
  case CLOSE_T_R_EYELID:	// 20
    animEyeLid(a, eyeRightRoot, eyeRightTopRoot, eyeRightTopL, eyeRightTopR);
    break;
  case CLOSE_B_L_EYELID:	// 21
    animEyeLid(a, eyeLeftRoot, eyeLeftBotRoot, eyeLeftBotL, eyeLeftBotR);
    break;
  case CLOSE_B_R_EYELID:	// 22
    animEyeLid(a, eyeRightRoot, eyeRightBotRoot, eyeRightBotL, eyeRightBotR);
    break;
  case YAW_L_EYEBALL:		// 23
    //if (a) echo("YAW_L_EYEBALL %s a=%d", e_not_implemented, a);
    //dax animEyeBall(a, eyeLeftRoot, 0);
    break;
  case YAW_R_EYEBALL:		// 24
    //if (a) echo("YAW_R_EYEBALL %s a=%d", e_not_implemented, a);
    //dax animEyeBall(a, eyeRightRoot, 0);
    break;
  case PITCH_L_EYEBALL:		// 25
    //if (a) echo("PITCH_L_EYEBALL %s a=%d", e_not_implemented, a);
    //dax animEyeBall(a, eyeLeftRoot, 1);
    break;
  case PITCH_R_EYEBALL:		// 26
    //if (a) echo("PITCH_R_EYEBALL %s a=%d", e_not_implemented, a);
    //dax animEyeBall(a, eyeRightRoot, 1);
    break;
  case THRUST_L_EYEBALL:	// 27
  case THRUST_R_EYEBALL:	// 28
    //if (a) echo("THRUST_EYEBALL %s a=%d", e_not_implemented, a);
    break;
  case DILATE_L_PUPIL:		// 29
  case DILATE_R_PUPIL:		// 30
    //if (a) echo("DILATE_PUPIL %s a=%d", e_not_implemented, a);
    break;
  case RAISE_L_I_EYEBROW:	// 31
    if (a) //echo("RAISE_L_I_EYEBROW a=%d", a);
    animEyeBrow(a, browLeftRoot, browRightL);
    break;
  case RAISE_R_I_EYEBROW:	// 32
    if (a) //echo("RAISE_R_I_EYEBROW a=%d", a);
    animEyeBrow(a, browRightRoot, browRightR);
    break;
  case RAISE_L_M_EYEBROW:	// 33
    if (a) //echo("RAISE_L_M_EYEBROW a=%d", a);
    animEyeBrow(a, browLeftRoot, browRightL);
    break;
  case RAISE_R_M_EYEBROW:	// 34
    if (a) //echo("RAISE_R_M_EYEBROW a=%d", a);
    animEyeBrow(a, browRightRoot, browRightR);
    break;
  case RAISE_L_O_EYEBROW:	// 35
    if (a) //echo("RAISE_L_O_EYEBROW a=%d", a);
    animEyeBrow(a, browLeftRoot, browRightL);
    break;
  case RAISE_R_O_EYEBROW:	// 36
    if (a) //echo("RAISE_R_O_EYEBROW a=%d", a);
    animEyeBrow(a, browRightRoot, browRightR);
    break;
  case SQUEEZE_L_EYEBROW:	// 37
    //if (a) echo("SQUEEZE_L_EYEBROW %s a=%d", e_not_implemented, a);
    break;
  case SQUEEZE_R_EYEBROW:	// 38
    //if (a) echo("SQUEEZE_R_EYEBROW %s a=%d", e_not_implemented, a);
    break;
  case PUFF_L_CHEEK:		// 39
  case PUFF_R_CHEEK:		// 40
    //if (a) echo("PUFF_CHEEK %s", e_not_implemented);
    break;
  case LIFT_L_CHEEK:		// 41
  case LIFT_R_CHEEK:		// 42
    //if (a) echo("LIFT_CHEEK %s", e_not_implemented);
    break;
  case SHIFT_TONGUE_TIP:	// 43
  case RAISE_TONGUE_TIP:	// 44
  case THRUST_TONGUE_TIP:	// 45
  case RAISE_TONGUE:		// 46
  case TONGUE_ROLL:		// 47
    //if (a) echo("TONGUE %s", e_not_implemented);
    break;
  case HEAD_PITCH:		// 48
    if (a) //echo("HEAD_PITCH a=%d", a);
    animHead(a, 0, 1, 0);	// yes
    break;
  case HEAD_YAW:		// 49
    if (a) //echo("HEAD_YAW a=%d", a);
    animHead(a, 1, 0, 0);	// no
    break;
  case HEAD_ROLL:		// 50
    if (a) //echo("HEAD_ROLL a=%d", a);
    animHead(a, 0, 0, 1);	// maybe
    break;
  case LOWER_T_MIDLIP_O:	// 51
    //if (a) echo("LOWER_T_MIDLIP_O %s a=%d", e_not_implemented, a);
    break;
  case RAISE_B_MIDLIP_O:	// 52
    //if (a) echo("RAISE_B_MIDLIP_O %s a=%d", e_not_implemented, a);
    break;
  case STRETCH_L_CORNERLIP_O:	// 53
    //if (a) echo("STRETCH_L_CORNERLIP_O %s a=%d", e_not_implemented, a);
    break;
  case STRETCH_R_CORNERLIP_O:	// 54
    //if (a) echo("STRETCH_R_CORNERLIP_O %s a=%d", e_not_implemented, a);
    break;
  case LOWER_T_LIP_LM_O:	// 55
    if (a) //echo("LOWER_T_LIP_LM_O a=%d", a);
    animLip(a, lipsTopL);
    break;
  case LOWER_T_LIP_RM_O:	// 56
    if (a) //echo("LOWER_T_LIP_RM_O a=%d", a);
    animLip(a, lipsTopR);
    break;
  case RAISE_B_LIP_LM_O:	// 57
    if (a) //echo("RAISE_B_LIP_LM_O a=%d", a);
    animLip(a, lipsBotL);
    break;
  case RAISE_B_LIP_RM_O:	// 58
    if (a) //echo("RAISE_B_LIP_RM_O a=%d", a);
    animLip(a, lipsBotR);
    break;
  case RAISE_L_CORNERLIP_O:	// 59
    //if (a) echo("RAISE_L_CORNERLIP_O %s a=%d", e_not_implemented, a);
    break;
  case RAISE_R_CORNERLIP_O:	// 60
    //if (a) echo("RAISE_R_CORNERLIP_O %s a=%d", e_not_implemented, a);
    break;
  case STRETCH_L_NOSE:		// 61
    if (a) //echo("STRETCH_L_NOSE a=%d", a);
    animNose(a, noseLeft);
    break;
  case STRETCH_R_NOSE:		// 62
    if (a) //echo("STRETCH_R_NOSE a=%d", a);
    animNose(a, noseRight);
    break;
  case RAISE_NOSE:		// 63
    //if (a) echo("RAISE_NOSE %s", e_not_implemented);
    break;
  case BEND_NOSE:		// 64
    //if (a) echo("BEND_NOSE %s", e_not_implemented);
    break;
  case RAISE_L_EAR:		// 65
  case RAISE_R_EAR:		// 66
  case PULL_L_EAR:		// 67
  case PULL_R_EAR:		// 68
    //if (a) echo("EAR %s", e_not_implemented);
    break;
  default:
    error("bad fap number=%d", fapn);
  }
}

void V3d::animate()
{
  static float angle = 0;
  angle += 5.;

  BoneVertex *bone;
  // --- Lips management ---
  // == smile then sulk
  if ( moveMouth ) {
    if ((bone = root->findBone(lipsRoot)) != NULL) {
      Vect3D smileDelta(0, cos(angle/10.0) / 4.0, 0);
      float smile = 20 * cos(angle / 10.0);
      if ((bone = root->findBone(lipsTopL)) != NULL) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(smile, 0,0,1);
      }
      if ((bone = root->findBone(lipsTopR)) != NULL) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(-smile, 0,0,1);
      }
      if ((bone = root->findBone(lipsBotL)) != NULL) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(smile, 0,0,1);
      }
      if ((bone = root->findBone(lipsBotR)) != NULL) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(-smile, 0,0,1);
      }
    }
  }
  // == smile
  if ( moveSmile ) {
    if ((bone = root->findBone(lipsRoot)) != NULL) {
      Vect3D smileDelta(0, cos(angle/10.0) / 4.0, 0);
      float smile = 20 * cos(angle / 10.0);
      if ((bone = root->findBone(lipsTopL)) != NULL) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(smile, 0,0,1);
      }
      if ((bone = root->findBone(lipsTopR)) != NULL) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(-smile, 0,0,1);
      }
    }
  }
  // == sulk
  if ( moveSulk ) {
    if ((bone = root->findBone(lipsRoot)) != NULL) {
      Vect3D smileDelta(0, cos(angle/10.0) / 4.0, 0);
      float smile = 20 * cos(angle / 10.0);
      if ((bone = root->findBone(lipsBotL)) != NULL) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(smile, 0,0,1);
      }
      if ((bone = root->findBone(lipsBotR)) != NULL) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(-smile, 0,0,1);
      }
    }
  }

  // --- Left eye management ---
  // == eye glance
  if ( moveEyeL ) {
    if ((bone = root->findBone(eyeLeftRoot)) != NULL) {
      float eyeLeftScale = (1 - cos(angle / 20.0)) / 2.0;
      if ((bone = root->findBone(eyeLeftBotRoot)) != NULL) {
        bone->resetPos();
        bone->setScale(1, eyeLeftScale, 1);
        bone->setRot(-(1-eyeLeftScale)*20.0, 1,0,0);
      }
      if ((bone = root->findBone(eyeLeftTopRoot)) != NULL) {
        bone->resetPos();
        bone->setScale(1, eyeLeftScale, 1);
        bone->setRot((1-eyeLeftScale)*20.0, 1,0,0);
      }
      if ((bone = root->findBone(eyeLeftTopL)) != NULL) {
        bone->resetPos();
        bone->setScale(1, eyeLeftScale, 1);
      }
      if ((bone = root->findBone(eyeLeftTopR)) != NULL) {
        bone->resetPos();
        bone->setScale(1, eyeLeftScale, 1);
      }
      if ((bone = root->findBone(eyeLeftBotL)) != NULL) {
        bone->resetPos();
        bone->setScale(1, eyeLeftScale, 1);
      }
      if ((bone = root->findBone(eyeLeftBotR)) != NULL) {
        bone->resetPos();
        bone->setScale(1, eyeLeftScale, 1);
      }
    }
  }

  // --- Right eye management ---
  // eye move
  if ( moveEyeR ) {
    if ((bone = root->findBone(eyeRightRoot)) != NULL) {
      float eyeRightScale = (1 + cos(angle / 5.0)) / 20.0;
      if ((bone = root->findBone(browRightRoot)) != NULL) {
        bone->resetPos();
        bone->setTrans(0, eyeRightScale / 2.0, 0);
      }
      if ((bone = root->findBone(eyeRightBotRoot)) != NULL) {
        bone->resetPos();
        bone->setTrans(0, eyeRightScale / 2.0, 0);
      }
      if ((bone = root->findBone(eyeRightTopRoot)) != NULL) {
        bone->resetPos();
        bone->setTrans(0, eyeRightScale, 0);
      }
      if ((bone = root->findBone(eyeRightTopL)) != NULL) {
        bone->resetPos();
        bone->setTrans(0, eyeRightScale, 0);
      }
      if ((bone = root->findBone(eyeRightTopR)) != NULL) {
        bone->resetPos();
        bone->setTrans(0, eyeRightScale, 0);
      }
      if ((bone = root->findBone(eyeRightBotL)) != NULL) {
        bone->resetPos();
        bone->setTrans(0, eyeRightScale / 2.0, 0);
      }
      if ((bone = root->findBone(eyeRightBotR)) != NULL) {
        bone->resetPos();
        bone->setTrans(0, eyeRightScale / 2.0, 0);
      }
    }
  }

  // --- Nose management ---
  // == resserement narines
  if ( moveNose ) {
    if ((bone = root->findBone(noseRoot)) != NULL) {
      float noseScale = 1 - cos(angle / 16.0) / 4.0;
      if ((bone = root->findBone(noseLeft)) != NULL) {
        bone->resetPos();
        bone->setScale(noseScale, 1, 1);
      }
      if ((bone = root->findBone(noseRight)) != NULL) {
        bone->resetPos();
        bone->setScale(noseScale, 1, 1);
      }
    }
  }

  // --- Root management ---
  if ( moveYes ) {
    if ((bone = root->findBone(headRoot)) != NULL)
      bone->setRot(10 * sin(angle/50.0), 1, 0, 0);
  }
  if ( moveNo ) {
    if ((bone = root->findBone(headRoot)) != NULL)
      bone->setRot(10 * sin(angle/50.0), 0, 1, 0);
  }

#undef BROW_MOTION
#ifdef BROW_MOTION
  float browRightScale = cos(angle / 5.0);
  if ((bone = root->findBone(browRightRoot)) != NULL) {
    bone->resetPos();
    bone->setTrans(0, browRightScale / 25.0, 0);
  }
  if ((bone = root->findBone(browRightL)) != NULL) {
    bone->resetPos();
    bone->setRot(10 * browRightScale, 0,0,1);
    bone->setTrans(0, browRightScale / 25.0, 0);
  }
#endif
}

void V3d::changeMoveYes(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->v3d)
    o->body->v3d->moveYes = ! o->body->v3d->moveYes;
}

void V3d::changeMoveNo(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->v3d)
    o->body->v3d->moveNo = ! o->body->v3d->moveNo;
}

void V3d::changeMoveMouth(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->v3d)
    o->body->v3d->moveMouth = ! o->body->v3d->moveMouth;
}

void V3d::changeMoveSmile(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->v3d)
    o->body->v3d->moveSmile = ! o->body->v3d->moveSmile;
}

void V3d::changeMoveSulk(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->v3d)
    o->body->v3d->moveSulk = ! o->body->v3d->moveSulk;
}

void V3d::changeMoveEyeR(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->v3d)
    o->body->v3d->moveEyeR = ! o->body->v3d->moveEyeR;
}

void V3d::changeMoveEyeL(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->v3d)
    o->body->v3d->moveEyeL = ! o->body->v3d->moveEyeL;
}

void V3d::changeMoveNose(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->v3d)
    o->body->v3d->moveNose = ! o->body->v3d->moveNose;
}

void V3d::changeFace(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->v3d)
    o->body->v3d->change();
}

//---------------------------------------------------------------------------

//-- V3D internal format parser
void V3d::readV3Dfile(BoneMesh *result, BoneVertex *skel, char *filename, float scale)
{
  File *file = new File();
  FILE *fp = file->open(filename, "rb");
  if (fp == NULL) return;

  trace(DBG_MAN, "readV3Dfile: reading mesh and skeleton from V3D file %s", filename);

  // Reading name
  char name[512];
  readStr(fp, name);
  result->setName(name);

  // Reading vertices
  int vertices = readInt(fp);
  for (int i=0; i < vertices; i++) {
    float x = readFloat(fp) * scale;
    float y = readFloat(fp) * scale;
    float z = readFloat(fp) * scale;
    result->addVertex(x, y, z);
  }
  trace(DBG_MAN, "v3d: Vertices added: %i", vertices);

  int facets = readInt(fp);
  for (int i=0; i < facets; i++) {
    // Reading indices
    int index1 = readInt(fp);
    int index2 = readInt(fp);
    int index3 = readInt(fp);
    result->addTriangle(index1, index2, index3);
    BoneTriangle *triangle = result->triangleList.getElemAt(i);
    // Reading color
    float r = readFloat(fp);
    float g = readFloat(fp);
    float b = readFloat(fp);
    float a = readFloat(fp);
    triangle->setColor(r, g, b, a);
    // Reading texture coordinantes
    triangle->u1 = readFloat(fp);
    triangle->v1 = readFloat(fp);
    triangle->u2 = readFloat(fp);
    triangle->v2 = readFloat(fp);
    triangle->u3 = readFloat(fp);
    triangle->v3 = readFloat(fp);
  }
  trace(DBG_MAN, "v3d: Faces added   : %i", facets);

  // Reading skeleton
  skel->readFromFile(fp, scale);
  result->rebuildNormals();
  file->close();
  delete file;
  trace(DBG_MAN, "skeleton added");
}


#if 0 //notused --------------------------------------------------------------------

void V3d::writeV3Dfile(BoneMesh *outMesh, BoneVertex *skeletonRoot, char *filename)
{
  File *file = new File();
  FILE *fp = file->open(filename, "wb");
  if (fp == NULL) return;

  if (! outMesh->vertexListCompiled) outMesh->compileVertexList();
  if (! outMesh->triangleListCompiled) outMesh->compileTriangleList();

  int i, index1, index2, index3;

  trace(DBG_FORCE, "writeV3Dfile: writing mesh and skeleton to V3D file");

  // Writing name
  writeString(fp, outMesh->getName());

  // Writing vertices coordinates
  writeInt(fp, outMesh->vertices);
  for (i=0; i < outMesh->vertices; i++) {
    writeFloat(fp, outMesh->vertex[i]->iniPosition.x);
    writeFloat(fp, outMesh->vertex[i]->iniPosition.y);
    writeFloat(fp, outMesh->vertex[i]->iniPosition.z);
  }
  trace(DBG_FORCE, "           Vertices added: %i", outMesh->vertices);

  // Writing triangles
  writeInt(fp, outMesh->triangles);
  for (i=0; i < outMesh->triangles; i++) {
    index1 = 0;
    index2 = 0;
    index3 = 0;
    while (outMesh->vertex[index1] != outMesh->triangle[i]->vertex1) index1++;
    while (outMesh->vertex[index2] != outMesh->triangle[i]->vertex2) index2++;
    while (outMesh->vertex[index3] != outMesh->triangle[i]->vertex3) index3++;
    // Writing indices
    writeInt(fp, index1);
    writeInt(fp, index2);
    writeInt(fp, index3);
    // Writing triangle color
    writeFloat(fp, outMesh->triangle[i]->r);
    writeFloat(fp, outMesh->triangle[i]->g);
    writeFloat(fp, outMesh->triangle[i]->b);
    writeFloat(fp, outMesh->triangle[i]->a);
    // Writing triangle texture coordinates
    writeFloat(fp, outMesh->triangle[i]->u1);
    writeFloat(fp, outMesh->triangle[i]->v1);
    writeFloat(fp, outMesh->triangle[i]->u2);
    writeFloat(fp, outMesh->triangle[i]->v2);
    writeFloat(fp, outMesh->triangle[i]->u3);
    writeFloat(fp, outMesh->triangle[i]->v3);
  }
  trace(DBG_FORCE, "           Faces added   : %i", outMesh->triangles);

  // Writing skeleton
  skeletonRoot->writeToFile(fp);
  trace(DBG_FORCE, "           Skeleton added!");
  file->close();
  delete file;
}

void V3d::readVRMLfile(BoneMesh *result, char *filename, float size, float centerx, float centery, float centerz, int colorMask)
{
  trace(DBG_MAN, "Creating TRI mesh with vrml file");
  trace(DBG_MAN, "           Filename      : [%s]", filename);

  char endLine = '\0';
  char skippedText[1024];
  float x,y,z;
  int index1, index2, index3, indexEnd;
  int vertices=0, facets=0;

  // First look if the file exists
  File *file = new File();
  FILE *fp = file->open(filename, "rt");
  if (fp == NULL) return;
  file->close();
  delete file;

  // Now that we know it exists, we'll look after the vertices part
  File *file2 = new File();
  fp = file2->open(filename, "rt");
  strcpy(skippedText, "");
  while ((strcmp(skippedText, "point") != 0) && (!feof(fp)))
    fscanf(fp, "%s", skippedText);
  if (!feof(fp)) {
    fscanf(fp, "%s\n", skippedText);
    endLine = '\0';
    while (endLine != ']') {
      fscanf(fp, "%f %f %f%c", &x, &y, &z, &endLine);
      result->addVertex(centerx + x * size, centery + y * size, centerz + z * size);
      vertices ++;
    }
    trace(DBG_MAN, "           Vertices added: %i", vertices);
  }
  file2->close();
  delete file2;

  // We'll look after the facets part
  File *file3 = new File();
  fp = file3->open(filename, "rt");
  strcpy(skippedText, "");
  while ((strcmp(skippedText, "coordIndex") != 0) && (!feof(fp)))
    fscanf(fp, "%s", skippedText);
  if (!feof(fp)) {
    fscanf(fp, "%s", skippedText);
    endLine = '\0';
    while (endLine != ']') {
      fscanf(fp, "%d, %d, %d, %d%c", &index1, &index2, &index3, &indexEnd, &endLine);
      result->addTriangle(index1, index2, index3);
      facets ++;
    }
    trace(DBG_MAN, "           Faces added   : %i", facets);
  }
  file3->close();
  delete file3;

  // We'll now go on color part
  File *file4 = new File();
  fp = file4->open(filename, "rt");
  strcpy(skippedText, "");
  while ((strcmp(skippedText, "color") != 0) && (!feof(fp)))
    fscanf(fp, "%s", skippedText);
  if (!feof(fp))
    fscanf(fp, "%s", skippedText);
  if (!feof(fp) && strcmp(skippedText, "NULL")) {
    BoneTriangle *triangle;
    float r,g,b;
    int cpt=0;

    printf("[%s]\n", skippedText);
    fscanf(fp, "%s", skippedText);
    fscanf(fp, "%s", skippedText);
    fscanf(fp, "%s", skippedText);

    endLine = '\0';
    while (endLine != ']') {
      fscanf(fp, "%f %f %f%c", &r, &g, &b, &endLine);
      triangle = result->triangleList.getElemAt(cpt++);

      if ((r==g) && (g==b) && (colorMask != 0)) {
        r = (triangle->vertex1->iniPosition.x + 1) / 2;
        g = (triangle->vertex1->iniPosition.y + 1) / 2;
        b = (triangle->vertex1->iniPosition.z + 1) / 2;
      }
      triangle->setColor(r,g,b,1);
    }
  }
  file4->close();
  delete file4;

  // And finaly come on texture mapping coordinates
  File *file5 = new File();
  fp = file5->open(filename, "rt");
  strcpy(skippedText, "");
  while ((strcmp(skippedText, "texCoord") != 0) && (!feof(fp)))
    fscanf(fp, "%s", skippedText);
  if (!feof(fp)) {
    Vertex * vertex;
    float u,v;
    int cpt=0;

    fscanf(fp, "%s", skippedText);
    fscanf(fp, "%s", skippedText);
    fscanf(fp, "%s", skippedText);
    fscanf(fp, "%s", skippedText);

    endLine = '\0';
    while (endLine != ']') {
      fscanf(fp, "%f %f%c", &u, &v, &endLine);
      vertex = result->vertexList.getElemAt(cpt++);
      vertex->u = u;
      vertex->v = v;
    }
  }
  file5->close();
  delete file5;
  result->rebuildNormals();
  result->setName(filename);
}

#endif //notused --------------------------------------------------------------------
