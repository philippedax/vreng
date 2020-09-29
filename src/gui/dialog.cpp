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
/*
 *  dialog.cpp : Ubit dialogs for the VREng GUI
 *
 *  VREng / Ubit Project
 *  Authors: Eric Lecolinet and Philippe Dax
 *
 *  Ubit Toolkit: A Brick Construction Game Model for Creating GUIs
 *  Please refer to the Ubit GUI Toolkit Home Page for details.
 *
 *  (C) 2002/2009 Eric Lecolinet - ENST Paris
 */
#include "vreng.hpp"
#include "env.hpp"
#include "widgets.hpp"
#include "theme.hpp"
#include "wobject.hpp"	// WObject
#include "universe.hpp"	// current
#include "world.hpp"	// current
#include "universe.hpp"	// MAX_WORLDS
#include "app.hpp"	// TOOLS
#include "http.hpp"	// httpOpen
#include "grid.hpp"	// buildBox
#include "file.hpp"	// openFile
#include "cache.hpp"	// openCache


/** Displays preferences */
UDialog& GuiWidgets::createPrefsDialog()
{
  UBox& settings_box = uvbox(UBackground::white + upadding(2,2));

  UDialog* prefs_dialog =
  new UOptionDialog("Preferences",
                    uvbox(ulabel("Preferences: " + UColor::red + ::g.env.prefs())
                          + uvflex()
                          + uscrollpane(usize(400,300) + settings_box)),
                    UArgs::none, UArgs::none);

  FILE *fpprefs;
  char bufprefs[128];
  if ((fpprefs = File::openFile(::g.env.prefs(), "r"))) {
    while (fgets(bufprefs, sizeof(bufprefs), fpprefs)) {
      if (isalpha(*bufprefs)) settings_box.add(uitem(UColor::red + UFont::bold + bufprefs));
      else                    settings_box.add(uitem(UColor::black + bufprefs));
    }
    File::closeFile(fpprefs);
  }
  return *prefs_dialog;
}

/** Displays vre source */
static void sourceHttpReader(void *box, Http *http)
{
  if (! http) return;
  UBox *source_box = (UBox *) box;
  char line[BUFSIZ];

  source_box->setAutoUpdate(false);

  FILE *fpcache = Cache::openCache(http->url, http);
  while (fgets(line, sizeof(line), fpcache))
    source_box->add(uitem(UColor::black + line));
  source_box->setAutoUpdate(true);
  source_box->update();
}

/** Displays list of worlds */
static void destinationsHttpReader(void *box, Http *http)
{
  if (! http) return;
  UBox *universe_box = (UBox *) box;
  char line[URL_LEN + CHAN_LEN +2];

  for (int i=0 ; http->nextLine(line) && i < MAX_WORLDS ; i++) {
    UStr& url = ustr();

    //printf("line= %s\n", line);
    char tmpline[URL_LEN + CHAN_LEN +2];
    char tmpname[URL_LEN + 2];
    strcpy(tmpline, line);
    char *p = strchr(tmpline, ' ');
    if (p) *p = '\0';
    else {
      p = strchr(tmpline, '\t');
      if (p) *p = '\0';
    }
    strcpy(tmpname, tmpline);
    char *name = strrchr(tmpname, '/');
    if (name) *name++ = '\0';
    p = strchr(name, '.');
    if (p) *p = '\0';
    url = tmpline;

    universe_box->add(UBackground::black //+ ualpha(0.5)
                      + uitem(UBackground::none + UColor::green + UFont::bold + UFont::large + name
                      + ucall(&g.gui, (const UStr&)url, &Gui::gotoWorld))
                     );
  }
}

/** Displays list of worlds */
static void universeHttpReader(void *box, Http *http)
{
  if (! http) return;
  UBox *universe_box = (UBox *) box;
  char line[URL_LEN + CHAN_LEN +2];

  for (int i=0 ; http->nextLine(line) && i < MAX_WORLDS ; i++) {
    UStr& url = ustr();
    UStr& chan = ustr();

    //printf("line: %s\n", line);
    char tmpline[URL_LEN + CHAN_LEN +2];
    strcpy(tmpline, line);
    char *p = strchr(tmpline, ' ');
    if (p) *p = '\0';
    else {
      p = strchr(tmpline, '\t');
      if (p) *p = '\0';
    }
    if (p) chan = ++p;
    url = tmpline;

    universe_box->add(uitem(UColor::navy + UFont::bold + url
                            + " " + UFont::plain + chan
                            + ucall(&g.gui, (const UStr&)url, &Gui::gotoWorld))
                      );
  }
}

