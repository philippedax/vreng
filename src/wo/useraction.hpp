//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://vreng.enst.fr/
//
// Copyright (C) 1997-2012 Philippe Dax
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
#ifndef USERACTION_HPP
#define USERACTION_HPP

struct UserAction {
  enum Type {
    HALO_WEAR,
    HALO_TAKEOFF,
    HAT_WEAR,
    HAT_TAKEOFF,
    DRESS_WEAR,
    DRESS_TAKEOFF,
    BULLET,
    DART,
    FOVYDEF,
    FOVYLESS,
    FOVYMORE,
    FOVYSET,
    LSPEEDDEF,
    LSPEEDLESS,
    LSPEEDMORE,
    ASPEEDDEF,
    ASPEEDLESS,
    ASPEEDMORE,
    PAUSE,
    PAUSEON,
    PAUSEOFF,
    SETLSPEED,
    SETASPEED,
    SWITCHVIEW,
    FIRSTVIEW,
    THIRDVIEWFAR,
    MAPVIEW,
    TPVIEWROTL,
    TPVIEWROTR,
    TPVIEWROTU,
    TPVIEWROTD,
    TPVIEWROTN,
    TPVIEWROTF,
    SETROLL,
    SETPITCH,
    PITCHMORE,
    PITCHLESS,
    ROLLMORE,
    ROLLLESS,
    NONE
  };
};

#endif
