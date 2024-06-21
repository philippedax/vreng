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

#include <algorithm>
#include <list>
#include <vector>


static const char head[]		= "root";
static const char lips[]		= "lipsRoot";
static const char eyeLeft[]		= "eyeLeftRoot";
static const char eyeRight[]		= "eyeRightRoot";
static const char eyeLeftTop[]		= "eyeLeftTopRoot";
static const char eyeRightTop[]		= "eyeRightTopRoot";
static const char eyeLeftBot[]		= "eyeLeftBotRoot";
static const char eyeRightBot[]		= "eyeRightBotRoot";
static const char browLeft[]		= "sourcilLeftRoot";
static const char browRight[]		= "sourcilRightRoot";
static const char nose[]		= "noseRoot";

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
  skel = NULL;
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
  skel = NULL;
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
  //dax urlList.clear();
  nburl = 0;
  Http::httpOpen(urls, facesreader, this, 0);
  nburl = urlList.count();
  //dax nburl = urlList.size();
  iurl = rand() % nburl;
  //echo("nburl: %d/%d", iurl, nburl);
  iurl = rand() % urlList.count();
  //dax for (vector<char*>::iterator it = urlList.begin(); it != urlList.end(); ++it) {
  //dax   echo("list: %s", *it);
  //dax }
}

Face::~Face()
{
  if (mesh) delete mesh;
  if (skel) delete skel;
  mesh = NULL;
  skel = NULL;
}

/** Downloads list of face url - static */
void Face::facesreader(void *_face, Http *http)
{
  Face *face = static_cast<Face *>(_face);
  if (! http) {
    return;
  }
  char url[URL_LEN];

  Cache *cache = new Cache();
  FILE *f = cache->open(http->url, http);
  if (! f) {
    error("can't read %s", http->url);
    delete cache;
    return;
  }
  while (cache->nextLine(f, url)) {
    //echo("facesreader: add %s", url);
    face->urlList.addElem(url);
    //dax face->urlList.push_back(url);
  }
  cache->close();
  delete cache;
}

/** Changes Face */
void Face::change()
{
  if (! indexed) return;

  //dax uint8_t idx = 0;

  iurl++;
  iurl %= nburl;
  iurl %= urlList.count();
  char *url = urlList.getElemAt(iurl);

  //dax char *url = new char[URL_LEN];
  //dax for (vector<char*>::iterator it = urlList.begin(); it != urlList.end(); ++it) {
  //dax   echo("list: %s", *it);
  //dax   if (idx == iurl) {
  //dax     strcpy(url, *it);
  //dax     echo("found: %s %d", *it, idx);
  //dax     break;
  //dax   }
  //dax   idx++;
  //dax }
  echo("change: %d/%d url=%s", iurl, nburl, url);
  if (! isascii(url[0])) {
    error("change: BUG! url=%02x %02x", url[0], url[1]);
    return;
  }
  //dax echo("url: %s", url);
  load(url);
  //dax delete [] url;
}

/** Loads V3D file */
void Face::load(const char *url)
{
  mesh = new BoneMesh();
  skel = new BoneVertex();

  if (Cache::setCachePath(url, pathfile) == 0) {
    error("Face load: file=%s url=%s", pathfile, url);
    return;
  }
  V3d::readV3D(mesh, skel, pathfile);

  bone = new Bone();

  bone->registerMesh(mesh);
  bone->registerSkel(skel);
  bone->genLinkList();
}

/** Renders the face */
void Face::render()
{
  if (! mesh) return;

  if (bone->mesh) {
    bone->render();
  }
}

//
//  Y
//  |
//  .--> X
//  /
// Z
//
// 1,0,0 X yaw   (no)
// 0,1,0 Y pitch (yes)
// 0,0,1 Z roll  (maybe)

/** Anims head */
void Face::animHead(float a, int x, int y, int z)
{
  BoneVertex *bv;

  echo("Head: a=%.0f", a);
  if ((bv = skel->getBone(head)))
    bv->setRot(sin(a/50)*10 , x, y, z);
  else
    error("head not found");
}

/** Anims nose */
void Face::animNose(float a, const char *b)
{
  BoneVertex *bv;
  float scale = 1 - cos(a/16) / 4;

  echo("Nose: a=%.0f s=%.1f", a, scale);
  if ((bv = skel->getBone(nose))) {
    if ((bv = skel->getBone(b))) {
      bv->resetPos();
      bv->setScale(scale, 1, 1);
    }
  }
  else
    error("nose not found");
}

