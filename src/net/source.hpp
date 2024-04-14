//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
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
#ifndef SOURCE_HPP
#define SOURCE_HPP

#include "rtp.hpp"


/**
 * Source class
 * Informations on a RTP source
 */
class Source {
 private:
  //notused static uint16_t getMembersNumber();
  //notused static void dumpAll();
  //notused void dump();

 public:
  sourceInfos	s;		///< source info
  uint32_t	ssrc;		///< ssrc id
  class Source	*next;		///< next source
  uint32_t	extended_max;	///< extented max
  uint32_t	expected;	///< expected packets
  uint32_t	lost;		///< lost packets
  rtcp_sr_t	sr;		///< sender report
  rtcp_rr_t	rr;		///< receiver report
  SdesItem	sdes;		///< sdes from the source, if we received it

  Source(uint32_t ssrc);	/**< Constructor: creates a new source */

  virtual ~Source();		/**< destructor */

  static Source * getSource(uint32_t ssrc);
  static uint16_t getSourcesNumber();
  static uint16_t incrSourcesNumber();
  static uint16_t decrSourcesNumber();
};

#endif
