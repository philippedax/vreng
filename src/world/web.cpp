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
// web.cpp
//
// Web handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "web.hpp"
#include "matvec.hpp"   // V3 M4
#include "user.hpp"	// USER_TYPE
#include "text.hpp"	// Text
#include "color.hpp"	// Color
#include "browser.hpp"	// start


const OClass Web::oclass(WEB_TYPE, "Web", Web::creator);

const float Web::TTL = 4.;       	// 4 sec max
const float Web::ASPEED = M_PI_2;	// 90deg/sec

// local
static uint16_t oid = 0;

enum {
  RECTO,
  VERSO
};

/* creation from a file */
Object * Web::creator(char *l)
{
  return new Web(l);
}

void Web::defaults()
{
  *caption = '\0';
}

void Web::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "url"))
      l = parseUrl(l, name.url);
    else if (! stringcmp(l, "caption"))
      l = parseCaption(l, caption, "caption");
    else if (! stringcmp(l, "text"))
      l = parseCaption(l, caption, "text");
  }
  end_while_parse(l);
}

Web::Web(char *l)
{
  face = RECTO;	// recto face
  aspeed = ASPEED;
  text = NULL;

  parser(l);
  angori = pos.az;	// keep initial angle

  mobileObject(TTL);
  createNetObj(PROPS, ++oid);
}

void Web::timing(time_t sec, time_t usec, float *lasting)
{
  updateLasting(sec, usec, lasting);
}

void Web::imposed(float lasting)
{ 
  static float roting = 0;

  if (roting < M_PI) {
    pos.az += lasting * move.aspeed.v[0];
    roting += lasting * move.aspeed.v[0];
  }
  else {
    roting = 0;
    switch (face) {
    case RECTO: pos.az = angori; break;
    case VERSO: pos.az = angori + M_PI; break;
    }
    stopMovement();	// stop roting
    if (*caption) {
      //showCaption();
    }
  }
}

void Web::showCaption()
{
  if (! *caption) return;

  if (! text) {	// text not writed
    Pos postext = pos;
    V3 dim;

    getDim(dim);			// get dim of the surface
    postext.z += (dim.v[2] - 0.20);	// 20cm under the top
    postext.ax = M_PI_2;
    postext.az = pos.az + M_PI;
    postext.x += (dim.v[0] + 0.001) * sin(postext.az);	// 1mm near the surface
    postext.y -= (dim.v[1] - 0.01) * cos(postext.az);	// 1cm from the left margin

    text = new Text(caption, postext, 0.5, Color::green);	// scale half
    text->setPos(postext.x, postext.y, postext.z, postext.az, postext.ax);
  }
  else {	// remove the text
    text->toDelete();
    text = NULL;
  }
}

void Web::pivot()
{
  clearV3(move.aspeed);
  move.aspeed.v[0] = aspeed;
  imposedMovement(TTL);
  if (*caption) {
    showCaption();
  }
  face ^= 1;	// switch face
}

/* object intersects: projection */
bool Web::whenIntersect(Object *pcur, Object *pold)
{
  switch (pcur->type) {
  case USER_TYPE:
    projectPosition(pcur, pold);
    return true;
    break;
  default:
    return false;
    break;
  }
}

/* Opens browser */
void Web::open_cb(Web *web, void *d, time_t s, time_t u)
{
  Browser::start(web->name.url);
#if 0 //obsoleted
  if (!stringcmp(purl, "wrl") || !stringcmp(purl, "vrm")) {
    Browser::startvrml(web->name.url);
    return;
  }
#endif
}

/* Pivot */
void Web::pivot_cb(Web *web, void *d, time_t s, time_t u)
{
  web->pivot();
}

/* Caption */
void Web::caption_cb(Web *web, void *d, time_t s, time_t u)
{
  web->showCaption();
}

void Web::quit()
{
  oid = 0;
  if (text)
    text->toDelete();
}

void Web::funcs()
{
  getPropFunc(WEB_TYPE, PROPXY, _Payload get_xy);
  getPropFunc(WEB_TYPE, PROPZ, _Payload get_z);
  getPropFunc(WEB_TYPE, PROPAZ, _Payload get_az);
  getPropFunc(WEB_TYPE, PROPAX, _Payload get_ax);
  getPropFunc(WEB_TYPE, PROPAY, _Payload get_ay);
  getPropFunc(WEB_TYPE, PROPHNAME, _Payload get_hname);

  putPropFunc(WEB_TYPE, PROPXY, _Payload put_xy);
  putPropFunc(WEB_TYPE, PROPZ, _Payload put_z);
  putPropFunc(WEB_TYPE, PROPAZ, _Payload put_az);
  putPropFunc(WEB_TYPE, PROPAX, _Payload put_ax);
  putPropFunc(WEB_TYPE, PROPAY, _Payload put_ay);
  putPropFunc(WEB_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(WEB_TYPE, 0, _Action open_cb, "Open");
  setActionFunc(WEB_TYPE, 1, _Action pivot_cb, "Pivot");
  setActionFunc(WEB_TYPE, 2, _Action caption_cb, "Text");
}