/** Anims eyeball */
void Face::animEyeBall(float a, const char *b, int d)
{
  BoneVertex *bv;
  float scale = 1 - cos(a/16) /* / 2 */;

  echo("eyeball: a=%.0f s=%.1f d=%d", a, scale, d);
  if ((bv = skel->getBone(b))) {
    bv->resetPos();
    bv->setScale(scale, 1, 1);
    if (d)
      bv->setRot((1-scale)*20, 0,1,0);	// pitch
    else
      bv->setRot((1-scale)*20, 1,0,0);	// yaw
    bv->resetPos();
  }
  else
    error("%s not found", b);
}

/** Anims eye lid */
void Face::animEyeLid(float a, const char *root, const char *lid, const char *left, const char *right)
{
  BoneVertex *bv;
  float scale = (1 - cos(a/10)) /* / 2 */;

  if ((bv = skel->getBone(root))) {
    if ((bv = skel->getBone(lid))) {
      int sign;
      if (! strcmp(lid, eyeLeftTop) || ! strcmp(lid, eyeRightTop))
        sign = 1;
      else
        sign = -1;
      echo("eyelid: a=%.0f s=%.1f r=%.0f", a, scale, sign*(1-scale)*20);
      bv->resetPos();
      bv->setScale(1, scale, 1);
      bv->setRot(sign*(1-scale)*20, 1,0,0);
    }
    else
      error("%s not found", lid);
    if ((bv = skel->getBone(left))) {
      bv->resetPos();
      bv->setScale(1, scale, 1);
    }
    if ((bv = skel->getBone(right))) {
      bv->resetPos();
      bv->setScale(1, scale, 1);
    }
  }
  else
    error("%s not found", root);
}

/** Anims eye brow */
void Face::animEyeBrow(float a, const char *_root, const char *b)
{
  BoneVertex *bv;
  float scale = cos(a/5);

  echo("eyebrow: a=%.0f s=%.1f", a, scale);
  if ((bv = skel->getBone(_root))) {
    bv->resetPos();
    bv->setTrans(0, scale/25, 0);
  }
  else
    error("%s not found", _root);
  if ((bv = skel->getBone(b))) {
    bv->resetPos();
    bv->setRot(scale*10, 0,0,1);
    bv->setTrans(0, scale/25, 0);
  }
  else
    error("%s not found", b);
}

/** Anims lip */
void Face::animLip(float a, const char *b)
{
  BoneVertex *bv;

  if ((bv = skel->getBone(lips))) {
    Vect3D delta(0, cos(a/10) / 4, 0);
    //dax float smile = cos(a/10)*20;

    echo("lip: a=%.0f d=%.1f", a, cos(a/10/4));
    if ((bv = skel->getBone(b))) {
      bv->resetPos();
      bv->setTrans(delta);
      //dax bv->setRot(smile, 0,0,1);
      bv->setRot(a, 0,0,1);
    }
  }
  else
    error("lips not found");
}

