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
#include "web.hpp"
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
WObject * Web::creator(char *l)
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
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (! stringcmp(l, "url"))
      l = parse()->parseUrl(l, names.url);
    else if (! stringcmp(l, "caption"))
      l = parse()->parseCaption(l, caption, "caption");
    else if (! stringcmp(l, "text"))
      l = parse()->parseCaption(l, caption, "text");
  }
  end_while_parse(l);
}

Web::Web(char *l)
{
  face = RECTO;	// recto face
  aspeed = ASPEED;
  text = NULL;

  parser(l);
  angori = pos.az;

  initMobileObject(TTL);
  createPermanentNetObject(PROPS, ++oid);
}

void Web::updateTime(time_t sec, time_t usec, float *lasting)
{
  updateLasting(sec, usec, lasting);
}

void Web::changePosition(float lasting)
{ 
  static float rot = 0;

  //error("rot: %.1f %.1f %.2f", rot, pos.az, lasting);
  if (rot < M_PI) {
    pos.az += lasting * move.aspeed.v[0];
    rot += lasting * move.aspeed.v[0];
  }
  else {
    rot = 0;
    switch (face) {
    case RECTO: pos.az = angori; break;
    case VERSO: pos.az = angori + M_PI; break;
    }
    stopImposedMovement();
    if (caption) {
      //showCaption();
    }
  }
}

void Web::showCaption()
{
  if (! caption) return;

  if (! text) {	// text not writed
    Pos postext = pos;
    V3 dim;

    getDimBB(dim);			// get dim of the surface
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
  initImposedMovement(TTL);
  if (caption) {
    showCaption();
  }
  face ^= 1;	// switch face
}

/* object intersects: projection */
bool Web::whenIntersect(WObject *pcur, WObject *pold)
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
  Browser::start(web->names.url);
#if 0 //obsoleted
  if (!stringcmp(purl, "wrl") || !stringcmp(purl, "vrm")) {
    Browser::startvrml(web->names.url);
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
  savePersistency();
  if (text)
    text->toDelete();
}

void Web::funcs()
{
  getPropertyFunc(WEB_TYPE, PROPXY, _Payload get_xy);
  getPropertyFunc(WEB_TYPE, PROPZ, _Payload get_z);
  getPropertyFunc(WEB_TYPE, PROPAZ, _Payload get_az);
  getPropertyFunc(WEB_TYPE, PROPAX, _Payload get_ax);
  getPropertyFunc(WEB_TYPE, PROPAY, _Payload get_ay);
  getPropertyFunc(WEB_TYPE, PROPHNAME, _Payload get_hname);

  putPropertyFunc(WEB_TYPE, PROPXY, _Payload put_xy);
  putPropertyFunc(WEB_TYPE, PROPZ, _Payload put_z);
  putPropertyFunc(WEB_TYPE, PROPAZ, _Payload put_az);
  putPropertyFunc(WEB_TYPE, PROPAX, _Payload put_ax);
  putPropertyFunc(WEB_TYPE, PROPAY, _Payload put_ay);
  putPropertyFunc(WEB_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(WEB_TYPE, 0, _Action open_cb, "Open");
  setActionFunc(WEB_TYPE, 1, _Action pivot_cb, "Pivot");
  setActionFunc(WEB_TYPE, 2, _Action caption_cb, "Text");
}
