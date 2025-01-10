//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// vnc.cpp
//
// Virtual Network Computing object
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "vnc.hpp"
#include "matvec.hpp"   // V3 M4
#include "move.hpp"	// gotoFront
#include "glu.hpp"	// glu alternative
#include "gui.hpp"	// setToVnc launchVnc
#include "env.hpp"	// passwd


const OClass Vnc::oclass(VNC_TYPE, "Vnc", Vnc::creator);

const uint16_t Vnc::VNC_PORT = DEF_VNC_PORT;


/** Creation from a file */
WO * Vnc::creator(char *l)
{
  return new Vnc(l);
}

/** Sets default values */
void Vnc::defaults()
{
  serverdefined = false;
  connected = false;
  focus = false;
  tex_pixmap = NULL;
  port = VNC_PORT;

  strcpy(servername, "localhost");
  memset(passwdfile, 0, sizeof(passwdfile));

  // screen aspect
  tex_width = 4;
  tex_height = 3;
  glGenTextures(1, &texture);	// texture number given by OpenGL

  // set default pixmap
  def_pixmap = new GLubyte[3 * tex_width * tex_height];

  GLubyte *pix = def_pixmap;
  for (int i=0; i < tex_width; i++) {
    for (int j=0; j < tex_height; j++) {
      *pix++ = 0;
      *pix++ = 127;	// cyan
      *pix++ = 127;
    }
  }
  displayTexture(0);		// without mipmaps
  u = v = 1.;
}

/** Parses the vre lines */
void Vnc::parser(char *l)
{
  char passwd[20];

  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (! stringcmp(l, "server")) {
      l = parseString(l, servername, "server");
      serverdefined = true;
    }
    else if (! stringcmp(l, "port")) {
      l = parseUInt16(l, &port, "port");
      if (port == 0) port = VNC_PORT;
    }
    else if (! stringcmp(l, "passwd")) {
      l = parseString(l, passwd, "passwd");
      passwd[8] = '\0';
    }
  }
  end_while_parse(l);

  sprintf(passwdfile, "%s-%s", ::g.env.passwd(), servername);
  vncEncryptAndStorePasswd(passwd, passwdfile);
}

/** Sets behaviors */
void Vnc::behaviors()
{
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);
}

/** Do sprecific initializations */
void Vnc::inits()
{
  initMobileObject(0);
  updatePosition();
  buildScreen();
  connectServer();
}

/** Constructor */
Vnc::Vnc(char *l)
{
  parser(l);
  behaviors();
  inits();
}

/** Builds the screen */
void Vnc::buildScreen()
{
  V3 dim;
  getDim(dim);

  float left  = MAX(dim.v[1], dim.v[0]);
  float front = MIN(dim.v[0], dim.v[1]) + 0.02;
  float top   = dim.v[2];
  float bot   = -top;
  float height;
  float width;
  float rsol = left / top;
  float rtex = (u * tex_width) / (v * tex_height);

  if (rtex > rsol) {
    height = bot + (2*0.95) * left/rtex;
    width = 0.95 * left;
  }
  else {
    height = (2*0.95-1) * top;
    width = 0.95 * top * rtex;
  }

  vertices[ 0] = front;  vertices[ 1] = width;  vertices[ 2] = bot;
  vertices[ 3] = front;  vertices[ 4] = width;  vertices[ 5] = height;
  vertices[ 6] = front;  vertices[ 7] = -width; vertices[ 8] = height;
  vertices[ 9] = front;  vertices[10] = -width; vertices[11] = bot;
}

/** Displays texture */
void Vnc::displayTexture(bool mipmap)
{
  glEnable(GL_TEXTURE_2D);	// we need to use a texture
  glBindTexture(GL_TEXTURE_2D, texture);	// we use ours

  //echo("tex: w=%d h=%d %d",tex_width,tex_height,mipmap);
  // put it into the video memory
  if (mipmap) {
#if HAVE_GLU
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tex_width, tex_height,
                      GL_RGB, GL_UNSIGNED_BYTE, tex_pixmap);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, 3, tex_width, tex_height, 0,
                      GL_RGB, GL_UNSIGNED_BYTE, tex_pixmap);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  else {
    glTexImage2D(GL_TEXTURE_2D, 0, 3, tex_width, tex_height, 0,
                      GL_RGB, GL_UNSIGNED_BYTE, tex_pixmap);
    // no interpolation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glDisable(GL_TEXTURE_2D);
}

