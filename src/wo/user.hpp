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
#ifndef USER_HPP
#define USER_HPP

#include "wobject.hpp"
#include "useraction.hpp"
#include "move.hpp"	// MAXKEYS

#define USER_TYPE	1
#define USER_NAME	"Avatar"

class Bullet;
class Dart;
class Carrier;
class Cart;
class Bubble;

extern class User *localuser;	// global

/**
 * User class
 */
class User: public WObject, public UserAction {
public:
  static const float LASTING;
  static const float DEFAULTWIDTH;
  static const float DEFAULTDEPTH;
  static const float DEFAULTHEIGHT;
  static const float LSPEED;
  static const float ASPEED;
  static const float DELTAZ;
  static const float GOTHROUGH;
  static const float FOVY;
  static const float NEAR;
  static const float FAR;
  static const float DENSITY;
  static const uint8_t MESS_LEN = 255;
  static const uint8_t MENSURATIONLEN = 64;

  class Man *man;		///< Man avatar model.
  class Guy *guy;		///< Guy avatar model.
  class Android *android;	///< Android avatar model.
  class Head *head;		///< Head avatar model.
  
protected:
  friend class WObject;
  float lspeed;		///< linear speed.
  float aspeed;		///< angular speed.
  float width;		///< body width.
  float depth;		///< body depth.

  uint32_t ssrc;	///< ssrc id.
  char  *front;		///< url body front.
  char  *back;		///< url body back.
  char  *left;		///< url body left.
  char  *right;		///< url body right.
  char  *vre;		///< url vre.
  char  *host;		///< url host.
  char  *model;		///< url model.
  char  *face;		///< url face.
  char  *sex;		///< sex.
  char  *headurl;	///< url head.
  char  *skin;		///< skin color.
  char  *bust;		///< bust color.
  char  *color;		///< color.
  char  *baps;		///< vaps server.

  WObject* lastSelected;
  float lastDistance;

  char  mensuration[MENSURATIONLEN];
  char  message[MESS_LEN];	///< message buffer.
  char  request[MESS_LEN];
  int   lastmess;	///< last message number.
  int   lastrequest;
  bool  goingup;	///< flag go up.

#if 0
  uint8_t weight;
  /* Adding aura (=sphere) of user object */
  float aura_center[3]; // center of sphere
  float aura_radius;    // sphere's radius
#endif

public:
  char  *web;		///< url web.
  char  *cname;		///< RTP cname.
  char  *rtcpname;	///< RTCP name.
  char  *email;		///< user's email.
  char  *tool;		///< tool used.
  float height;		///< body height.
  V3 ray;		///< ray extremity.
  uint8_t hit;		///< hit.
  Carrier *carrier;	///< Carrier.
  Cart *cart;		///< Cart.
  Bubble *bubble;	///< bubble text.
  uint8_t current_view;	///< current_view.

  bool   kpressed[MAXKEYS];	///< is the key pressed ?
  time_t kpstart_s[MAXKEYS];	///< key press starting time (sec).
  time_t kpstart_u[MAXKEYS];	///< key press starting time (usec).
  time_t kpdur_s[MAXKEYS];	///< key press duration from starting time sec.
  time_t kpdur_u[MAXKEYS];	///< key press duration from starting time usec.

  /* props */
  enum {
    PROPHNAME,
    PROPMAPFRONT,
    PROPMAPBACK,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAY,
    PROPAX,
    PROPMSG,
    PROPMENSURATION,
    PROPMAPLEFT,
    PROPMAPRIGHT,
    PROPVRE,
    PROPWEB,
    PROPSSRC,
    PROPRTCPNAME,
    PROPRTCPEMAIL,
    PROPMODEL,
    PROPFACE,
    PROPSEX,
    PROPHEAD,
    PROPSKIN,
    PROPBUST,
    PROPRAY,
    PROPS
  };
  enum {
    PROPBEGINVAR = PROPXY,
    PROPENDVAR = PROPMSG
  };

  static const OClass oclass;	///< class variable.

  static void funcs();		///< init funclist.

  virtual const OClass* getOClass() {return &oclass;};
  /**< Returns instance of this class. */

  User();
  /**< Constructor builtin. */

  User(uint8_t type_id, Noid noid, Payload *pp);
  /**< Constructor replicator. */

