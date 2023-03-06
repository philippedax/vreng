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
#include "vreng.hpp"
#include "doc.hpp"
#include "ps.hpp"	// view, print


const OClass Doc::oclass(DOC_TYPE, "Doc", Doc::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Doc::creator(char *l)
{
  return new Doc(l);
}

void Doc::parser(char *l)
{
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (! stringcmp(l, "url")) l = parseUrl(l, names.url);
  }
  end_while_parse(l);
}

Doc::Doc(char *l)
{
  parser(l);

  enableBehavior(TAKABLE);

  initMobileObject(0);
  createPermanentNetObject(PROPS, ++oid);
}

bool Doc::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

bool Doc::whenIntersect(WObject *pcur, WObject *pold)
{
  pold->copyPositionAndBB(pcur);
  return true;
}

void Doc::quit()
{
  Ps::quit();
  savePersistency();
}

void Doc::view_cb(Doc *doc, void *d, time_t s, time_t u)
{
  Ps::view(doc->names.url);
}

void Doc::print_cb(Doc *doc, void *d, time_t s, time_t u)
{
  Ps::print(doc->names.url);
}

void Doc::cancel_cb(Doc *doc, void *d, time_t s, time_t u)
{
  Ps::quit();
}

void Doc::funcs()
{
  getPropertyFunc(DOC_TYPE, PROPXY, _Payload get_xy);
  getPropertyFunc(DOC_TYPE, PROPZ, _Payload get_z);
  getPropertyFunc(DOC_TYPE, PROPAZ, _Payload get_az);
  getPropertyFunc(DOC_TYPE, PROPAX, _Payload get_ax);
  getPropertyFunc(DOC_TYPE, PROPAY, _Payload get_ay);
  getPropertyFunc(DOC_TYPE, PROPHNAME, _Payload get_hname);

  putPropertyFunc(DOC_TYPE, PROPXY, _Payload put_xy);
  putPropertyFunc(DOC_TYPE, PROPZ, _Payload put_z);
  putPropertyFunc(DOC_TYPE, PROPAZ, _Payload put_az);
  putPropertyFunc(DOC_TYPE, PROPAX, _Payload put_ax);
  putPropertyFunc(DOC_TYPE, PROPAY, _Payload put_ay);
  putPropertyFunc(DOC_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(DOC_TYPE, 0, _Action view_cb, "View");
  setActionFunc(DOC_TYPE, 1, _Action print_cb, "Print");
  setActionFunc(DOC_TYPE, 2, _Action cancel_cb, "Cancel");
  //setActionFunc(DOC_TYPE, 3, _Action moveObject, "Move");
}
