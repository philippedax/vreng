WHAT IS VRENG ?
---------------
VREng (Virtual Reality Engine) is an interactive and distributed 3D application
allowing interactions in real-time between avatars and objects and navigation
in virtual worlds connected over the Internet.


REQUIREMENTS AND RECOMMENDATIONS
--------------------------------
You need the X-Window System (Xorg, X11R6 or Openwin).
Your visual must have a color depth of 8, 15, 16 or 24.

The following libraries are required: libjpeg, libungif, libXpm.
Other recommended libraries: libmysqlclient, libz, libpng.

Your machine and your network should support IP Multicast, mainly IGMPv2
and Multicast routing protocols to see other avatars in the same scene.
If is not the case a Unicast/Multicast Reflector is activated when vreng
is launched. VREng detects automatically if your host is on the MBone,
if true, VREng switches in Multicast mode, else continues in Unicast mode
with the Reflector.


SUPPORTED PLATFORMS
-------------------
	- Linux-*
	- MacOSX-1[01].*
	- Solaris*
	- Sunos4.1.*
	- HP-UX-B.10.20
	- FreeBsd[345].*
	- NetBsd*
	- CygWin32

VREng has not been ported on other platforms yet, but any port will be welcome.


DOWNLOADING
-----------
The VREng sources are available by:
  - Git:        git clone https://github.com/philippedax/vreng.git (recommanded)
  - Git:        git clone https://gitlab.freedesktop.org/philippedax/vreng.git


COMPILING
---------

If the script "configure" is not present, you need to build it first by:
	./build or ./autogen.sh

VREng uses an OpenGL library. The configure script will usually figure out
which one is installed on your system. There are basically two possibilities:
  1 - Your system already has a vendor OpenGL library installed.
  2 - If you want to use the Mesa free OpenGL emulation library (http://www.mesa3d.org/).

If your OpenGL library is not visible through the standard paths, you
can use the configure option: --with-GL-prefix=DIR.

VREng works with the Ubit toolkit.
Since vreng-7.0.0 the Ubit Toolkit is built in VReng by default.

Before compiling VREng check available features with:
	./configure --help

To compile VREng with defaults, type:
	./configure

and then
	make or gmake


INSTALLING
----------
The vreng binary executable and the vre script-shell
will be installed by default in /usr/local/bin.
To do this, type:
	make install
or
	make install-strip


HOW TO NAVIGATE ?
-----------------
You can alternatively use left button's mouse either arrow keys either
menu buttons to navigate in a virtual scene.
Arrow keys on your keyboard allow 6 spatial degrees of freedom:

- `key_up` to go straight on
- `key_down` to move back
- `key_right` to turn on your right
- `key_left` to turn on your left
- `key_pageup` to look up
- `key_pagedown` to look down
 
plus

- `key_home` to look forward
- `key_end` to accelerate
- `key_insert` to rise up
- `key_delete` to land on the ground

To identify objects, click on the left or right button of the mouse, the name
of the pointed object will appear, maybe with a pushdown menu containing buttons
to activate an action on this object (for example open/close for a door).


DOCUMENTATION
-------------
For more details, the documentation is available at
https://web.archive.org/web/20160329013641/http://www.vreng.enst.fr/html/index.html

It is recommended to see also the ChangeLog file for more informations.


AUTHORS
-------

- Denis Arnaud <denis.arnaud@free.fr>: interface between world mgt and network
- Fabrice Bellard <fabrice@bellard.org>: global architecture and 3D rendering
- Stephane Belmon <belmon@email.enst.fr>: network interface
- Samuel Orzan <orzan@email.enst.fr>: world management
- Lionel Ulmer <ulmer@email.enst.fr>: the GUI under X11
- Eric Lecolinet <elc@telecom-paris.fr>: new GUI + Ubit toolkit
- Philippe Dax <philippe.dax@gmail.com>: project supervisor

CONTRIBUTORS
------------
- Francois Karr : Windows95 port
- Vincent Haverlant : RTP protocol
- Adrien Felon : aoi object
- Patrick Bellot <bellot@enst.fr>: vrel language
- Ankit Jalote : humanoid
- Yann Renard : face, bone, v3d
- Mathieu Seigneurin : vjs, vjc, vrelet
- Guillaume Brule : bap
- Pierre Gros : transform, lwo, mysql
- Mario Trentini : landmarks, axis
- Julien Desreumaux - Sebastien Piraud : book, sheet
- Romain Piegay : views, 3ds, ase, repeat textures
- Pascal Chambon : x3d, carousel
- Eric Varadaradjou : communication agent, ocaml

MAINTAINER
----------
Philippe Dax <philippe.dax@gmail.com>: project supervisor

CONTACTS
--------
You can send BUG reports and comments via Github.

Have fun!

-- Philippe Dax

-----

Copyright (C) 1996 - 2021
  Free Software Foundation, Inc.

This file is part of GNU VREng.

GNU VREng is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU VREng is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with autoconf; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