  virtual ~User();
  /**< Destructor. */

  virtual void inits();
  /**< Does needed initializations. */

  static void updateTime2(WObject *pu, time_t sec, time_t usec, float *lasting);
  /**< Updates times array. */

  static WObject * replicator(uint8_t type_id, Noid noid, Payload *pp);
  /**< Replicates a remote avatar. */

  virtual void resetBubble();
  /**< Resets bubble text. */

  virtual Bubble * getBubble() const;
  /**< Gets bubble text. */

  virtual void changePosition(float lasting);
  /**< Imposed movement. */

  virtual void changePermanent(float lasting);
  /**< Permanent movement. */

  virtual void updateTime(time_t sec, time_t usec, float *last);
  /**< Updates times. */

  virtual bool updateToNetwork(const Pos &oldpos);
  /**< Publishes position to the network. */

  virtual bool whenIntersect(WObject *pcur, WObject *pold);
  /**< When an other object intersects. */

  static void userWriting(const char *usermsg);
  /**< Signals a message sent by an user. */

  static void userRequesting(const char *usermsg);
  /**< Signals a message sent by an user. */

  virtual void clearKeyTab();
  /**< Clears keys times array. */

  virtual void updateKeysTimes(time_t sec, time_t usec);
  /**< Updates the keydifftime arrays. */

  virtual void changePositionOneDir(const int move_type, const float last);
  /**< Modifies user position in one direction. */

  virtual void updateTime(float lasting[]);
  /**< Fills delays's array for each user motion direction. */

  virtual void changePosition(const float lasting[]);
  /**< Does the motion in each direction. */

  virtual void elementaryUserMovement(const float lastings[]);
  /**< User motion limited by the maxlast. */

  virtual void userMovement(time_t sec, time_t usec);
  /**< User general motion. */

  virtual void setRayDirection(GLint x, GLint y);
  /**< Lanche a ray in thar direction. */

  virtual void specialAction(int act_id, void *data, time_t sec, time_t usec);
  /**< Special action to do. */

  virtual void enableGravity();
  /**< enables gravity. */

  virtual void disableGravity();
  /**< disables gravity. */

  virtual void resetPosition();
  /**< Resets user position. */

  virtual void setPosition();
  /**< Sets user position. */

  static void bulletPutHit(class Bullet *po, Payload *pp);
  static void dartPutHit(class Dart *po, Payload *pp);

  virtual bool hasHead();
  /**< Returns boolean. */


private:
  virtual void setView(uint8_t view);
  /**< User View managment. */

  virtual void setGravity(bool flag);
  /**< enables/disables gravity. */

  void setGoingup(bool flag);
  /**< sets flog goingup. */

  void getMemory();
  /**< Gets needed memory. */

  virtual void defaults();
  /**< Sets default values. */

  virtual void makeSolid();
  /**< User geometry. */

  virtual void setName();
  /**< Sets user name. */

  virtual void checkMysql();
  /**< Checks Mysql objects. */

  virtual void setCamera();
  /**< Sets user camera. */

  virtual void setRtcp();
  /**< Sets RTCP parameters. */

  virtual void addGui();
  /**< Adds user to GUI. */