/** Plays a fap */
void Face::play(int fap, float a)
{
  //echo("fap: %d %.0f", fap, a);

  switch (fap) {
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
    //if (a) echo("LOWER_T_MIDLIP %s a=%.0f", e_not_implemented, a);
    break;
  case RAISE_B_MIDLIP:		// 5
    //if (a) echo("RAISE_B_MIDLIP %s a=%.0f", e_not_implemented, a);
    break;
  case STRETCH_L_CORNERLIP:	// 6
    //if (a) echo("STRETCH_L_CORNERLIP %s a=%.0f", e_not_implemented, a);
    break;
  case STRETCH_R_CORNERLIP:	// 7
    //if (a) echo("STRETCH_R_CORNERLIP %s a=%.0f", e_not_implemented, a);
    break;
  case LOWER_T_LIP_LM:		// 8
    //if (a) echo("%d LOWER_T_LIP_LM a=%.0f", fap, a);
    animLip(a, lipsTopL);
    break;
  case LOWER_T_LIP_RM:		// 9
    //if (a) echo("%d LOWER_T_LIP_RM a=%.0f", fap, a);
    animLip(a, lipsTopR);
    break;
  case RAISE_B_LIP_LM:		// 10
    //if (a) echo("%d LOWER_B_LIP_LM a=%.0f", fap, a);
    animLip(a, lipsBotL);
    break;
  case RAISE_B_LIP_RM:		// 11
    //if (a) echo("%d LOWER_B_LIP_RM a=%.0f", fap, a);
    animLip(a, lipsBotR);
    break;
  case RAISE_L_CORNERLIP:	// 12
  case RAISE_R_CORNERLIP:	// 13
    //if (a) echo("RAISE_CORNERLIP %s a=%.0f", e_not_implemented, a);
    break;
  case THRUST_JAW:		// 14
    //if (a) echo("THRUST_JAW %s a=%.0f", e_not_implemented, a);
    break;
  case SHIFT_JAW:		// 15
    //if (a) echo("SHIFT_JAW %s a=%.0f", e_not_implemented, a);
    break;
  case PUSH_B_LIP:		// 16
    //if (a) echo("PUSH_B_LIP %s a=%.0f", e_not_implemented, a);
    break;
  case PUSH_T_LIP:		// 17
    //if (a) echo("PUSH_T_LIP %s a=%.0f", e_not_implemented, a);
    break;
  case DEPRESS_CHIN:		// 18 menton
    //if (a) echo("DEPRESS_CHIN %s a=%.0f", e_not_implemented, a);
    break;
  case CLOSE_T_L_EYELID:	// 19
    //if (a) echo("%d CLOSE_T_L_EYELID a=%.0f", fap, a);
    animEyeLid(a, eyeLeft, eyeLeftTop, eyeLeftTopL, eyeLeftTopR);
    break;
  case CLOSE_T_R_EYELID:	// 20
    //if (a) echo("%d CLOSE_T_R_EYELID a=%.0f", fap, a);
    animEyeLid(a, eyeRight, eyeRightTop, eyeRightTopL, eyeRightTopR);
    break;
  case CLOSE_B_L_EYELID:	// 21
    //if (a) echo("%d CLOSE_B_L_EYELID a=%.0f", fap, a);
    animEyeLid(a, eyeLeft, eyeLeftBot, eyeLeftBotL, eyeLeftBotR);
    break;
  case CLOSE_B_R_EYELID:	// 22
    //if (a) echo("%d CLOSE_B_R_EYELID a=%.0f", fap, a);
    animEyeLid(a, eyeRight, eyeRightBot, eyeRightBotL, eyeRightBotR);
    break;
  case YAW_L_EYEBALL:		// 23
    //if (a) echo("YAW_L_EYEBALL %s a=%.0f", e_not_implemented, a);
    //dax animEyeBall(a, eyeLeft, 0);
    break;
  case YAW_R_EYEBALL:		// 24
    //if (a) echo("YAW_R_EYEBALL %s a=%.0f", e_not_implemented, a);
    //dax animEyeBall(a, eyeRight, 0);
    break;
  case PITCH_L_EYEBALL:		// 25
    //if (a) echo("PITCH_L_EYEBALL %s a=%.0f", e_not_implemented, a);
    //dax animEyeBall(a, eyeLeft, 1);
    break;
  case PITCH_R_EYEBALL:		// 26
    //if (a) echo("PITCH_R_EYEBALL %s a=%.0f", e_not_implemented, a);
    //dax animEyeBall(a, eyeRight, 1);
    break;
  case THRUST_L_EYEBALL:	// 27
  case THRUST_R_EYEBALL:	// 28
    //if (a) echo("THRUST_EYEBALL %s a=%.0f", e_not_implemented, a);
    break;
  case DILATE_L_PUPIL:		// 29
  case DILATE_R_PUPIL:		// 30
    //if (a) echo("DILATE_PUPIL %s a=%.0f", e_not_implemented, a);
    break;
  case RAISE_L_I_EYEBROW:	// 31
    //if (a) echo("%d RAISE_L_I_EYEBROW a=%.0f", fap, a);
    animEyeBrow(a, browLeft, browRightL);
    break;
  case RAISE_R_I_EYEBROW:	// 32
    //if (a) echo("%d RAISE_R_I_EYEBROW a=%.0f", fap, a);
    animEyeBrow(a, browRight, browRightR);
    break;
  case RAISE_L_M_EYEBROW:	// 33
    //if (a) echo("%d RAISE_L_M_EYEBROW a=%.0f", fap, a);
    animEyeBrow(a, browLeft, browRightL);
    break;
  case RAISE_R_M_EYEBROW:	// 34
    //if (a) echo("%d RAISE_R_M_EYEBROW a=%.0f", fap, a);
    animEyeBrow(a, browRight, browRightR);
    break;
  case RAISE_L_O_EYEBROW:	// 35
    //if (a) echo("%d RAISE_L_O_EYEBROW a=%.0f", fap, a);
    animEyeBrow(a, browLeft, browRightL);
    break;
  case RAISE_R_O_EYEBROW:	// 36
    //if (a) echo("%d RAISE_R_O_EYEBROW a=%.0f", fap, a);
    animEyeBrow(a, browRight, browRightR);
    break;
  case SQUEEZE_L_EYEBROW:	// 37
    //if (a) echo("SQUEEZE_L_EYEBROW %s a=%.0f", e_not_implemented, a);
    break;
  case SQUEEZE_R_EYEBROW:	// 38
    //if (a) echo("SQUEEZE_R_EYEBROW %s a=%.0f", e_not_implemented, a);
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
    //if (a) echo("%d HEAD_PITCH a=%.0f", fap, a);
    animHead(a, 0, 1, 0);	// yes
    break;
  case HEAD_YAW:		// 49
    //if (a) echo("%d HEAD_YAW a=%.0f", fap, a);
    animHead(a, 1, 0, 0);	// no
    break;
  case HEAD_ROLL:		// 50
    //if (a) echo("%d HEAD_ROLL a=%.0f", fap, a);
    animHead(a, 0, 0, 1);	// maybe
    break;
  case LOWER_T_MIDLIP_O:	// 51
    //if (a) echo("LOWER_T_MIDLIP_O %s a=%.0f", e_not_implemented, a);
    break;
  case RAISE_B_MIDLIP_O:	// 52
    //if (a) echo("RAISE_B_MIDLIP_O %s a=%.0f", e_not_implemented, a);
    break;
  case STRETCH_L_CORNERLIP_O:	// 53
    //if (a) echo("STRETCH_L_CORNERLIP_O %s a=%.0f", e_not_implemented, a);
    break;
  case STRETCH_R_CORNERLIP_O:	// 54
    //if (a) echo("STRETCH_R_CORNERLIP_O %s a=%.0f", e_not_implemented, a);
    break;
  case LOWER_T_LIP_LM_O:	// 55
    //if (a) echo("%d LOWER_T_LIP_LM_O a=%.0f", fap, a);
    animLip(a, lipsTopL);
    break;
  case LOWER_T_LIP_RM_O:	// 56
    //if (a) echo("%d LOWER_T_LIP_RM_O a=%.0f", fap, a);
    animLip(a, lipsTopR);
    break;
  case RAISE_B_LIP_LM_O:	// 57
    //if (a) echo("%d RAISE_B_LIP_LM_O a=%.0f", fap, a);
    animLip(a, lipsBotL);
    break;
  case RAISE_B_LIP_RM_O:	// 58
    //if (a) echo("%d RAISE_B_LIP_RM_O a=%.0f", fap, a);
    animLip(a, lipsBotR);
    break;
  case RAISE_L_CORNERLIP_O:	// 59
    //if (a) echo("RAISE_L_CORNERLIP_O %s a=%.0f", e_not_implemented, a);
    break;
  case RAISE_R_CORNERLIP_O:	// 60
    //if (a) echo("RAISE_R_CORNERLIP_O %s a=%.0f", e_not_implemented, a);
    break;
  case STRETCH_L_NOSE:		// 61
    //if (a) echo("%d STRETCH_L_NOSE a=%.0f", fap, a);
    animNose(a, noseLeft);
    break;
  case STRETCH_R_NOSE:		// 62
    //if (a) echo("%d STRETCH_R_NOSE a=%.0f", fap, a);
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
    error("bad fap number=%d", fap);
  }
}

