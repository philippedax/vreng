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
#include "host.hpp"
#include "xterm.hpp"	// start


const OClass Host::oclass(HOST_TYPE, "Host", Host::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Host::creator(char *l)
{
  return new Host(l);
}

void Host::parser(char *l)
{
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (! stringcmp(l, "host=")) l = parse()->parseString(l, names.url, "host");
  }
  end_while_parse(l);
}

Host::Host(char *l)
{
  parser(l);

  initMobileObject(0);
  createPermanentNetObject(PROPS, ++oid);
}

bool Host::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

/* object intersects: stop */
bool Host::whenIntersect(WObject *pcur, WObject *pold)
{
  pold->copyPositionAndBB(pcur);
  return true;
}

/* xterm */
void Host::connect(Host *host, void *d, time_t s, time_t u)
{
  char *h = strrchr(host->names.url, '/');

  if (h)	//url telnet://host
    h++;
  else		// host
    h = host->names.url;
  trace(DBG_TOOL, "connect: to %s", h);
  Xterm::start(h);
}

void Host::disconnect(Host *host, void *d, time_t s, time_t u)
{
  Xterm::quit();
}

void Host::quit()
{
  Xterm::quit();
  oid = 0;
  savePersistency();
}

void Host::funcs()
{
  getPropertyFunc(HOST_TYPE, PROPXY, _Payload get_xy);
  getPropertyFunc(HOST_TYPE, PROPZ, _Payload get_z);
  getPropertyFunc(HOST_TYPE, PROPAZ, _Payload get_az);
  getPropertyFunc(HOST_TYPE, PROPAX, _Payload get_ax);
  getPropertyFunc(HOST_TYPE, PROPAY, _Payload get_ay);
  getPropertyFunc(HOST_TYPE, PROPHNAME, _Payload get_hname);

  putPropertyFunc(HOST_TYPE, PROPXY, _Payload put_xy);
  putPropertyFunc(HOST_TYPE, PROPZ, _Payload put_z);
  putPropertyFunc(HOST_TYPE, PROPAZ, _Payload put_az);
  putPropertyFunc(HOST_TYPE, PROPAX, _Payload put_ax);
  putPropertyFunc(HOST_TYPE, PROPAY, _Payload put_ay);
  putPropertyFunc(HOST_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(HOST_TYPE, 0, _Action connect, "Connect");
  setActionFunc(HOST_TYPE, 1, _Action disconnect, "Disconnect");
  setActionFunc(HOST_TYPE, 2, _Action moveObject, "Move");
}
