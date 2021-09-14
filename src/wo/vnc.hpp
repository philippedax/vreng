//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
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
/*******************************************************/
/*                      VNC Object                     */
/*                                                     */
/* Thanks to that object, you can launch a VNC viewer  */
/* into a VREng world. So, you must have a VNC server  */
/* running on a machine (locally or remotely).         */
/*                                                     */
/* The syntax to add an object into a vre file is :    */
/*    <vnc name="..." pos="..." solid="..."            */
/*         server="..." port="..." passwd="..." />     */
/*  where                                              */
/*    pos <- "x,y,z,az,ax"                             */
/*    server <- VNC server                             */
/*    solid <- cf VReng specs (e.g. bbox,size=x,y,z)   */
/*                                                     */
/* If a server was entered in the vre file, the        */
/* server is connected...                              */
/*-----------------------------------------------------*/
#ifndef VNC_HPP
#define VNC_HPP

#define VNC_TYPE	35
#define VNC_NAME	"Vnc"

#include "wobject.hpp"
#include "vnccli.hpp"
#include "vncauth.hpp"
#include "gui.hpp"	// needed for X11


/**
 * Vnc class
 */
class Vnc : public WObject {

 private:
  static const uint16_t DEF_PORT;

  VNCClientTextured *vncClient;	///< vnc client
  GLuint texture;		///< texture number
  uint16_t tex_width;		///< texture width
  uint16_t tex_height;		///< texture height
  GLfloat u, v;			///< witdh and height screen ratios
  GLubyte *def_pixmap;		///< default texture pixmap
  GLubyte *tex_pixmap;		///< current pixmap
  GLfloat vertices[4*3];	///< screen geometry
  GLint viewport[4];		///< current viewport
  GLdouble modelmat[16];	///< modelisation matrix
  GLdouble projmat[16];		///< projection data coordinates
  bool serverdefined;		///< flag server defined or not
  bool connected;		///< flag connected or not
  bool focus;			///< flag have focus or not
  uint16_t port;		///< Vnc server port
  char servername[64];		///< Vnc server name
  char passwdfile[64];		///< server connection data

 public:
  static const OClass oclass;   ///< class variable

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();		///< init funclist

  Vnc(char *l);
  ///< Constructor

  static WObject * (creator)(char *l);
  /**< Creates from fileline */

  virtual void changePermanent(float lasting);
  /**< Called each frame - check if ths texture has to be updated */

  virtual void render();
  /**< Render the object */

  virtual void quit();
  /**< Called when the object is destroy */

  virtual bool mouseEvent(int16_t x, int16_t y, uint8_t button);
  /**< Redirect mouse  events to VNC Server */

  virtual bool keyEvent(const char *key, bool is_down);
  /**< Redirect keyboard  events to VNC Server */

  virtual void convert(const char *server, const char *port, const char *passwd);
  /**< updates server parameters from UStr from the dialog window */

 private:
  virtual void parser(char *l);
  /**< Parses */

  virtual void defaults();
  /**< Set the defaults */

  virtual void getVncCoords(int16_t &x, int16_t &y);
  /**< Convert VReng screen coordinates to VNC screen coordinates */

  virtual void setTexture(bool mipmap);
  /**< Build a texture */

  virtual void defaultPixmap();
  /**< Build the default pixmap */

  virtual void displayScreen();
  /**< Draw the screen */

  virtual void buildScreen();
  /**< Build the screen */

  virtual void connectServer();

  // GUI callbacks
  static void takeFocus(Vnc *o, void *d, time_t s, time_t u);
  static void leaveFocus(Vnc *o, void *d, time_t s, time_t u);
  static void disconnectServer(Vnc *o, void *d, time_t s, time_t u);
  static void reconnectServer(Vnc *o, void *d, time_t s, time_t u);

};

#endif
