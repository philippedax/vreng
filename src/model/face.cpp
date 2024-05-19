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
// face.cpp
//
// Face management
//
// Author   : This file has been written by Yann Renard
// Copyright: This file is totaly free and you may distribute
//            it to anyone you want, without modifying this
//            header. If you use it in a commercial project (?)
//            or in bigger project (!), I would be glad to know about it :) Please mail me...
//            be glad to know about it, please mail me  myself_yr@hotmail.com
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "face.hpp"
#include "v3d.hpp"
#include "humanoid.hpp"
#include "fap.hpp"
#include "body.hpp"	// body
#include "http.hpp"	// httpOpen
#include "cache.hpp"	// setCachePath, open
#include "bone.hpp"	// BoneMesh, BoneSkel


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

const float Face::SCALE = 0.075;	///< 3/400


Face::Face()
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
  pathfile[0] = '\0';
}

Face::Face(const char *urls)
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
  pathfile[0] = '\0';
  urlList.empty();
  Http::httpOpen(urls, facereader, this, 0);
  curl = rand() % urlList.count();
}

Face::~Face()
{
  if (mesh) delete mesh;
  if (root) delete root;
  mesh = NULL;
  root = NULL;
}

/** Downloads list of face url */
void Face::facereader(void *_face, Http *http)
{
  Face *face = static_cast<Face *>(_face);
  if (! http) {
    return;
  }

  char line[URL_LEN];

  Cache *cache = new Cache();
  FILE *f = cache->open(http->url, http);
  if (! f) {
    error("can't read %s", http->url);
    delete cache;
    return;
  }
  while (cache->nextLine(f, line)) {
    char *faceurl = strdup(line);
    //echo("facereader: add url=%s", faceurl);
    face->urlList.addElement(faceurl);
    free(faceurl);
  }
  cache->close();
  delete cache;
}

/** Changes Face */
void Face::change()
{
  if (! indexed) return;

  curl++;
  curl %= urlList.count();
  char *urlface = urlList.getElemAt(curl);
  echo("change: %d/%d urlface=%s", curl, urlList.count(), urlface);
  if (! isascii(urlface[0])) {
    error("change: BUG! urlface=%02x", urlface[0]);
    return;
  }
  echo("urlface: %s", urlface);
  load(urlface);
}

/** Loads V3D file */
void Face::load(const char *url)
{
  mesh = new BoneMesh();
  root = new BoneVertex();

  if (Cache::setCachePath(url, pathfile) == 0) {
    error("Face load: file=%s url=%s", pathfile, url);
    return;
  }
  V3d::readV3D(mesh, root, pathfile);

  boneanim.registerMesh(mesh);
  boneanim.registerSkel(root);
  boneanim.generateLinkList();
}

/** Renders the face */
void Face::render()
{
  if (! mesh) return;

  if (boneanim.meshToMove && boneanim.skeleton) {
    //dax boneanim.animate();
    boneanim.render();
  }
}

//
//  Y
//  |
//  .--> X
//  /
// Z
//
// 1,0,0 X yaw   (yes)
// 0,1,0 Y pitch (no)
// 0,0,1 Z roll  (maybe)

/** Anims head */
void Face::animHead(float a, int x, int y, int z)
{
  BoneVertex *bone;

  //echo("animHead: a=%.2f", a);
  if ((bone = root->findBone(headRoot)))
    bone->setRot(sin(a/50)*10 , x, y, z);
  else
    error("headRoot not found");
}

/** Anims nose */
void Face::animNose(float a, const char *_side)
{
  BoneVertex *bone;
  float scale = 1 - cos(a/16) / 4;

  //echo("animNose: a=%.2f scale=%.2f", a, scale);
  if ((bone = root->findBone(noseRoot))) {
    if ((bone = root->findBone(_side))) {
      bone->resetPos();
      bone->setScale(scale, 1, 1);
    }
  }
  else
    error("noseRoot not found");
}

/** Anims eyeball */
void Face::animEyeBall(float a, const char *_side, int dir)
{
  BoneVertex *bone;
  float scale = 1 - cos(a/16) /* / 2 */;

  //echo("animEyeBall: a=%.2f scale=%.2f dir=%d", a, scale, dir);
  if ((bone = root->findBone(_side))) {
    bone->resetPos();
    bone->setScale(scale, 1, 1);
    if (dir)
      bone->setRot((1-scale)*20, 0,1,0);	// pitch
    else
      bone->setRot((1-scale)*20, 1,0,0);	// yaw
    bone->resetPos();
  }
  else
    error("%s not found", _side);
}

