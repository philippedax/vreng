//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://www.vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
#ifndef DEFAULTS_HPP
#define DEFAULTS_HPP

/* default servers */
#if STANDALONE			// by ./configure --enable-standalone
#define IPMC_ENABLED		0	// ipmulticast disabled
#define DEF_HTTP_SERVER		"localhost"
#define DEF_VJS_SERVER		"localhost"
#define DEF_VAPS_SERVER		"localhost"
#define DEF_VACS_SERVER		"localhost"
#define DEF_VRUM_SERVER		"localhost"
#define DEF_MYSQL_SERVER	"localhost"
#define DEF_PGSQL_SERVER	"localhost"
#define DEF_VNC_SERVER		"localhost"

#else	// by default

#define IPMC_ENABLED		1	// ipmulticast enabled
#define DEF_HTTP_SERVER		"localhost"
				// was "www.vreng.enst.fr" but recently unreachable
				// because it is no more maintained today
				// the httpd server and its datas associated
				// should be well known at a stable location (url)
				// and so reachable for everybody
				// allowing multi-users behavior
				// localhost is not the ideal solution (mono-user)
				// localhost allows to run simply autonomously.
				// see also DEF_URL_PFX witch can be changed
#define DEF_VJS_SERVER		"localhost"
#define DEF_VAPS_SERVER		"localhost"
#define DEF_VACS_SERVER		"localhost"
#define DEF_VRUM_SERVER		"localhost"
#define DEF_MYSQL_SERVER	"localhost"
#define DEF_PGSQL_SERVER	"localhost"
#define DEF_VNC_SERVER		"localhost"
#endif	// def servers

#define VRE_VERSION	8	// current stable version of vre files

/* default URLs */
//#define GEN_URL(h, u, p) 	("http://" h u p) // notused
#if !defined(DEF_URL_PFX)	// by ./configure --with-httpd-prefix

// Url prefix allowing access to the htdocs location
// default url prefix: can be changed to be adapted to your site.
#define DEF_URL_PFX	"~%s/vreng"	// <-- s is the loginname : to customize !!!
				// prefix to tune, should be "" (old prefix)
				// it is the HTDOCS equivallent for your
				// local httpd server (after http://httpserver/)
				// for example :
				//   if http://httpserver/~login/vreng the location of data is
				//     ~/public_html/vreng (for Linux) or 
				//     ~/Sites/vreng (for MacOS)
				// where "vreng" is the same as "htdocs/" directory
				// in this distribution.
#endif

// these paths are relative to the htdocs directory
#define DEF_URL_WORLD	"/vre/Rendezvous.vre"		// location: /vre/Rendezvous.vre
#define DEF_URL_WORLD_BAK "/vre/v8/Rendezvous.vre"	// alt location: /vre/v8/Rendezvous.vre
#define DEF_URL_WORLDS	"/vre/worlds"			// list of worlds
#define DEF_URL_FRONT	"/gif/default.gif"		// for user box
#define DEF_URL_BACK	"/gif/default.1.gif"		// for user box
#define DEF_URL_BALL	"/gif/ball.gif"			// ball by default
#define DEF_URL_TXF	"/txf/helvetica-br-24.txf"	// txf font by default
#define DEF_URL_PAPER	"/jpg/paper.jpg"		// paper for book
#define DEF_URL_NOISE	"/mp3/sheet.mp3"		// noise for sheet
#define DEF_URL_JAR	"/jar/vrengapp.jar"		// jar vjs server

/* default IPmulticast addresses & ports */
#define DEF_VRENG_MADDR		"224.255.0.0"
#define DEF_VRENG_PORT		52656
#define DEF_VRENG_TTL		127
#define DEF_VRENG_CHANNEL	"224.255.0.1/52666/127"
#define DEF_MANAGER_CHANNEL	"224.255.0.0/52656/127"
#define DEF_VACS_PORT		4150 + VRE_VERSION	// port of vacs server
#define DEF_SAP_ADDR		"224.2.127.254"		// address of Session Address Protocol
#define DEF_SAP_PORT		9875			// port of Session Address Protocol
#define DEF_HTTP_PORT		80			// port of http server
#define DEF_VNC_PORT		5900			// port of Vnc server
#define DEF_VAPS_PORT		4150			// port of Vaps server
#define DEF_VJS_PORT		4151			// port of Vjs server

#define DEF_RATE		50		// 50 frames/sec
#define DEF_RATIO3D		(16./9.)	// screen ratio 16/9
#define DEF_WIDTH3D		960 		// screen width
#define DEF_HEIGHT3D		540 		// screen height
#define DEF_MAXSIMCON		8		// MAX simultaneous threaded connections
#define DEF_MAXFRAMES		255		// MAX frames
#define DEF_REFRESH_TIMEOUT	1.5		//orig: 1.5

#define PATH_LEN		128		// max lenght of path file
#define CHAN_LEN		32		// max length of a channel
#define GROUP_LEN		16		// max length of a group
#define URL_LEN			256		// max length of an url
#define USER_LEN		9		// max lenght of user name
#define OBJNAME_LEN		64		// max lenght of object name
#define HNAME_LEN		11		// max length of understandable name
#define ACTIONNAME_LEN		16		// max length of action name
#define ACTIONSNUMBER		64		// number max of actions per object
#define PROPSNUMBER		20		// number max of object's properties
#define NEEDLOOPBACK		0		// no loopback for ipmulticast

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
#ifndef RAND_MAX
#define RAND_MAX	32767
#endif

#endif