  // GUI callbacks
  static void createBullet(User *pu, void *d, time_t s, time_t u);
  static void createDart(User *pu, void *d, time_t s, time_t u);
  static void defaultZoom(User *pu, void *d, time_t s, time_t u);
  static void increaseZoom(User *pu, void *d, time_t s, time_t u);
  static void decreaseZoom(User *pu, void *d, time_t s, time_t u);
  static void setZoom(User *pu, void *d, time_t s, time_t u);
  static void defaultLinearSpeed(User *pu, void *d, time_t s, time_t u);
  static void decreaseLinearSpeed(User *pu, void *d, time_t s, time_t u);
  static void increaseLinearSpeed(User *pu, void *d, time_t s, time_t u);
  static void setLspeed(User *pu, void *d, time_t s, time_t u);
  static void defaultAngularSpeed(User *pu, void *d, time_t s, time_t u);
  static void decreaseAngularSpeed(User *pu, void *d, time_t s, time_t u);
  static void increaseAngularSpeed(User *pu, void *d, time_t s, time_t u);
  static void setAspeed(User *pu, void *d, time_t s, time_t u);
  static void switchView(User *pu, void *d, time_t s, time_t u);
  static void papView(User *pu, void *d, time_t s, time_t u);
  static void firstPersonView(User *pu, void *d, time_t s, time_t u);
  static void thirdPersonViewFar(User *pu, void *d, time_t s, time_t u);
  static void thirdPersonView_RotL(User *pu, void *d, time_t s, time_t u);
  static void thirdPersonView_RotR(User *pu, void *d, time_t s, time_t u);
  static void thirdPersonView_RotU(User *pu, void *d, time_t s, time_t u);
  static void thirdPersonView_RotD(User *pu, void *d, time_t s, time_t u);
  static void thirdPersonView_Near(User *pu, void *d, time_t s, time_t u);
  static void thirdPersonView_Far(User *pu, void *d, time_t s, time_t u);
  static void pause(User *pu, void *d, time_t s, time_t u);
  static void pauseOn(User *pu, void *d, time_t s, time_t u);
  static void pauseOff(User *pu, void *d, time_t s, time_t u);
  static void mapView(User *pu, void *d, time_t s, time_t u);
  static void setRoll(User *pu, void *d, time_t s, time_t u);
  static void setPitch(User *pu, void *d, time_t s, time_t u);
  static void increasePitch(User *pu, void *d, time_t s, time_t u);
  static void decreasePitch(User *pu, void *d, time_t s, time_t u);
  static void increaseRoll(User *pu, void *d, time_t s, time_t u);
  static void decreaseRoll(User *pu, void *d, time_t s, time_t u);

  // network change callbacks
  static void get_username(User *pu, Payload *pp);
  static void u_get_xy(WObject *po, Payload *pp);
  static void u_get_z(WObject *po, Payload *pp);
  static void u_get_ax(WObject *po, Payload *pp);
  static void u_get_ay(WObject *po, Payload *pp);
  static void u_get_az(WObject *po, Payload *pp);
  static void get_msg(User *pu, Payload *pp);
  static void get_infos(User *pu, Payload *pp);
  static void get_mensuration(User *pu, Payload *pp);
  static void get_mapfront(User *pu, Payload *pp);
  static void get_mapback(User *pu, Payload *pp);
  static void get_mapleft(User *pu, Payload *pp);
  static void get_mapright(User *pu, Payload *pp);
  static void get_vre(User *pu, Payload *pp);
  static void get_web(User *pu, Payload *pp);
  static void get_model(User *pu, Payload *pp);
  static void get_face(User *pu, Payload *pp);
  static void get_sex(User *pu, Payload *pp);
  static void get_head(User *pu, Payload *pp);
  static void get_skin(User *pu, Payload *pp);
  static void get_bust(User *pu, Payload *pp);
  static void get_color(User *pu, Payload *pp);
  static void get_ssrc(User *pu, Payload *pp);
  static void get_rtcpname(User *pu, Payload *pp);
  static void get_rtcpemail(User *pu, Payload *pp);
  static void get_ray(User *pu, Payload *pp);
  static void put_username(User *pu, Payload *pp);
  static void put_msg(User *pu, Payload *pp);
  static void put_infos(User *pu, Payload *pp);
  static void put_mensuration(User *pu, Payload *pp);
  static void put_mapfront(User *pu, Payload *pp);
  static void put_mapback(User *pu, Payload *pp);
  static void put_mapleft(User *pu, Payload *pp);
  static void put_mapright(User *pu, Payload *pp);
  static void put_vre(User *pu, Payload *pp);
  static void put_web(User *pu, Payload *pp);
  static void put_model(User *pu, Payload *pp);
  static void put_face(User *pu, Payload *pp);
  static void put_sex(User *pu, Payload *pp);
  static void put_head(User *pu, Payload *pp);
  static void put_skin(User *pu, Payload *pp);
  static void put_bust(User *pu, Payload *pp);
  static void put_color(User *pu, Payload *pp);
  static void put_ssrc(User *pu, Payload *pp);
  static void put_rtcpname(User *pu, Payload *pp);
  static void put_rtcpemail(User *pu, Payload *pp);
  static void put_ray(User *pu, Payload *pp);
};


#endif
