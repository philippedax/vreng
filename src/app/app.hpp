//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
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
#ifndef APP_HPP
#define APP_HPP


/** masks */
#define AUDIO_MASK	0x00000003
#define VAT_TOOL	0x00000001
#define RAT_TOOL	0x00000002
#define FPHONE_TOOL	0x00000003

#define XTERM_MASK	0x00000006
#define TELNET_TOOL	0x00000004
#define SSH_TOOL	0x00020006

#define VIDEO_MASK	0x0000000c
#define VIC_TOOL	0x00000008
#define VLCMC_TOOL	0x0000000c

#define WHITEBOARD_MASK	0x00000070
#define WB_TOOL		0x00000010
#define WBD_TOOL	0x00000020
#define NTE_TOOL	0x00000030
#define MB_TOOL		0x00000040

#define HTML_MASK	0x00000700
#define NETSCAPE_TOOL	0x00000100
#define MOZILLA_TOOL	0x00000200
#define FIREFOX_TOOL	0x00000300
#define SAFARI_TOOL	0x00000400
#define OPERA_TOOL	0x00000500
#define IEXPLORER_TOOL	0x00000600
#define CHROME_TOOL	0x00000700

#define MODELER_MASK	0x00000e00
#define VRED_TOOL	0x00000800
#define VREM_TOOL	0x00000a00
#define VRML_MASK	0x00000c00

#define PDF_MASK	0x00007000
#define ACROBAT_TOOL	0x00001000
#define XPDF_TOOL	0x00002000
#define EVINCE_TOOL	0x00003000
#define PREVIEW_TOOL	0x00004000

#define PS_MASK		0x0000e000
#define GV_TOOL		0x00008000
#define GHOSTVIEW_TOOL	0x0000a000
#define A2PS_TOOL	0x0000c000

#define OFFICE_MASK	0x00070000
#define MSOFFICE_TOOL	0x00010000
#define STAROFFICE_TOOL	0x00020000
#define OPENOFFICE_TOOL	0x00030000
#define LIBROFFICE_TOOL	0x00040000

#define MP3_MASK	0x00700000
#define MPG123_TOOL	0x00100000
#define SDLMIXER_TOOL	0x00200000
#define TIMIDITY_TOOL	0x00300000
#define QUICKTIME_TOOL	0x00400000
#define PLAY_TOOL	0x00500000
#define XAUDIO_TOOL	0x00600000
#define FREEAMP_TOOL	0x00700000

#define MPEG_MASK	0x07000000
#define VLC_TOOL	0x01000000
#define MTVP_TOOL	0x02000000
#define MPEGPLAY_TOOL	0x03000000
#define SDLVIDEO_TOOL	0x04000000

/** paths */
#if MACOSX
#define QUICKTIME_PATH	"/Applications/QuickTime Player.app"
#define SAFARI_PATH	"/Applications/Safari.app"
#define PREVIEW_PATH	"/Applications/Preview.app"
#define CHROME_PATH	"/Applications/Google Chrome.app"
#define VLC_PATH	"/Applications/VLC.app"
#endif

/** ports */
#define AUDIO_INCR_PORT		2 // rtp
#define VIDEO_INCR_PORT		4 // rtp
#define WHITEBOARD_INCR_PORT	6 // udp


/**
 * App class
 * External applications
 */
class App {
 public:

  static void quitTools();
  ///< Quit current tools

  static void quitAllTools();
  ///< Quit all tools

};


#endif