/** Makes video frames changes */
void Vnc::changePermanent(float lasting)
{
  if (connected && vncClient) {
    fd_set rmask;
    struct timeval delay;
    int rfbsock = vncClient->getSock();

    delay.tv_sec =  0;
    delay.tv_usec = 1;
    FD_ZERO(&rmask);
    FD_SET(rfbsock, &rmask);

    if (select(32, &rmask, NULL, NULL, &delay)) {
      if (FD_ISSET(rfbsock, &rmask)) {
        if (! vncClient->handleRFBMessage()) {
          error("can't handle RFB server message");
          return;
        }
        //echo("frame");
        displayTexture(1);	// with mipmaps
      }
    }
  }
}

/** Renders */
void Vnc::render()
{
  if (! vncClient) return;

  GLint renderMode;
  glGetIntegerv(GL_RENDER_MODE, &renderMode);

  updatePosition();

  glPushMatrix();
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  glDisable(GL_LIGHTING);

  M4 posmat;
  getPosition(posmat);
  GLfloat gl_mat[16];
  M4toV16(&posmat, gl_mat);
  glMultMatrixf(gl_mat);

  glRotatef(RAD2DEG(pos.az), 0, 0, 1);

  // displays the screen
  glBegin(GL_QUADS);
   glTexCoord2f(u, v); glVertex3fv(vertices+0);	// bot, right
   glTexCoord2f(u, 0); glVertex3fv(vertices+3);	// top, right
   glTexCoord2f(0, 0); glVertex3fv(vertices+6);	// top, left
   glTexCoord2f(0, v); glVertex3fv(vertices+9);	// bot, left
  glEnd();

  // update parameters for mouse handling
  if (renderMode == GL_RENDER) {
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelmat);
    glGetDoublev(GL_PROJECTION_MATRIX, projmat);
  }

  glEnable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

/** Gets vnc coords */
void Vnc::getVncCoords(int16_t &x, int16_t &y)
{
  y = viewport[3] - y;

  // get intersection with z=0 plan
  GLdouble ox, oy, oz;
  gluUnProject((GLdouble) x, (GLdouble) y, (GLdouble) 0,
               modelmat, projmat, viewport,
               &ox, &oy, &oz);
  // get intersection with z=1 plan
  GLdouble ax, ay, az;
  gluUnProject((GLdouble) x, (GLdouble) y, (GLdouble) 1,
               modelmat, projmat, viewport,
               &ax, &ay, &az);

  // calculate intersection coordinates in vnc screen plane
  GLdouble dx, dy, dz;
  dx = ax - ox;
  if (dx >= 0) {
    x = -1;
    y = -1;
    return;
  }
  dy = ay - oy;
  dz = az - oz;

  GLdouble t = (vertices[0] - ox) / dx;
  GLdouble ey = t*dy + oy;
  GLdouble ez = t*dz + oz;

  ey = (ey-vertices[10]) / (vertices[4]-vertices[10]);
  ez = (vertices[5]-ez)  / (vertices[5]-vertices[11]);

  // build final coordinates
  if ((ey > 0) && (ey < 1) && (ez > 0) && (ez < 1)) {
    x = int( (ey * vncClient->realScreenWidth) );
    y = int( (ez * vncClient->realScreenHeight) );
  }
  else {
    x = -1;
    y = -1;
  }
}

/** Connects to a Vnc server */
void Vnc::connectServer()
{
  if (! serverdefined) return;

  vncClient = new VNCCliTextured(servername, port, passwdfile);

  // client initialization
  if (! vncClient->initVNC()) {
    serverdefined = false;
    error("VNC connect failed to %s:%d", servername, port);
    delete vncClient;
    vncClient = NULL;
    return;
  }
  else {
    connected = true;

    vncClient->sendFramebufferUpdateRequest(0, 0,
                                            vncClient->realScreenWidth,
                                            vncClient->realScreenHeight,
                                            false);
    enablePermanentMovement();	// follows video frames
  }

  // texture initialization from the framebuffer
  tex_pixmap = (uint8_t *) (vncClient->framebuffer);	// reinterpret_cast
  tex_width = vncClient->fbWidth;
  tex_height = vncClient->fbHeight;
  //echo("tex: w=%d h=%d", tex_width, tex_height);

  displayTexture(0);		// without mipmaps

  u = static_cast<float>(vncClient->realScreenWidth / tex_width);
  v = static_cast<float>(vncClient->realScreenHeight / tex_height);
  //echo("real_width=%d tex_width=%d u=%.2f", vncClient->realScreenWidth, tex_width, u);
  //echo("real_height=%d tex_height=%d v=%.2f", vncClient->realScreenHeight, tex_height, v);
}