/** Anims eye lid */
void Face::animEyeLid(float a, const char *root1, const char *lid, const char *left, const char *right)
{
  BoneVertex *bone;
  float scale = (1 - cos(a/10)) /* / 2 */;

  if ((bone = root->findBone(root1))) {
    if ((bone = root->findBone(lid))) {
      int sign;
      if (! strcmp(lid, eyeLeftTopRoot) || ! strcmp(lid, eyeRightTopRoot))
        sign = 1;
      else
        sign = -1;
      //echo("animEyeLid: a=%.2f scale=%.2f rot=%.2f", a, scale, sign*(1-scale)*20);
      bone->resetPos();
      bone->setScale(1, scale, 1);
      bone->setRot(sign*(1-scale)*20, 1,0,0);
    }
    else
      error("%s not found", lid);
    if ((bone = root->findBone(left))) {
      bone->resetPos();
      bone->setScale(1, scale, 1);
    }
    if ((bone = root->findBone(right))) {
      bone->resetPos();
      bone->setScale(1, scale, 1);
    }
  }
  else
    error("%s not found", root1);
}

/** Anims eye brow */
void Face::animEyeBrow(float a, const char *_root, const char *_side)
{
  BoneVertex *bone;
  float scale = cos(a/5);

  //echo("animEyeBrow: a=%.2f scale=%.2f", a, scale);
  if ((bone = root->findBone(_root))) {
    bone->resetPos();
    bone->setTrans(0, scale/25, 0);
  }
  else
    error("%s not found", _root);
  if ((bone = root->findBone(_side))) {
    bone->resetPos();
    bone->setRot(scale*10, 0,0,1);
    bone->setTrans(0, scale/25, 0);
  }
  else
    error("%s not found", _side);
}

/** Anims lip */
void Face::animLip(float a, const char *_side)
{
  BoneVertex *bone;

  if ((bone = root->findBone(lipsRoot))) {
    Vect3D delta(0, cos(a/10) / 4, 0);
    //dax float smile = cos(a/10)*20;

    //echo("animLip: a=%.2f smile=%.2f", a, smile);
    if ((bone = root->findBone(_side))) {
      bone->resetPos();
      bone->setTrans(delta);
      //dax bone->setRot(smile, 0,0,1);
      bone->setRot(a, 0,0,1);
    }
  }
  else
    error("lipsRoot not found");
}

