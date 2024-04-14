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
#ifndef EVENT_HPP
#define EVENT_HPP


int netTimeout();
/**<
 * Check if some responsibilities must not to be taken when a timeout occurs.
 * Do the heatbeat refreshing.
 * Check if some netobjects must not to be deleted.
 * Returns the delay after which we want to be awaked (ms) before recall.
 * Exported to GUI.
 */

void netIncoming(int fd);
/**<
 * Awake the net when data is available on fd.
 * Exported to GUI.
 */

#endif
