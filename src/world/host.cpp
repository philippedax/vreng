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
// host.cpp
//
// Host handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "host.hpp"
#include "xterm.hpp"	// start


const OClass Host::oclass(HOST_TYPE, "Host", Host::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
Object * Host::creator(char *l)
{
  return new Host(l);
}

void Host::parser(char *l)
{
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (! stringcmp(l, "host=")) l = parseString(l, name.url, "host");
  }
  end_while_parse(l);
}

Host::Host(char *l)
{
  parser(l);

  mobileObject(0);
  createNetObj(PROPS, ++oid);
}

bool Host::publish(const Pos &oldpos)
{
  return publishPos(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

/* object intersects: stop */
bool Host::intersect(Object *pcur, Object *pold)
{
  pold->copyPositionAndBB(pcur);
  return true;
}

/* xterm */
void Host::connect(Host *host, void *d, time_t s, time_t u)
{
  char *h = strrchr(host->name.url, '/');

  if (h)	//url telnet://host
    h++;
  else		// host
    h = host->name.url;
  trace1(DBG_TOOL, "connect: to %s", h);
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
}

void Host::funcs()
{
  getPropFunc(HOST_TYPE, PROPXY, _Payload get_xy);
  getPropFunc(HOST_TYPE, PROPZ, _Payload get_z);
  getPropFunc(HOST_TYPE, PROPAZ, _Payload get_az);
  getPropFunc(HOST_TYPE, PROPAX, _Payload get_ax);
  getPropFunc(HOST_TYPE, PROPAY, _Payload get_ay);
  getPropFunc(HOST_TYPE, PROPHNAME, _Payload get_hname);

  putPropFunc(HOST_TYPE, PROPXY, _Payload put_xy);
  putPropFunc(HOST_TYPE, PROPZ, _Payload put_z);
  putPropFunc(HOST_TYPE, PROPAZ, _Payload put_az);
  putPropFunc(HOST_TYPE, PROPAX, _Payload put_ax);
  putPropFunc(HOST_TYPE, PROPAY, _Payload put_ay);
  putPropFunc(HOST_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(HOST_TYPE, 0, _Action connect, "Connect");
  setActionFunc(HOST_TYPE, 1, _Action disconnect, "Disconnect");
  setActionFunc(HOST_TYPE, 2, _Action moveObject, "Move");
}
