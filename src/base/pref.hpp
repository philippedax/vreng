//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// pref.hpp
//---------------------------------------------------------------------------
#ifndef PREF_HPP
#define PREF_HPP


/**
 * Pref class
 */
class Pref {
 public:

  Pref();
  /**< constructor */
  
  void init(int argc, char **argv, const char* pref_file);
  /**< Init preferences */
  
  uint32_t debug;       ///< debug flags.
  uint16_t width3D;	///< 3D width
  uint16_t height3D;	///< 3D height
  uint8_t maxsimcon;	///< max simultaneous connections
  uint8_t maxfps;	///< max frames/sec
  uint32_t dbg;		///< debug mask
  uint32_t cachetime;	///< live time of the cache
  time_t frame_delay;	///< inter frame delay
  bool quality3D;	///< 3D quality or not
  bool refresh;		///< refresh cache
  bool gravity;		///< with/without gravity
  bool cpulimit;	///< CPU limitation
  bool infogl;		///< OpenGL infos
  bool keep;		///< keep all in cache
  bool reflector;	///< unicast/multicast reflector
  bool expand;		///< palettes collapsed by default
  bool nopersist;	///< no persistency if true
  bool bbox;		///< draw bounding-boxes
  bool silent;		///< audio silent
  bool progress;	///< show progression
  bool stats;		///< show stats
  bool trace;		///< show trace
  bool loghttpd;	///< show loghttpd
  bool tview;		///< thirdpersonn view
  bool sql;		///< trace sql table

  /* preferences */
  char *my_avatar;	///< avatar model
  char *my_vrestr;
  char *my_widthstr;
  char *my_depthstr;
  char *my_heightstr;
  char *my_mapfrontstr;
  char *my_mapbackstr;
  char *my_hoststr;
  char *my_webstr;
  char *my_facestr;
  char *my_bapsstr;
  char *my_sexstr;
  char *my_headstr;
  char *my_skinstr;
  char *my_buststr;
  char *my_colorstr;
  char *my_skinf;
  char *my_skinb;
  char *httpproxystr;
  char *noproxystr;
  char *mcastproxystr;
  char *sqltable;

 private:
  char *stdcpp;

  void parse(int argc, char **argv);
  /**< Parses options in command line */
};

#endif
