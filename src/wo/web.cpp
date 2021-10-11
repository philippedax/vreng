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
#include "move.hpp"	// gotoFront
#include "browser.hpp"	// start
#include "user.hpp"	// USER_TYPE
#include "text.hpp"	// Text
#include "color.hpp"	// Color


const OClass Web::oclass(WEB_TYPE, "Web", Web::creator);

const float Web::TTL = 4.;       // 4 sec max
const float Web::ASPEED = M_PI;  // 180deg/sec

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Web::creator(char *l)
{
  return new Web(l);
}

void Web::defaults()
{
  face = 0;	// front face
  aspeed = ASPEED;
  atemp = 0;
  text = NULL;
  legend = NULL;
}

void Web::parselegend(char *l)
{
  char line[256];	// must be >128 else segfault !!!

  strcpy(line, l);
  char *p = strstr(line, "legend=");
  if (p) {
    p += 8;
    char *q = strchr(p, '"');
    if (q) *q = '\0';
    legend = new char[128];
    strcpy(legend, p);
  }
}

void Web::parser(char *l)
{
  defaults();
  parselegend(l);
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (! stringcmp(l, "url")) l = parse()->parseUrl(l, names.url);
    else if (! stringcmp(l, "legend")) {
      l += 7; // skip legend=
      l = parse()->skipAttribute(l);
    }
  }
  end_while_parse(l);
}

Web::Web(char *l)
{
  parser(l);

  afront = pos.az;
  acurr = afront;
  aback = afront + M_PI;

  initMobileObject(TTL);
  createPermanentNetObject(PROPS, ++oid);
}

void Web::updateTime(time_t sec, time_t usec, float *lasting)
{
  updateLasting(sec, usec, lasting);
}

void Web::changePosition(float lasting)
{ 
  if (atemp < M_PI) {
    pos.az += lasting * move.aspeed.v[0];
    atemp += lasting * move.aspeed.v[0];
  }
  else {
    pos.az = acurr + M_PI;
    stopImposedMovement();
  }
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

void Web::pivot()
{
  float nexta;

  acurr = pos.az;
  atemp = 0;
  switch (face) {
  case 0: nexta = aback;  break;
  case 1: nexta = afront; break;
  }
  pos.alter = false;
  clearV3(move.aspeed);
  move.aspeed.v[0] = aspeed;
  initImposedMovement(TTL);

#if 0 //dax1
  if (text) {	// get rid of previous text
    text->delFromList(mobileList);
  }
#endif

  if (legend && nexta == aback) {	// back face
    Pos postx = pos;

    V3 dim;
    getDimBB(dim);			// get dim of the surface
    postx.z += (dim.v[2] - 0.20);	// 20cm under the top
    postx.ax = pos.ax;
    postx.az = afront;
    pos.az = afront;

    if (dim.v[0] > dim.v[1]) {
      postx.x += (dim.v[0] + 0.001) * sin(afront);	// 1mm near front face
      postx.y -= (dim.v[1] - 0.05) * cos(afront);	// 5cm from the left margin
    }
    else {
      postx.y += (dim.v[0] + 0.001) * sin(afront);	// 1mm near front face
      postx.x -= (dim.v[1] - 0.05) * cos(afront);	// 5cm from the left margin
    }

    text = new Text(legend, postx, 0.5, Color::black);	// scale half
    if (text) {
      text->setPos(postx.x, postx.y, postx.z, postx.az + M_PI, postx.ax + M_PI_2);
    }
  }
  else {	// front face
    if (text) {
      text->toDelete();
      text = NULL;
    }
  }

  face ^= 1;
  pos.alter = true;     // has changed
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

void Web::quit()
{
  oid = 0;
  savePersistency();
  if (text)
    text->toDelete();
  if (legend)
    delete[] legend;
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
  setActionFunc(WEB_TYPE, 2, _Action gotoFront, "Approach");
  setActionFunc(WEB_TYPE, 3, _Action moveObject, "Move");
}
