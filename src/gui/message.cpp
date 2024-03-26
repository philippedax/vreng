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
/*
 *  message.cpp : Ubit messages for the VREng GUI
 *
 *  VREng / Ubit Project
 *  Authors: Eric Lecolinet and Eric Varadaradjou
 *
 *  Ubit Toolkit: A Brick Construction Game Model for Creating GUIs
 *  Please refer to the Ubit GUI Toolkit Home Page for details.
 *
 *  (C) 2002/2009 Eric Lecolinet - ENST Paris
 *  WWW: http://www.enst.fr/~elc/ubit
 */
#include "vreng.hpp"
#include "message.hpp"
#include "widgets.hpp"	// Widgets gw
#include "theme.hpp"	// theme
#include "wobject.hpp"	// WO
#include "user.hpp"	// userWriting
#include "render.hpp"	// setCameraScissor
#include "cache.hpp"	// check
#include "pref.hpp"	// user
#include "icon.hpp"	// user
#if HAVE_OCAML
#include "aiinit.hpp"	// read_request
#endif
#include <iostream>
#include "ubit/ubit.hpp"


/* ==================================================== ======== ======= */
// MESSAGES and REQUESTS

Message::Message(Widgets* _gw) :
 gw(*_gw)
{
  mess_box.addAttr(UOrient::vertical + UFont::small + utop());
  mess_box.add(uhbox(UFont::bold + UColor::orange + "Have fun with VREng ;-)"));
  mess_history.push_back(ustr(""));
  mess_history_pos = 0;
}

/** Creates the input query box */
UBox& Message::createQuery()
{
  UTextfield& input = utextfield(usize(250)
                    + mess_text
                    + ucall(this, &Message::inputCB));
  UBox& query =  uhbox(ulabel(UFont::bold + "input: ")
               + uhflex()
               + input
               + uright()
               + uitem(  utip("Clear input")
                       + UFont::bold + "C"
                       + uassign(mess_text, "")
                      )
               + uitem(  utip("Previous message")
                       + USymbol::up
                       + ucall(this, -1, &Message::getHistoryCB)
                      )
               + uitem(  utip("Next message")
                       + USymbol::down
                       + ucall(this, +1, &Message::getHistoryCB)
                      )
              );
  input.addAttr(UBackground::none + UColor::white);
  return query;
}

/** Creates the message panel */
UBox& Message::createMessagePanel(bool transparent)
{
  mess_scrollpane.add(mess_box);
  mess_scrollpane.showVScrollButtons(false);
  mess_scrollpane.getHScrollbar()->show(false);

  UTextfield& input = utextfield(usize(250)
                    + mess_text
                    + ucall(this, &Message::inputCB));
#if 0 //dax
  UBox& query =  uhbox(ulabel(UFont::bold + "input: ")
               + uhflex()
               + input
               + uright()
               + uitem(  utip("Clear input")
                       + UFont::bold + "C"
                       + uassign(mess_text, "")
                      )
               + uitem(  utip("Previous message")
                       + USymbol::up
                       + ucall(this, -1, &Message::getHistoryCB)
                      )
               + uitem(  utip("Next message")
                       + USymbol::down
                       + ucall(this, +1, &Message::getHistoryCB)
                      )
              );
#endif
  UBox& notif = ubox(UOrient::vertical
                     + uhflex()
                     + uvflex()
                     + mess_scrollpane
                     + ubottom()
                     //dax + query
                    );
  if (transparent) {
    input.addAttr(UBackground::none + UColor::white);
    notif.addAttr(UColor::white);
  }
  return notif;
}

void Message::inputCB()
{
  if (mess_text.empty()) return;

  mess_history.push_back(mess_text);	// save text into history

  if (mess_text[0] == '!') {		// starts with a '!'
    performRequest(mess_text);
  }
  else {
    writeMessage("chat", g.user, mess_text.c_str());	// display in message zone
    User::userWriting(mess_text.c_str());		// send to localuser
  }
  mess_text = "";			// clear textfield
}

void Message::getHistoryCB(int go)
{
  mess_history_pos = (mess_history.size() + mess_history_pos + go) % mess_history.size();
  mess_text = mess_history[mess_history_pos];
}

