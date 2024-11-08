WHAT IS VRENG ?
---------------
VREng (Virtual Reality Engine) is an interactive and distributed 3D application
allowing interactions in real-time between avatars and objects and navigation
in virtual worlds connected over the Internet.


REQUIREMENTS AND RECOMMENDATIONS
--------------------------------
To build VREng you need a C++ and C compiler (for ex. g++, gcc).
Furthmore you need the X-Window System X11 (Xorg, X11R6 or Openwin).
Your visual must have a color depth of 8, 16, 24 or 32.

The following libraries are required: libjpeg, libungif, libXpm.
Other recommended libraries: libmysqlclient, libz, libpng.
For more details see md/REQUIREMENTS.md.

Your machine and your network should support IP Multicast, mainly IGMPv2
and Multicast routing protocols to see other avatars in the same scene.
If is not the case a Unicast/Multicast Reflector is activated when vreng
is launched. VREng detects automatically if your host is on the MBone,
if true, VREng switches in Multicast mode, else continues in Unicast mode.


SUPPORTED PLATFORMS
-------------------
	- Linux* (debian, unbutu, fedora, centos, arch,...)
	- MacOSX*
	- FreeBSD*
	- NetBSD*
	- Solaris*
	- Sunos4.1.*
	- HP-UX-B.10.20
	- CygWin32

VREng has not been ported on other platforms yet, but any port will be welcome.


DOWNLOADING
-----------
The VREng sources are available by:
	git clone https://github.com/philippedax/vreng.git


COMPILING
---------

If the script "configure" is not present, you need to build it first by:
	./build [args...]
where args are the same as those of ./configure [args...].

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

To configure and compile VREng with defaults, type:
	./configure

and then type:
	make or gmake

To launch VREng type:
        ./vr or ./vreng


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

It is recommended to see also the ChangeLog file md/CHANGELOG.md for more informations.
The history of commits are also presents in conf/history


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