/** Plays a fap */
void Face::play(int fapn, int a)
{
  echo("fap: %d %d", fapn, a);
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
    if (a) echo("SHIFT_JAW %s a=%d", e_not_implemented, a);
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

/** Plays animations */
void Face::play()
{
  static float a = 0;
  a += 5.;

  BoneVertex *bone;

  // --- Lips management ---
  // == smile then sulk
  if ( moveMouth ) {
    echo("moveMouse");
    if ((bone = root->findBone(lipsRoot))) {
      Vect3D smileDelta(0, cos(a/10) / 4, 0);
      float smile = 20*cos(a/10);
      if ((bone = root->findBone(lipsTopL))) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(smile, 0,0,1);
      }
      if ((bone = root->findBone(lipsTopR))) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(-smile, 0,0,1);
      }
      if ((bone = root->findBone(lipsBotL))) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(smile, 0,0,1);
      }
      if ((bone = root->findBone(lipsBotR))) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(-smile, 0,0,1);
      }
    }
  }
  // == smile
  if ( moveSmile ) {
    echo("moveSmile");
    if ((bone = root->findBone(lipsRoot))) {
      Vect3D smileDelta(0, cos(a/10) / 4, 0);
      float smile = 20*cos(a/10);
      if ((bone = root->findBone(lipsTopL))) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(smile, 0,0,1);
      }
      if ((bone = root->findBone(lipsTopR))) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(-smile, 0,0,1);
      }
    }
  }
  // == sulk
  if ( moveSulk ) {
    if ((bone = root->findBone(lipsRoot))) {
      Vect3D smileDelta(0, cos(a/10) / 4, 0);
      float smile = 20*cos(a/10);
      if ((bone = root->findBone(lipsBotL))) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(smile, 0,0,1);
      }
      if ((bone = root->findBone(lipsBotR))) {
        bone->resetPos();
        bone->setTrans(smileDelta);
        bone->setRot(-smile, 0,0,1);
      }
    }
  }

  // --- Left eye management ---
  // == eye glance
  if ( moveEyeL ) {
    echo("moveEyeL");
    if ((bone = root->findBone(eyeLeftRoot))) {
      float eyeLeftScale = (1 - cos(a/20)) / 2;
      if ((bone = root->findBone(eyeLeftBotRoot))) {
        bone->resetPos();
        bone->setScale(1, eyeLeftScale, 1);
        bone->setRot(-(1-eyeLeftScale)*20, 1,0,0);
      }
      if ((bone = root->findBone(eyeLeftTopRoot))) {
        bone->resetPos();
        bone->setScale(1, eyeLeftScale, 1);
        bone->setRot((1-eyeLeftScale)*20, 1,0,0);
      }
      if ((bone = root->findBone(eyeLeftTopL))) {
        bone->resetPos();
        bone->setScale(1, eyeLeftScale, 1);
      }
      if ((bone = root->findBone(eyeLeftTopR))) {
        bone->resetPos();
        bone->setScale(1, eyeLeftScale, 1);
      }
      if ((bone = root->findBone(eyeLeftBotL))) {
        bone->resetPos();
        bone->setScale(1, eyeLeftScale, 1);
      }
      if ((bone = root->findBone(eyeLeftBotR))) {
        bone->resetPos();
        bone->setScale(1, eyeLeftScale, 1);
      }
    }
  }

  // --- Right eye management ---
  // eye move
  if ( moveEyeR ) {
    echo("moveEyeR");
    if ((bone = root->findBone(eyeRightRoot))) {
      float eyeRightScale = (1 + cos(a/5)) / 20;
      if ((bone = root->findBone(browRightRoot))) {
        bone->resetPos();
        bone->setTrans(0, eyeRightScale/2, 0);
      }
      if ((bone = root->findBone(eyeRightBotRoot))) {
        bone->resetPos();
        bone->setTrans(0, eyeRightScale/2, 0);
      }
      if ((bone = root->findBone(eyeRightTopRoot))) {
        bone->resetPos();
        bone->setTrans(0, eyeRightScale, 0);
      }
      if ((bone = root->findBone(eyeRightTopL))) {
        bone->resetPos();
        bone->setTrans(0, eyeRightScale, 0);
      }
      if ((bone = root->findBone(eyeRightTopR))) {
        bone->resetPos();
        bone->setTrans(0, eyeRightScale, 0);
      }
      if ((bone = root->findBone(eyeRightBotL))) {
        bone->resetPos();
        bone->setTrans(0, eyeRightScale/2, 0);
      }
      if ((bone = root->findBone(eyeRightBotR))) {
        bone->resetPos();
        bone->setTrans(0, eyeRightScale/2, 0);
      }
    }
  }

  // --- Nose management ---
  // == resserement narines
  if ( moveNose ) {
    echo("moveNose");
    if ((bone = root->findBone(noseRoot))) {
      float noseScale = 1 - cos(a/16) / 4;
      if ((bone = root->findBone(noseLeft))) {
        bone->resetPos();
        bone->setScale(noseScale, 1, 1);
      }
      if ((bone = root->findBone(noseRight))) {
        bone->resetPos();
        bone->setScale(noseScale, 1, 1);
      }
    }
  }

  // --- Root management ---
  if ( moveYes ) {
    if ((bone = root->findBone(headRoot)))
      bone->setRot(10*sin(a/50), 0,1,0);
  }
  if ( moveNo ) {
    if ((bone = root->findBone(headRoot)))
      bone->setRot(10*sin(a/50), 1,0,0);
  }

#undef BROW_MOTION
#ifdef BROW_MOTION
  float browRightScale = cos(a/5);
  if ((bone = root->findBone(browRightRoot))) {
    bone->resetPos();
    bone->setTrans(0, browRightScale/25, 0);
  }
  if ((bone = root->findBone(browRightL))) {
    bone->resetPos();
    bone->setRot(10*browRightScale, 0,0,1);
    bone->setTrans(0, browRightScale/25, 0);
  }
#endif
}

void Face::Yes(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->face)
    o->body->face->moveYes = ! o->body->face->moveYes;
}

void Face::No(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->face)
    o->body->face->moveNo = ! o->body->face->moveNo;
}

void Face::Mouth(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->face)
    o->body->face->moveMouth = ! o->body->face->moveMouth;
}

void Face::Smile(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->face)
    o->body->face->moveSmile = ! o->body->face->moveSmile;
}

void Face::Sulk(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->face)
    o->body->face->moveSulk = ! o->body->face->moveSulk;
}

void Face::EyeR(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->face)
    o->body->face->moveEyeR = ! o->body->face->moveEyeR;
}

void Face::EyeL(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->face)
    o->body->face->moveEyeL = ! o->body->face->moveEyeL;
}

void Face::Nose(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->face)
    o->body->face->moveNose = ! o->body->face->moveNose;
}

void Face::changeFace(Humanoid *o, void *d, time_t s, time_t u)
{
  if (o->body->face)
    o->body->face->change();
}
