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
#ifndef SANDBOX_HPP
#define SANDBOX_HPP

static char sandbox_vre[] = "\
<vre version=\"8.0\" profile=\"Interactive\">\n\
<!--\n\
<head>\n\
<meta name=\"buildin\" content=\"sandbox_vre\"/>\n\
</head>\n\
-->\n\
<scene>\n\
\n\
<!-- the following line is not parsed after tag <scene> : to fix parse.cpp -->\n\
<!--<entry pos=\"0 0 0\"></entry>-->\n\
<wall name=\"border-n\" pos=\"20 0 .2 1.57\">  <solid dim=\"40 .2 1\" dif=\"red\" /></wall>\n\
<wall name=\"border-s\" pos=\"-20 0 .2 1.57\"> <solid dim=\"40 .2 1\" dif=\"red\" /></wall>\n\
<wall name=\"border-w\" pos=\"0 20 .2 0\">     <solid dim=\"40 .2 1\" dif=\"red\" /></wall>\n\
<wall name=\"border-e\" pos=\"0 -20 .2 0\">    <solid dim=\"40 .2 1\" dif=\"red\" /></wall>\n\
<ground name=\"ground\" pos=\"0 0 -.1\"> <solid dim=\"40 40 .1\" dif=\".3 .3 .3\" /></ground>\n\
<!--\n\
<mirage name=\"sky\" aspeed=\".01\" mode=\"self\">\n\
 <solid shape=\"sphere\" r=\"30\" tx=\"/gif/sky.gif\" slices=\"255\" stacks=\"255\" />\n\
</mirage>\n\
-->\n\
\n\
</scene>\n\
</vre>\n\
";

#endif
