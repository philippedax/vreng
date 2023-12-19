const char README[] = "\
WHAT IS VRENG ?\n\
---------------\n\
VREng (Virtual Reality Engine) is an interactive and distributed 3D application\n\
allowing interactions in real-time between avatars and objects and navigation\n\
in virtual worlds connected over the Internet.\n\
\n\
\n\
REQUIREMENTS AND RECOMMENDATIONS\n\
--------------------------------\n\
You need the X-Window System (Xorg, X11R6 or Openwin).\n\
Your visual must have a color depth of 8, 15, 16 or 24.\n\
\n\
The following libraries are required: libjpeg, libungif, libXpm.\n\
Other recommended libraries: libmysqlclient, libz, libpng.\n\
\n\
Your machine and your network should support IP Multicast, mainly IGMPv2\n\
and Multicast routing protocols to see other avatars in the same scene.\n\
If is not the case a Unicast/Multicast Reflector is activated when vreng\n\
is launched. VREng detects automatically if your host is on the MBone,\n\
if true, VREng switches in Multicast mode, else continues in Unicast mode\n\
with the Reflector.\n\
\n\
\n\
SUPPORTED PLATFORMS\n\
-------------------\n\
	- Linux-*\n\
	- MacOSX-1[01].*\n\
	- Solaris*\n\
	- Sunos4.1.*\n\
	- HP-UX-B.10.20\n\
	- FreeBsd[345].*\n\
	- NetBsd*\n\
	- CygWin32\n\
\n\
VREng has not been ported on other platforms yet, but any port will be welcome.\n\
\n\
\n\
DOWNLOADING\n\
-----------\n\
The VREng sources are available by:\n\
  - Git:        git clone https://github.com/philippedax/vreng.git (recommanded)\n\
  - Git:        git clone https://gitlab.freedesktop.org/philippedax/vreng.git\n\
\n\
\n\
COMPILING\n\
---------\n\
\n\
If the script \"configure\" is not present, you need to build it first by:\n\
	./build or ./autogen.sh\n\
\n\
VREng uses an OpenGL library. The configure script will usually figure out\n\
which one is installed on your system. There are basically two possibilities:\n\
  1 - Your system already has a vendor OpenGL library installed.\n\
  2 - If you want to use the Mesa free OpenGL emulation library (http://www.mesa3d.org/).\n\
\n\
If your OpenGL library is not visible through the standard paths, you\n\
can use the configure option: --with-GL-prefix=DIR.\n\
\n\
VREng works with the Ubit toolkit.\n\
Since vreng-7.0.0 the Ubit Toolkit is built in VReng by default.\n\
\n\
Before compiling VREng check available features with:\n\
	./configure --help\n\
\n\
To compile VREng with defaults, type:\n\
	./configure\n\
\n\
and then\n\
	make or gmake\n\
\n\
\n\
INSTALLING\n\
----------\n\
The vreng binary executable and the vre script-shell\n\
will be installed by default in /usr/local/bin.\n\
To do this, type:\n\
	make install\n\
or\n\
	make install-strip\n\
\n\
\n\
HOW TO NAVIGATE ?\n\
-----------------\n\
You can alternatively use left button's mouse either arrow keys either\n\
menu buttons to navigate in a virtual scene.\n\
Arrow keys on your keyboard allow 6 spatial degrees of freedom:\n\
\n\
- `key_up` to go straight on\n\
- `key_down` to move back\n\
- `key_right` to turn on your right\n\
- `key_left` to turn on your left\n\
- `key_pageup` to look up\n\
- `key_pagedown` to look down\n\
 \n\
plus\n\
\n\
- `key_home` to look forward\n\
- `key_end` to accelerate\n\
- `key_insert` to rise up\n\
- `key_delete` to land on the ground\n\
\n\
To identify objects, click on the left or right button of the mouse, the name\n\
of the pointed object will appear, maybe with a pushdown menu containing buttons\n\
to activate an action on this object (for example open/close for a door).\n\
\n\
\n\
DOCUMENTATION\n\
-------------\n\
For more details, the documentation is available at\n\
https://web.archive.org/web/20160329013641/http://www.vreng.enst.fr/html/index.html\n\
\n\
It is recommended to see also the ChangeLog file for more informations.\n\
\n\
\n\
AUTHORS\n\
-------\n\
\n\
- Denis Arnaud <denis.arnaud@free.fr>: interface between world mgt and network\n\
- Fabrice Bellard <fabrice@bellard.org>: global architecture and 3D rendering\n\
- Stephane Belmon <belmon@email.enst.fr>: network interface\n\
- Samuel Orzan <orzan@email.enst.fr>: world management\n\
- Lionel Ulmer <ulmer@email.enst.fr>: the GUI under X11\n\
- Eric Lecolinet <elc@telecom-paris.fr>: new GUI + Ubit toolkit\n\
- Philippe Dax <philippe.dax@gmail.com>: project supervisor\n\
\n\
CONTRIBUTORS\n\
------------\n\
- Francois Karr : Windows95 port\n\
- Vincent Haverlant : RTP protocol\n\
- Adrien Felon : aoi object\n\
- Patrick Bellot <bellot@enst.fr>: vrel language\n\
- Ankit Jalote : humanoid\n\
- Yann Renard : face, bone, v3d\n\
- Mathieu Seigneurin : vjs, vjc, vrelet\n\
- Guillaume Brule : bap\n\
- Pierre Gros : transform, lwo, mysql\n\
- Mario Trentini : landmarks, axis\n\
- Julien Desreumaux - Sebastien Piraud : book, sheet\n\
- Romain Piegay : views, 3ds, ase, repeat textures\n\
- Pascal Chambon : x3d, carousel\n\
- Eric Varadaradjou : communication agent, ocaml\n\
\n\
MAINTAINER\n\
----------\n\
Philippe Dax <philippe.dax@gmail.com>: project supervisor\n\
\n\
CONTACTS\n\
--------\n\
You can send BUG reports and comments via Github.\n\
\n\
Have fun!\n\
\n\
-- Philippe Dax\n\
\n\
-----\n\
\n\
Copyright (C) 1996 - 2021\n\
  Free Software Foundation, Inc.\n\
\n\
This file is part of GNU VREng.\n\
\n\
GNU VREng is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation; either version 2, or (at your option)\n\
any later version.\n\
\n\
GNU VREng is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with autoconf; see the file COPYING.  If not, write to\n\
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,\n\
Boston, MA 02111-1307, USA.\n\
\n\
";
