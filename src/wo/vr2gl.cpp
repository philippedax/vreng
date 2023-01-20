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
#include "wobject.hpp"
#include "solid.hpp"	// Solid
#include "render.hpp"	// ::g.render
#include "user.hpp"	// localuser
#include <list>


/* Updates objects in 3D */
void WObject::updateAll3D(Pos &pos)
{
  if (! solid || removed) return;

  for (list<Solid*>::iterator s = _solids.begin(); s != _solids.end(); s++) {
    M4 matobj = mulM4(transM4(pos.x, pos.y, pos.z),
			      mulM4(rotM4(pos.az, UZ),
                                    mulM4(rotM4(pos.ay, UY),
			                  rotM4(pos.ax, UX)
                                         )
                                   )
                     );
    (*s)->setPosition(matobj);
  }
}

void WObject::update3D(Pos &pos)
{
  if (! solid || removed) return;

  // checks if pos is out of bounds : FIXME!
  if ( localuser->pos.x>100 || localuser->pos.x<-100 || localuser->pos.y>100 || localuser->pos.y<-100 || localuser->pos.z>100 || localuser->pos.z<-100 ) {
    echo("localuser->pos: %.1f %.1f %.1f", localuser->pos.x, localuser->pos.y, localuser->pos.z);
    echo("reset localuser pos");
    localuser->pos.x = localuser->pos.y = localuser->pos.z = 0;
  }

  for (list<Solid*>::iterator s = _solids.begin(); s != _solids.end(); s++) {
    M4 matobj = mulM4(transM4(pos.x, pos.y, pos.z),
			      mulM4(rotM4(pos.az, UZ),
			            rotM4(pos.ax, UX)
                             )
                     );
    (*s)->setPosition(matobj);
  }
}

/* Updates camera in 3D */
void WObject::updateCamera(Pos &pos)
{
  M4 matcam = mulM4(rotM4(-M_PI_2, UX),
                    mulM4(rotM4(M_PI_2, UZ),
                          mulM4(rotM4(pos.ax, UX),
                                mulM4(rotM4(pos.ay, UY),
                                      mulM4(rotM4(-pos.az, UZ),
                                            transM4(-pos.x, -pos.y, -(pos.z + localuser->height/2))
                                           )
                                     )
                               )
                         )
                   );
  ::g.render.setCameraPosition(&matcam);
}

/* Returns three vectors (v, w, n) that describe the object's surface */
void WObject::getSurfVecs(Pos &pos, V3 *v, V3 *w, V3 *normal)
{
  M4 rot = mulM4(rotM4(pos.az, UZ), rotM4(pos.ax, UX));
  V3 vec = setV3(1, 0, 0);
  mulM4V3(v, &rot, &vec);       // v
  vec = setV3(0, 1, 0);
  mulM4V3(w, &rot, &vec);       // w
  vec = setV3(0, 0, 1);
  mulM4V3(normal, &rot, &vec);  // n
}