/** Disconnects from Vnc server */
void Vnc::disconnectServer(Vnc *vnc, void *d, time_t s, time_t u)
{
  if (vnc->connected) {
    if (! vnc->vncClient->closeVNC()) {
      echo("disconnectServer: failed");
    }
    else {
      vnc->connected = false;
      vnc->serverdefined = false;
      ::g.gui.setToVnc(NULL);
    }
  }
}

/** Opens the dialog window */
void Vnc::reconnectServer(Vnc *vnc, void *d, time_t s, time_t u)
{
  if (vnc->connected || vnc->serverdefined)
    echo("VNC: already connected, disconnect first!");
  else
    ::g.gui.launchVnc(vnc);
}

/** Updates server parameters from the dialog window : called from gui */
void Vnc::convert(const char *srvstr, const char *portstr, const char *passstr)
{
  if (!srvstr || !portstr || !passstr) {
    return;
  }
  strcpy(servername, srvstr);
  port = atoi(portstr);
  vncEncryptAndStorePasswd(const_cast<char *>(passstr), passwdfile);
  connectServer();
}

/** Redirects mouse events to the Vnc object */
bool Vnc::mouseEvent(int16_t x, int16_t y, uint8_t button)
{
  if (! connected) return false;

  uint32_t card = 4;
  const char *params[card];
  char *p1 = new char[16];
  char *p2 = new char[16];
  char *p3 = new char[16];

  getVncCoords(x, y);  // change coords

  sprintf(p1, "%d", x);
  sprintf(p2, "%d", y);
  sprintf(p3, "%d", button);
  params[0] = "ptr";
  params[1] = p1;
  params[2] = p2;
  params[3] = p3;

  vncClient->sendRFBEvent((char **)params, &card); // send ptr

  delete[] p1;
  delete[] p2;
  delete[] p3;
  return true;
}

/** Redirects key events to the Vnc object */
bool Vnc::keyEvent(const char *key, bool is_pressed)
{
  if (! connected) return false;

  uint32_t card = 2;
  const char *params[card];
  char *pk = strdup(key);

  params[0] = (is_pressed) ? "keydown" : "keyup";
  params[1] = pk;

  vncClient->sendRFBEvent((char **)params, &card); // send key

  free(pk);
  return true;
}

/** Quits */
void Vnc::quit()
{
  if (def_pixmap) delete[] def_pixmap;

  if (connected) {
    leaveFocus(this, NULL, 0L, 0L);
    connected = false;
    if (vncClient) {
      vncClient->closeVNC();
      delete vncClient;
      vncClient = NULL;
    }
  }
}

/** Gives the focus to this object: all events will be redirected */
void Vnc::takeFocus(Vnc *vnc, void *d, time_t s, time_t u)
{
  if (! vnc->focus) {
    echo("Take Focus");
    ::g.gui.setToVnc(vnc);
    vnc->focus = true;
  }
}

/** Leaves the focus from the object */
void Vnc::leaveFocus(Vnc *vnc, void *d, time_t s, time_t u)
{
  if (vnc->focus) {
    echo("Leave Focus");
    ::g.gui.setToVnc(NULL);
    vnc->focus = false;
  }
}

/** Actions functions */
void Vnc::funcs()
{
  setActionFunc(VNC_TYPE, 0, _Action takeFocus,  "Focus");
  setActionFunc(VNC_TYPE, 1, _Action leaveFocus, "Navigate");
  setActionFunc(VNC_TYPE, 2, _Action disconnectServer, "Disconnect");
  setActionFunc(VNC_TYPE, 3, _Action reconnectServer, "Reconnect");
  setActionFunc(VNC_TYPE, 4, _Action gotoFront, "Approach");
}