void Message::performRequest(const UStr& req)	// req starts with a '!'
{
#if HAVE_OCAML
  if (req.empty() || !isalpha(req[1])) return;

  const char* req_chars = req.c_str() + 1;	// skip the initial '!'
  mess_text = "";
  uint8_t rr = read_request(req_chars);

  if (rr > 0) {
    nclicks = rr;
    for (int i=0; i<7 ;i++) {
      clicked[i] = MAXFLOAT;
    }

    char *sentence = strdup(req_chars);
    char *brkt = null;
    char *p = strtok_r(sentence, " ", &brkt);
    while (p) {
      mess_text.append(p);
      mess_text.append(" ");	// should be with red color
      p = strtok_r(NULL, " ", &brkt);
    }
    free(sentence);
  }
  else {
    g.gui.writeMessage("request", null, req_chars); // display in request history
    mess_text.append(req);
  }
#endif //HAVE_OCAML
}

void Message::performRequest(WO *object)
{
  if (object && nclicks > 0) {
    clicked[0]=object->pos.x;
    clicked[1]=object->pos.y;
    clicked[2]=object->pos.z;
    clicked[3]=object->pos.az;
    clicked[4]=object->pos.bbs.v[0];
    clicked[5]=object->pos.bbs.v[1];
    clicked[6]=object->pos.bbs.v[2];
    nclicks--;
    if (! mess_text.empty()) performRequest(mess_text);
  }
}

void Message::initClicked()
{
  nclicks = 0;

  for (int i=0; i < sizeof(clicked)/sizeof(float) ; i++) {
    clicked[i] = MAXFLOAT;
  }
}

void Message::getClicked(uint8_t *_nclicks, float _clicked[])
{
  *_nclicks = nclicks;

  for (int i=0; i < sizeof(clicked)/sizeof(float) ; i++) {
    _clicked[i] = clicked[i];
  }
}

static bool string2Coord(const char* positions, float& x, float& y, float& z, float& az)
{
  char *positions_copy = strdup(positions);
  char *p = null, *brkt = null;

  x = y = z = az = 0;
  p = strtok_r(positions_copy, ",", &brkt);
  if (p) x = static_cast<float>(atof(p)); else goto endstr;
  p = strtok_r(NULL, ",", &brkt);
  if (p) y = static_cast<float>(atof(p)); else goto endstr;
  p = strtok_r(NULL, ",", &brkt);
  if (p) z = static_cast<float>(atof(p)); else goto endstr;
  p = strtok_r(NULL, ",", &brkt);
  if (p) az = static_cast<float>(atof(p)); else goto endstr;

  free(positions_copy);
  return true;

endstr:
  free(positions_copy);
  return false;
}

// move the satellite camera at wanted position
static void moveSatCamera(char* pos)
{
  float x, y, z, az;

  string2Coord(pos, x, y, z, az);
  g.render.setCameraScissor(x, y, z, az);
}

void Message::convertTextToLink(const string& text, char **listeObjets, int size)
{
  UElem* allmsgs = new UElem();
  bool found = false;
  char* mess = strdup(text.c_str());

  if (size < 1) {
    cerr << "convertTextToLink not implemented" << endl;
    mess_box.add(uhbox(ugroup(mess)));
  }
  char *brkt = null;
  char *tmpmess = strtok_r(mess, " ", &brkt);

  while (tmpmess) {
    found = false;
    for (int i=0; i<size ; i=i+3) {

      if (! listeObjets[i])
        break;
      if ((! strcasecmp(listeObjets[i], tmpmess)) && (listeObjets[i+1])) {
        UIma& uimg = uima(listeObjets[i+1]);	// loads image
        uimg.rescale(0.25);
        ULinkbutton& ulinkb =
        ulinkbutton(  listeObjets[i]
                    + UColor::green + UFont::bold
                    + umenu(ulabel(uimg))
                    + UOn::doubleClick / ucall(reinterpret_cast<char*>(listeObjets[i+2]), moveSatCamera)
                    );
        allmsgs->add(ulinkb);
        allmsgs->add(ustr(" "));
        found = true;
        unlink(listeObjets[i+1]);	// unlink tmp file
        break;
      }
    }
    if (! found) {
      allmsgs->add(ugroup(tmpmess));
      allmsgs->add(ustr(" "));
    }
    tmpmess = strtok_r(NULL, " ", &brkt);
  }
  mess_box.add(uhbox(ustr("augmented msg>") + allmsgs));
  free(mess);
}

