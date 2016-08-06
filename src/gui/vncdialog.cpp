//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://vreng.enst.fr/
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
/*
 *  vncdialog.cpp : cnc dialog functions for the VREng GUI
 *  NOTE: this file should be common to all GUI variants
 *
 *  VREng Project / Ubit GUI
 *  Authors: Eric Lecolinet and Philippe Dax
 *
 *  (C) 1999-2008 Eric Lecolinet / ENST Paris
 *  WWW: http://www.enst.fr/~elc/ubit
 */
#include "vreng.hpp"
#include "widgets.hpp"
#include "vncdialog.hpp"
#include "vnc.hpp"

VncDialog* VncDialog::vnc_dialog = null;

void VncDialog::create(GuiWidgets* gw, Vnc* vnc)
{
  if (!gw) return;
  if (!vnc_dialog) {
    vnc_dialog = new VncDialog(gw, vnc);
    gw->add(vnc_dialog);
    vnc_dialog->centerOnScreen();
  }
  vnc_dialog->show(true);
}

VncDialog::VncDialog(GuiWidgets* _gw, Vnc* _vnc) : vnc(_vnc)
{
  vnc_port = "5901";
  setTitle("VNC Server");
  setMessage(uvbox(uhflex() + uvflex()
                   + uhbox(ulabel(14, UFont::bold + "Server name:")
                           + utextfield(25, vnc_server))
                   + uhbox(ulabel(14, UFont::bold + "Port number:")
                           + utextfield(vnc_port))
                   + uhbox(ulabel(14, UFont::bold + "Password:")
                           + utextfield(vnc_passwd))
                   ));
  setButtons(ubutton("Connect" + ucloseWin() + ucall(vnc_dialog, &VncDialog::convert))
             + " "
             + ubutton("Cancel" + ucloseWin()));
  }

void VncDialog::convert()
{
  if (vnc) vnc->convert(vnc_server.c_str(), vnc_port.c_str(), vnc_passwd.c_str());
}

