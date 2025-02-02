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
// doc.cpp
//
// Document handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "doc.hpp"
#include "ps.hpp"	// view, print


const OClass Doc::oclass(DOC_TYPE, "Doc", Doc::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
Object * Doc::creator(char *l)
{
  return new Doc(l);
}

void Doc::parser(char *l)
{
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (! stringcmp(l, "url")) l = parseUrl(l, name.url);
  }
  end_while_parse(l);
}

Doc::Doc(char *l)
{
  parser(l);

  enableBehavior(TAKABLE);

  mobileObject(0);
  createNetObj(PROPS, ++oid);
}

bool Doc::publish(const Pos &oldpos)
{
  return publishPos(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

bool Doc::whenIntersect(Object *pcur, Object *pold)
{
  pold->copyPositionAndBB(pcur);
  return true;
}

void Doc::quit()
{
  Ps::quit();
}

void Doc::view_cb(Doc *doc, void *d, time_t s, time_t u)
{
  Ps::view(doc->name.url);
}

void Doc::print_cb(Doc *doc, void *d, time_t s, time_t u)
{
  Ps::print(doc->name.url);
}

void Doc::cancel_cb(Doc *doc, void *d, time_t s, time_t u)
{
  Ps::quit();
}

void Doc::funcs()
{
  getPropFunc(DOC_TYPE, PROPXY, _Payload get_xy);
  getPropFunc(DOC_TYPE, PROPZ, _Payload get_z);
  getPropFunc(DOC_TYPE, PROPAZ, _Payload get_az);
  getPropFunc(DOC_TYPE, PROPAX, _Payload get_ax);
  getPropFunc(DOC_TYPE, PROPAY, _Payload get_ay);
  getPropFunc(DOC_TYPE, PROPHNAME, _Payload get_hname);

  putPropFunc(DOC_TYPE, PROPXY, _Payload put_xy);
  putPropFunc(DOC_TYPE, PROPZ, _Payload put_z);
  putPropFunc(DOC_TYPE, PROPAZ, _Payload put_az);
  putPropFunc(DOC_TYPE, PROPAX, _Payload put_ax);
  putPropFunc(DOC_TYPE, PROPAY, _Payload put_ay);
  putPropFunc(DOC_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(DOC_TYPE, 0, _Action view_cb, "View");
  setActionFunc(DOC_TYPE, 1, _Action print_cb, "Print");
  setActionFunc(DOC_TYPE, 2, _Action cancel_cb, "Cancel");
  //setActionFunc(DOC_TYPE, 3, _Action moveObject, "Move");
}
