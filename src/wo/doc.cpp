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
#include "browser.hpp"	// start
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
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (!stringcmp(l, "url")) l = parse()->parseUrl(l, names.url);
  }
  end_while_parse(l);
}

Doc::Doc(char *l)
{
  parser(l);

  enableBehavior(PERSISTENT);
  enableBehavior(TAKABLE);
  setRenderPrior(RENDER_NORMAL);

  initializeMobileObject(0);
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

void Doc::quit()
{
  Ps::quit();
  oid = 0;
  savePersistency();
}

void Doc::funcs()
{
  getPropertyFunc(DOC_TYPE, PROPXY, WO_PAYLOAD get_xy);
  getPropertyFunc(DOC_TYPE, PROPZ, WO_PAYLOAD get_z);
  getPropertyFunc(DOC_TYPE, PROPAZ, WO_PAYLOAD get_az);
  getPropertyFunc(DOC_TYPE, PROPAX, WO_PAYLOAD get_ax);
  getPropertyFunc(DOC_TYPE, PROPAY, WO_PAYLOAD get_ay);
  getPropertyFunc(DOC_TYPE, PROPHNAME, WO_PAYLOAD get_hname);

  putPropertyFunc(DOC_TYPE, PROPXY, WO_PAYLOAD put_xy);
  putPropertyFunc(DOC_TYPE, PROPZ, WO_PAYLOAD put_z);
  putPropertyFunc(DOC_TYPE, PROPAZ, WO_PAYLOAD put_az);
  putPropertyFunc(DOC_TYPE, PROPAX, WO_PAYLOAD put_ax);
  putPropertyFunc(DOC_TYPE, PROPAY, WO_PAYLOAD put_ay);
  putPropertyFunc(DOC_TYPE, PROPHNAME, WO_PAYLOAD put_hname);

  setActionFunc(DOC_TYPE, 0, WO_ACTION view_cb, "View");
  setActionFunc(DOC_TYPE, 1, WO_ACTION print_cb, "Print");
  setActionFunc(DOC_TYPE, 2, WO_ACTION cancel_cb, "Cancel");
  setActionFunc(DOC_TYPE, 3, WO_ACTION moveObject, "Move");
}
