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
#ifndef VRE_HPP
#define VRE_HPP

static char sandbox_vre[] = "\
<vre profile=\"Interactive\">\n\
<head> <meta name=\"filename\" content=\"sandbox.vre\"/> </head>\n\
<scene>\n\
<ground name=\"ground\" pos=\"0 0 -.2\"> <solid dim=\"20 20 .1\" zp=\"/gif/pelouse.gif\" /></ground>\n\
<wall name=\"border-n\" pos=\"20 0 .2 1.57\"> <solid dim=\"20 .02 .4\" dif=\"1 0 0\" /></wall>\n\
<wall name=\"border-s\" pos=\"-20 0 .2 1.57\"> <solid dim=\"20 .02 .4\" dif=\"1 0 0\" /></wall>\n\
<wall name=\"border-w\" pos=\"0 20 .2 0\"> <solid dim=\"20 .02 .4\" dif=\"1 0 0\" /></wall>\n\
<wall name=\"border-e\" pos=\"0 -20 .2 0\"> <solid dim=\"20 .02 .4\" dif=\"1 0 0\" /></wall>\n\
<mirage name=\"sky\" aspeed=\".01\" mode=\"self\">\n\
 <solid shape=\"sphere\" r=\"30\" tx=\"/gif/sky.gif\" slices=\"255\" stacks=\"255\" />\n\
</mirage>\n\
</scene>\n\
</vre>";

#endif
