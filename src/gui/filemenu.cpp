//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
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
#include "vreng.hpp"
#include "widgets.hpp"
#include "theme.hpp"
#include "message.hpp"	// PutInfo


UMenu& GuiWidgets::createFileMenu()
{
  // Open a vreng URL
  UStr& url_or_name = ustr();

  UDialog* openvre_dialog =  // args are: title, content, icon, buttons
  new UOptionDialog("Open Vreng URL",
                     ulabel(UFont::bold + "URL or name: ")
                     // space for 80 chars, shows and edits string 'url_or_name'
                     + uhflex() + utextfield(80, url_or_name),
                     UArgs::none,
                     uhbox(ubutton("  Open  " + ucall(&gui, (const UStr&)url_or_name, &Gui::gotoWorld))
                           + ubutton("  Cancel  " + ucloseWin()))
                     );

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Put & Publish URL
  UBox& puturl_box =
  uvbox(uvspacing(5)
        + uhbox(ulabel(20, UFont::bold + "Url" + UFont::plain + " (required)" )
                + uhflex() + utextfield(65, putinfo.url))
        + uhbox(ulabel(20, UFont::bold + "Alias" + UFont::plain + " (short name)" )
                + uhflex() + utextfield(65, putinfo.name))
        + uhbox(ulabel(20, UFont::bold + "Icon" + UFont::plain + " (optional)" )
                + uhflex() + utextfield(65, putinfo.icon))
        );

  UDialog* puturl_dialog =
  new UOptionDialog("Put new URL document", //title
                    puturl_box,		// message
                    UArgs::none,	// no icon
                    ubutton("  Put & Publish URL  "
                            + ucall(&putinfo,&PutInfo::putIconCB))); //buttons

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Put & Publish File
  UBox& putfile_box =
  uvbox(uvspacing(5)
        + uhbox(ulabel(25, UFont::bold + "Input File" + UFont::plain + " (required)" )
               + uhflex() + utextfield(65, putinfo.file))
        + uhbox( ulabel(25, UFont::bold +"Output File" + UFont::plain + " (public location)" )
                + uhflex() + utextfield(65, putinfo.ofile))
        + uhbox(ulabel(25, UFont::bold + "Alias" + UFont::plain + " (short name)" )
                + uhflex() + utextfield(65, putinfo.name))
        + uhbox(ulabel(25, UFont::bold + "Icon" + UFont::plain + " (optional)" )
                + uhflex() + utextfield(65, putinfo.icon))
        );

  UDialog* putfile_dialog =
  new UOptionDialog("Put new File document",  // title
                    putfile_box,  // message
                    UArgs::none,  // no icon
                    ubutton("  Put & Publish File  "
                            + ucall(&putinfo, &PutInfo::putIconCB)));

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Create File menu
  return umenu(ubutton(g.theme.World  + " Open Vreng URL..." + openvre_dialog)
             + ubutton(g.theme.Save + " Save Vreng File" + ucall(this, &GuiWidgets::saveCB))
             + usepar()
             + ubutton(g.theme.Doc  + " Put & Publish URL..." + puturl_dialog)
             + ubutton(g.theme.Book + " Put & Publish File..." + putfile_dialog)
             + usepar()
             + ubutton(g.theme.Exit + " Quit" + ucall(0/*status*/, Global::quitVreng))
             );
}