/** Dialog box for vre source */
void GuiWidgets::openSourceDialog()
{
  World *world = World::current();
  if (! world) return;

  UBox& box = uvbox(UBackground::white);
  if (Http::httpOpen(world->getUrl(), sourceHttpReader, &box, -1) < 0) { // -1 = no cache
    delete &box;
    return;
  }
  source_dialog.setMessage(uscrollpane(usize(450,350) + UBackground::white + box));
  source_dialog.show();
}

/** Dialog box for worlds list */
void GuiWidgets::destinationsDialog()
{
  char universe_url[URL_LEN];
  char str[URL_LEN];
  if (!strncmp(Universe::current()->name, "http://", 7))
    sprintf(str, "%s", "%s%s/vacs/v%d/worlds");
  else
    sprintf(str, "%s%s", "http://", "%s%s/vacs/v%d/worlds");
  sprintf(universe_url, str, Universe::current()->name, Universe::current()->urlpfx, Universe::current()->version);
  UBox& box = uvbox(g.theme.scrollpaneStyle);
  if (Http::httpOpen(universe_url, destinationsHttpReader, &box, 0) < 0) {
    delete &box;
    return;
  }
  printf("destinations: universe=%s name=%s pfx=%s : open OK\n", universe_url, Universe::current()->name, Universe::current()->urlpfx);
  worlds_dialog.setMessage(uscrollpane(usize(120,400) + box));
  worlds_dialog.show();
}

/** Dialog box for worlds list */
void GuiWidgets::openWorldsDialog()
{
  char universe_url[URL_LEN];
  sprintf(universe_url, "%s%s/vacs/v%d/worlds",
                      Universe::current()->name,
                      Universe::current()->urlpfx,
                      Universe::current()->version);

  UBox* box = new UTextarea;
  if (Http::httpOpen(universe_url, universeHttpReader, box, 0) < 0) {
    delete box;
    return;
  }
  worlds_dialog.setMessage(uscrollpane(usize(450,350) + box));
  worlds_dialog.show();
}