/** Plays animations */
void Face::play()
{
  static float a = 0;
  a += 5.;

  BoneVertex *bv;

  // --- Lips management ---
  // == smile then sulk
  if ( moveMouth ) {
    echo("moveMouse");
    if ((bv = skel->getBone(lips))) {
      Vect3D delta(0, cos(a/10) / 4, 0);
      float smile = 20*cos(a/10);
      if ((bv = skel->getBone(lipsTopL))) {
        bv->resetPos();
        bv->setTrans(delta);
        bv->setRot(smile, 0,0,1);
      }
      if ((bv = skel->getBone(lipsTopR))) {
        bv->resetPos();
        bv->setTrans(delta);
        bv->setRot(-smile, 0,0,1);
      }
      if ((bv = skel->getBone(lipsBotL))) {
        bv->resetPos();
        bv->setTrans(delta);
        bv->setRot(smile, 0,0,1);
      }
      if ((bv = skel->getBone(lipsBotR))) {
        bv->resetPos();
        bv->setTrans(delta);
        bv->setRot(-smile, 0,0,1);
      }
    }
  }
  // == smile
  if ( moveSmile ) {
    echo("moveSmile");
    if ((bv = skel->getBone(lips))) {
      Vect3D delta(0, cos(a/10) / 4, 0);
      float smile = 20*cos(a/10);
      if ((bv = skel->getBone(lipsTopL))) {
        bv->resetPos();
        bv->setTrans(delta);
        bv->setRot(smile, 0,0,1);
      }
      if ((bv = skel->getBone(lipsTopR))) {
        bv->resetPos();
        bv->setTrans(delta);
        bv->setRot(-smile, 0,0,1);
      }
    }
  }
  // == sulk
  if ( moveSulk ) {
    if ((bv = skel->getBone(lips))) {
      Vect3D delta(0, cos(a/10) / 4, 0);
      float smile = 20*cos(a/10);
      if ((bv = skel->getBone(lipsBotL))) {
        bv->resetPos();
        bv->setTrans(delta);
        bv->setRot(smile, 0,0,1);
      }
      if ((bv = skel->getBone(lipsBotR))) {
        bv->resetPos();
        bv->setTrans(delta);
        bv->setRot(-smile, 0,0,1);
      }
    }
  }

  // --- Left eye management ---
  // == eye glance
  if ( moveEyeL ) {
    echo("moveEyeL");
    if ((bv = skel->getBone(eyeLeft))) {
      float eyeLeftScale = (1 - cos(a/20)) / 2;
      if ((bv = skel->getBone(eyeLeftBot))) {
        bv->resetPos();
        bv->setScale(1, eyeLeftScale, 1);
        bv->setRot(-(1-eyeLeftScale)*20, 1,0,0);
      }
      if ((bv = skel->getBone(eyeLeftTop))) {
        bv->resetPos();
        bv->setScale(1, eyeLeftScale, 1);
        bv->setRot((1-eyeLeftScale)*20, 1,0,0);
      }
      if ((bv = skel->getBone(eyeLeftTopL))) {
        bv->resetPos();
        bv->setScale(1, eyeLeftScale, 1);
      }
      if ((bv = skel->getBone(eyeLeftTopR))) {
        bv->resetPos();
        bv->setScale(1, eyeLeftScale, 1);
      }
      if ((bv = skel->getBone(eyeLeftBotL))) {
        bv->resetPos();
        bv->setScale(1, eyeLeftScale, 1);
      }
      if ((bv = skel->getBone(eyeLeftBotR))) {
        bv->resetPos();
        bv->setScale(1, eyeLeftScale, 1);
      }
    }
  }

  // --- Right eye management ---
  // eye move
  if ( moveEyeR ) {
    echo("moveEyeR");
    if ((bv = skel->getBone(eyeRight))) {
      float eyeRightScale = (1 + cos(a/5)) / 20;
      if ((bv = skel->getBone(browRight))) {
        bv->resetPos();
        bv->setTrans(0, eyeRightScale/2, 0);
      }
      if ((bv = skel->getBone(eyeRightBot))) {
        bv->resetPos();
        bv->setTrans(0, eyeRightScale/2, 0);
      }
      if ((bv = skel->getBone(eyeRightTop))) {
        bv->resetPos();
        bv->setTrans(0, eyeRightScale, 0);
      }
      if ((bv = skel->getBone(eyeRightTopL))) {
        bv->resetPos();
        bv->setTrans(0, eyeRightScale, 0);
      }
      if ((bv = skel->getBone(eyeRightTopR))) {
        bv->resetPos();
        bv->setTrans(0, eyeRightScale, 0);
      }
      if ((bv = skel->getBone(eyeRightBotL))) {
        bv->resetPos();
        bv->setTrans(0, eyeRightScale/2, 0);
      }
      if ((bv = skel->getBone(eyeRightBotR))) {
        bv->resetPos();
        bv->setTrans(0, eyeRightScale/2, 0);
      }
    }
  }

  // --- Nose management ---
  // == resserement narines
  if ( moveNose ) {
    echo("moveNose");
    if ((bv = skel->getBone(nose))) {
      float noseScale = 1 - cos(a/16) / 4;
      if ((bv = skel->getBone(noseLeft))) {
        bv->resetPos();
        bv->setScale(noseScale, 1, 1);
      }
      if ((bv = skel->getBone(noseRight))) {
        bv->resetPos();
        bv->setScale(noseScale, 1, 1);
      }
    }
  }

  // --- Head management ---
  if ( moveYes ) {
    if ((bv = skel->getBone(head)))
      bv->setRot(10*sin(a/50), 0,1,0);
  }
  if ( moveNo ) {
    if ((bv = skel->getBone(head)))
      bv->setRot(10*sin(a/50), 1,0,0);
  }

#undef BROW_MOTION
#ifdef BROW_MOTION
  float browRightScale = cos(a/5);
  if ((bv = skel->getBone(browRight))) {
    bv->resetPos();
    bv->setTrans(0, browRightScale/25, 0);
  }
  if ((bv = skel->getBone(browRightL))) {
    bv->resetPos();
    bv->setRot(10*browRightScale, 0,0,1);
    bv->setTrans(0, browRightScale/25, 0);
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
