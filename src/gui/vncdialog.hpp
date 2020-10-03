//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
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
#ifndef VNCDIALOG_HPP
#define VNCDIALOG_HPP

#include <ubit/ustr.hpp>

/**
 * VncDialog class
 */
class VncDialog : public UOptionDialog {
public:
  static void create(Widgets*, class Vnc*);
 
private:
  static VncDialog* vnc_dialog;
  class Vnc* vnc;	///< vnc pointer.

  UStr vnc_server;	///< vnc server string.
  UStr vnc_port;	///< vnc port string.
  UStr vnc_passwd;	///< vnc passwd string.
  
  VncDialog(class Widgets*, class Vnc*);

  virtual void convert();
};

#endif