void Message::postRequest(const string& mess, string& result)
{
  int sizemax = 256;
  float posx, posy, posz, posaz; //coordonnees trouvees
  int afficher = 0;
  int nbobjs = 0;
  int sizerecu = mess.length();
  char pos[sizerecu +1];
  char name[sizerecu +1];
  char tmp[sizerecu +1];
  char **listeObjets = new char*[sizemax];

  pos[0] = name[0] = tmp[0] = '\0';

  for (int i=0; i < sizerecu ; i++) {
    if (mess[i] == '@') {
      if (nbobjs > (sizemax-2)) {
        cerr << "message has too many request" <<endl;
        break;
      }
      if (afficher == 0) {
        pos[0] = '\0';
        name[0] = '\0';
      }
      else if (afficher == 1) {
        listeObjets[nbobjs] = strdup(name);
        nbobjs++;
      }
      else if (afficher == 2) {
        //on enregistre l'image ds un fichier.
        listeObjets[nbobjs] =  strdup("request.jpg");
        listeObjets[nbobjs+1] = strdup(pos);

        //on convertit le string en coords.
        string2Coord(pos, posx, posy, posz, posaz);

        //recupere la position et fait un snapshot de la position demandee	
        ::g.render.calculateFov(posx,posy,posz,posaz, listeObjets[nbobjs]);
        nbobjs += 2;
      }
      afficher = (afficher+1) % 3;
      continue;
    }
    else { 		//not @
      if (afficher == 0) {
        result += mess[i];
      }
      else if (afficher == 1) {
        result += mess[i];
	strcpy(tmp, name);
        sprintf(name, "%s%c", tmp, mess[i]);
      }
      else if (afficher == 2) {
	strcpy(tmp, pos);
        sprintf(pos, "%s%c", tmp, mess[i]);
      }
    }
  }

  if (nbobjs > 0) convertTextToLink(result, listeObjets, nbobjs);
  //TODO ev : traiter le message non augmente

  if (listeObjets) delete[] listeObjets;
  listeObjets = NULL;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// chat, warning, request, notice...

void Message::writeMessage(const char* mode, const char* from, const char* msg)
{
  if (! msg)  return;

  uptr<UStr> mess_text = null;
  uptr<UStr> prefix = null;
  uptr<UColor> prefix_color = null;

  if (from) {		// chat
    string result;
    postRequest(msg, result);
    mess_text = new UStr(result);

    prefix = new UStr("from ");
    prefix->append(from);
    prefix->append("> ");
    prefix_color = &::g.theme.chatColor;
  }
  else if (mode) {	// internal
    mess_text = new UStr(msg);

    if (! strcmp(mode, "notice")) {		// notice
      prefix = new UStr(mode);
      prefix->append("> ");
      prefix_color = ::g.theme.noticeColor;
    }
    else if (! strcmp(mode, "progress")) {	// progress
      prefix = new UStr("");
      prefix_color = ::g.theme.noticeColor;
      mess_box.add(uhbox(ugroup(UFont::bold + prefix_color) + mess_text));
    }
    else if (! strcmp(mode, "request")) {	// requext
      prefix = new UStr(mode);
      prefix->append("> ");
      prefix_color = ::g.theme.requestColor;
    }
    else {					// warning
      prefix = new UStr(mode);
      prefix->append("> ");
      prefix_color = ::g.theme.warningColor;
    }
  }
  else {		// chat to
    error("msg: %s", msg);
    mess_text = new UStr(msg);
    prefix = new UStr("> ");
    prefix_color = ::g.theme.messColor;
  }

  mess_box.add(uhbox(ugroup(UFont::bold + prefix_color + prefix) + mess_text));
  return;
}

/* ==================================================== ======== ======= */

void Message2::putIconCB()
{
  char infos[128];

  memset(infos, 0, sizeof(infos));
  if (url.c_str()) {
    strcat(infos, "<url=\"");
    strcat(infos, url.c_str());
  }
  if (file.c_str()) {
    strcat(infos, "<file=\"");
    strcat(infos, file.c_str());
  }
  if (ofile.c_str()) {
    strcat(infos, "<ofile=\"");
    strcat(infos, ofile.c_str());
  }
  if (name.c_str()) {
    strcat(infos, "<name=\"");
    strcat(infos, name.c_str());
  }
  if (icon.c_str()) {
    strcat(infos, "<icon=\"");
    strcat(infos, icon.c_str());
  }
  strcat(infos, "\">&");
  doAction(ICON_TYPE, Icon::CREATE, localuser, infos, 0, 0);
}

void Message2::putIcon(const UStr& val)
{
  doAction(ICON_TYPE, Icon::CREATE, localuser, (void*)val.c_str(), 0, 0);
}
