//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
  
  uint16_t width3D;	///< 3D width
  uint16_t height3D;	///< 3D height
  uint8_t maxsimcon;	///< max simultaneous connections
  uint8_t maxfps;	///< max frames/sec
  uint32_t dbg;		///< debug mask
  uint32_t cachetime;	///< live time of the cache
  time_t frame_delay;	///< inter frame delay
  bool new_universe;    ///< if new universe asked
  bool quality3D;	///< 3D quality or not
  bool refresh;		///< refresh cache
  bool gravity;		///< with/without gravity
  bool cpulimit;	///< CPU limitation
  bool infogl;		///< OpenGL infos
  bool reflector;	///< unicast/multicast reflector
  bool fast;		///< no persistency if true
  bool bbox;		///< draw bounding-boxes
  bool silent;		///< audio silent
  bool dbgtrace;	///< trace

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
  char *httpproxystr;
  char *noproxystr;
  char *mcastproxystr;

  void parse(int argc, char **argv);
  /**< Parses options in command line */
  
  void initEnvPrefs(const char* pref_file);
};

#endif
