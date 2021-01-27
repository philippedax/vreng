//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
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
#ifndef DEFAULTS_HPP
#define DEFAULTS_HPP

/* default servers */
#if STANDALONE
#define IPMC_ENABLED		0 // ipmulticast disabled
#define DEF_HTTP_SERVER		"localhost"
#define DEF_VJS_SERVER		"localhost"
#define DEF_VAPS_SERVER		"localhost"
#define DEF_VACS_SERVER		"localhost"
#define DEF_VRUM_SERVER		"localhost"
#define DEF_MYSQL_SERVER	"localhost"
#else
#define IPMC_ENABLED		1 // ipmulticast enabled
#define DEF_HTTP_SERVER		"localhost"	// should be "www.vreng.enst.fr"
#define DEF_VJS_SERVER		"localhost"
#define DEF_VAPS_SERVER		"localhost"
#define DEF_VACS_SERVER		"localhost"
#define DEF_VRUM_SERVER		"localhost"
#define DEF_MYSQL_SERVER	"localhost"
#endif

/* default URLs */
#define GEN_URL(h, u, p) 	("http://" h u p)
#if !defined(DEF_URL_PFX)
#define DEF_URL_PFX	"~dax/"		// should be ""
#endif
#define VRE_VERSION	8
#define DEF_URL_WORLD	"/vre/v8/Rendezvous.vre"
#define DEF_URL_FRONT	"/gif/default.gif"
#define DEF_URL_BACK	"/gif/default.1.gif"
#define DEF_URL_BALL	"/gif/ball.gif"
#define DEF_URL_PAPER	"/jpg/paper.jpg"
#define DEF_URL_FONT	"/txf/arial-br-18.txf"
#define DEF_URL_NOISE	"/mp3/sheet.mp3"
#define DEF_URL_JAR	"/jar/vrengapp.jar"
#define VACS_PORT	4150 + VRE_VERSION

/* default IPmc addresses & ports */
#define VRENG_MADDR_BASE	"224.255.0.0"
#define VRENG_MPORT_BASE	52656
#define DEF_VRE_MPORT		52666
#define DEF_VRE_TTL		127
#define DEF_VRE_MADDR		"224.255.0.1"
#define DEF_VRE_CHANNEL		"224.255.0.1/52666/127"
#define DEF_MANAGER_CHANNEL	"224.255.0.0/52656/127"

#define SAP_ADDR		"224.2.127.254"
#define SAP_PORT		9875
#if 0 //unused
#define DEF_SPIF_MADDR		"225.235.0.1"
#define DEF_SPIF_MPORT		52650
#endif

#define DEF_RATE		50	// 50 frames/sec
#define DEF_RATIO3D		(float)(16./9.)	// screen ratio
#define DEF_WIDTH3D		960 //640	// screen width
#define DEF_HEIGHT3D		540 //360	// screen height
#define DEF_MAXSIMCON		8	// MAX simultaneous threaded connections
#define DEF_MAXFRAMES		255	// MAX frames
#define DEF_REFRESH_TIMEOUT	1.5	//orig: 1.5

#define PATH_LEN	128
#define CHAN_LEN	32
#define GROUP_LEN	16
#define URL_LEN		256
#define USER_LEN	9	// max lenght of user name
#define OBJNAME_LEN	64	// max lenght of object name
#define HNAME_LEN	11	// max length of understandable name
#define ACTIONNAME_LEN	16	// max length of action name
#define ACTIONSNUMBER	64	// number max of actions per object
#define PROPSNUMBER	20	// number max of object's properties

#define NEEDLOOPBACK	0	// no loopback for ipmulticast

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
#ifndef RAND_MAX
#define RAND_MAX	32767
#endif

#endif
