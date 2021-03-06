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
#include "halo.hpp"
#include "user.hpp"	// User


const OClass Halo::oclass(HALO_TYPE, "Halo", Halo::creator);

GLfloat Halo::RI = 0.06;
GLfloat Halo::RE = 0.09;

//local
static Halo *phalo = NULL;
static uint16_t oid = 0;


WObject * Halo::creator(char *l)
{ 
  return new Halo(l);
}

void Halo::defaults()
{
  article = Cloth::HALO;
  ttl = MAXFLOAT;
  dx = 0;
  dy = 0;
  dz = 0.05;		// 5 cm above
  dax = M_PI_4 / 2;	// oblique PI/8
  day = 0;
  daz = -M_PI_2;	// behind

  color[0] = .94;
  color[1] = .90;
  color[2] = 0;
  color[3] = 1;
  sprintf(strcolor, "%.2f %.2f %.2f", color[0], color[1], color[2]);
}

/* solid geometry */
void Halo::makeSolid()
{
  char s[128];

  sprintf(s, "solid shape=\"disk\" ri=\"%f\" re=\"%f\" dif=\"%s\" />", RI, RE, strcolor);
  parse()->parseSolid(s, SEP, this);
}

/* Created from file */
void Halo::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);	// <solid ... />
    if (!l) break;
  }
  end_while_parse(l);
}

/* Created from file */
Halo::Halo(char *l)
{
  parser(l);
  taken = false;
  behavior();
}

/* Regenerated by world via MySql */
Halo::Halo(User *user, void *d, time_t s, time_t u)
{
  char *str = (char *) d;       // name transmitted
  if (!str)  return;

  strcpy(names.given, str);
  strcpy(names.type, typeName());     // need names.type for MySql

  phalo = this;
  defaults();
  taken = true;
  makeSolid();
  setName();
  setOwner();
  getPersist();
  behavior();
  inits();
}

void Halo::quit()
{
  phalo = NULL;
  oid = 0;
  savePersistency();
}

/* wear */
void Halo::wear()
{
  taken = true;
  defaults();
  setName();
  setOwner();
  setPersist();
  behavior();
  inits();
  addToWearList();
}

/* takeoff */
void Halo::takeoff()
{
  // restore original position
  restorePosition();
  delPersist();
  delFromWearList();
  taken = false;
}

/* wear: indirectly called by user button */
void Halo::wear_cb(Halo *halo, void *d, time_t s, time_t u)
{
  halo->wear();
}

/* takeoff: indirectly called by user button */
void Halo::takeoff_cb(Halo *halo, void *d, time_t s, time_t u)
{
  halo->takeoff();
}

/* Creation: this method is invisible: called by the World */
void Halo::recreate_cb(User *pu, void *d, time_t s, time_t u)
{
  new Halo(pu, d, s, u);
}

void Halo::funcs()
{
  getPropertyFunc(HALO_TYPE, PROPXY, WO_PAYLOAD get_xy);
  getPropertyFunc(HALO_TYPE, PROPZ, WO_PAYLOAD get_z);
  getPropertyFunc(HALO_TYPE, PROPAZ, WO_PAYLOAD get_az);
  getPropertyFunc(HALO_TYPE, PROPAX, WO_PAYLOAD get_ax);
  getPropertyFunc(HALO_TYPE, PROPAY, WO_PAYLOAD get_ay);
  getPropertyFunc(HALO_TYPE, PROPHNAME, WO_PAYLOAD get_hname);

  putPropertyFunc(HALO_TYPE, PROPXY, WO_PAYLOAD put_xy);
  putPropertyFunc(HALO_TYPE, PROPZ, WO_PAYLOAD put_z);
  putPropertyFunc(HALO_TYPE, PROPAZ, WO_PAYLOAD put_az);
  putPropertyFunc(HALO_TYPE, PROPAX, WO_PAYLOAD put_ax);
  putPropertyFunc(HALO_TYPE, PROPAY, WO_PAYLOAD put_ay);
  putPropertyFunc(HALO_TYPE, PROPHNAME, WO_PAYLOAD put_hname);

  setActionFunc(HALO_TYPE, WEAR, WO_ACTION wear_cb, "Wear");
  setActionFunc(HALO_TYPE, TAKEOFF, WO_ACTION takeoff_cb, "Takeoff");

  setActionFunc(HALO_TYPE, RECREATE, WO_ACTION recreate_cb, "");
}