UDialog& GuiWidgets::createSettingsDialog()
{
  URadioSelect
    &sel_audio_live      = uradioSelect(),
    &sel_video_live      = uradioSelect(),
    &sel_wb              = uradioSelect(),
    &sel_browser         = uradioSelect(),
    &sel_audio_streaming = uradioSelect(),
    &sel_video_streaming = uradioSelect(),
    &sel_modeler         = uradioSelect(),
    &sel_pspdf           = uradioSelect(),
    &sel_office          = uradioSelect(),
    &sel_session         = uradioSelect();

  return udialog
  (
   uhbox("Audio live        : " + UFont::plain
           + ucheckbox("Rat" + sel_audio_live
                       + UOn::select / ucall(this, RAT_TOOL, &GuiWidgets::setPrefCB))
                       .setSelected()
           + ucheckbox("Vat" + sel_audio_live
                       + UOn::select / ucall(this, VAT_TOOL, &GuiWidgets::setPrefCB))
           + ucheckbox("Fphone" + sel_audio_live
                       + UOn::select / ucall(this, FPHONE_TOOL, &GuiWidgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Video live      : " + UFont::plain
           + ucheckbox("Vic" + sel_video_live
                       + UOn::select / ucall( this, VIC_TOOL, &GuiWidgets::setPrefCB))
                       .setSelected()
           + ucheckbox("Vlc" + sel_video_live
                       + UOn::select / ucall(this, VLCMC_TOOL, &GuiWidgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Whiteboard      : " + UFont::plain
           + ucheckbox("Wb" + sel_wb
                       + UOn::select / ucall(this, WB_TOOL, &GuiWidgets::setPrefCB))
                       .setSelected()
           + ucheckbox("Wbd" + sel_wb
                       + UOn::select / ucall(this, WBD_TOOL, &GuiWidgets::setPrefCB))
           + ucheckbox("Nte" + sel_wb
                       + UOn::select / ucall(this, NTE_TOOL, &GuiWidgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Browser         : " + UFont::plain
           + ucheckbox("Firefox" + sel_browser
                       + UOn::select / ucall(this, FIREFOX_TOOL, &GuiWidgets::setPrefCB))
                       .setSelected()
           + ucheckbox("Chrome" + sel_browser
                       + UOn::select / ucall(this, CHROME_TOOL, &GuiWidgets::setPrefCB))
           + ucheckbox("Safari" + sel_browser
                       + UOn::select / ucall(this, SAFARI_TOOL, &GuiWidgets::setPrefCB))
           + ucheckbox("Opera" + sel_browser
                       + UOn::select / ucall(this, OPERA_TOOL, &GuiWidgets::setPrefCB))
           + ucheckbox("I-Explorer" + sel_browser
                       + UOn::select / ucall(this, IEXPLORER_TOOL, &GuiWidgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Audio streaming : " + UFont::plain
           + ucheckbox("Vlc" + sel_video_streaming
                       + UOn::select / ucall( this, VLC_TOOL, &GuiWidgets::setPrefCB)
                       ).setSelected()
           + ucheckbox("Mpg123" + sel_audio_streaming
                       + UOn::select / ucall(this, MPG123_TOOL, &GuiWidgets::setPrefCB))
           + ucheckbox("Freeamp" + sel_audio_streaming
                       + UOn::select / ucall(this, FREEAMP_TOOL, &GuiWidgets::setPrefCB))
           + ucheckbox("Quicktime" + sel_audio_streaming
                       + UOn::select / ucall(this, QUICKTIME_TOOL, &GuiWidgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Video streaming : " + UFont::plain
           + ucheckbox("Vlc" + sel_video_streaming
                       + UOn::select / ucall( this, VLC_TOOL, &GuiWidgets::setPrefCB))
                       .setSelected()
           + ucheckbox("Mpegplay" + sel_video_streaming
                       + UOn::select / ucall(this, MPEGPLAY_TOOL, &GuiWidgets::setPrefCB))
           + ucheckbox("Quicktime" + sel_video_streaming
                       + UOn::select / ucall(this, QUICKTIME_TOOL, &GuiWidgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Modeler         : " + UFont::plain
           + ucheckbox("Vred" + sel_modeler
                       + UOn::select / ucall(this, VRED_TOOL, &GuiWidgets::setPrefCB))
                       .setSelected()
           + ucheckbox("Vrem" + sel_modeler
                       + UOn::select / ucall(this, VREM_TOOL, &GuiWidgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("PsPdf           : " + UFont::plain
           + ucheckbox("Evince" + sel_pspdf
                       + UOn::select / ucall(this, EVINCE_TOOL, &GuiWidgets::setPrefCB))
                       .setSelected()
           + ucheckbox("Gv" + sel_pspdf
                       + UOn::select / ucall(this, GV_TOOL, &GuiWidgets::setPrefCB))
           + ucheckbox("Ghostscript" + sel_pspdf
                       + UOn::select / ucall(this, GHOSTVIEW_TOOL, &GuiWidgets::setPrefCB))
           + ucheckbox("Acrobat" + sel_pspdf
                       + UOn::select / ucall(this, ACROBAT_TOOL, &GuiWidgets::setPrefCB))
           + ucheckbox("Xpdf" + sel_pspdf
                       + UOn::select / ucall(this, XPDF_TOOL, &GuiWidgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Office          : " + UFont::plain
           + ucheckbox("LibreOffice" + sel_office
                       + UOn::select / ucall(this, LIBROFFICE_TOOL, &GuiWidgets::setPrefCB))
                       .setSelected()
           + ucheckbox("MsOffice" + sel_office
                       + UOn::select / ucall(this, MSOFFICE_TOOL, &GuiWidgets::setPrefCB))
           + ucheckbox("OpenOffice" + sel_office
                       + UOn::select / ucall(this, OPENOFFICE_TOOL, &GuiWidgets::setPrefCB))
           + ucheckbox("StarOffice" + sel_office
                       + UOn::select / ucall(this, STAROFFICE_TOOL, &GuiWidgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Session         : " + UFont::plain
           + ucheckbox("Ssh" + sel_session
                       + UOn::select / ucall(this, SSH_TOOL, &GuiWidgets::setPrefCB)
                       ).setSelected()
           + ucheckbox("Telnet" + sel_session
                       + UOn::select / ucall(this, TELNET_TOOL, &GuiWidgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + ubutton(UFont::bold + uhcenter() + " Close " + ucloseWin())
   );
}

UDialog& GuiWidgets::createGridDialog()
{
  return udialog(Grid::grid()->buildBox());
}

UDialog& GuiWidgets::createToolDialog()
{
  return udialog
  (  ubutton("Audio"      + ucall(this, true, &GuiWidgets::setAudioCB))
   + ubutton("Video"      + ucall(this, true, &GuiWidgets::setVideoCB))
   + ubutton("Whiteboard" + ucall(this, true, &GuiWidgets::setWhiteboardCB))
   + ubutton("Modeler"    + ucall(this, true, &GuiWidgets::setModelerCB))
   + uhbox(UBorder::shadowIn)
   + ubutton(UFont::bold + uhcenter() + " Close " + ucloseWin())
   );
}
