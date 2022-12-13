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
#include "wobject.hpp"	// WObject
#include "user.hpp"	// userWriting
#include "render.hpp"	// setCameraScissor
#include "cache.hpp"	// check
#include "pref.hpp"	// user
#include "icon.hpp"	// user
#if HAVE_OCAML
#include "aiinit.hpp"	// read_request
#endif
#include <iostream>


/* ==================================================== ======== ======= */
// MESSAGES and REQUESTS

Message::Message(Widgets* _gw) :
 gw(*_gw)
{
  messbox.addAttr(UOrient::vertical + UFont::small + utop());
  messbox.add(uhbox(UFont::bold + UColor::orange + "Have fun with VREng ;-)"));
  history.push_back(ustr(""));
  history_pos = 0;
}

UBox& Message::createQuery()
{
  UTextfield& input = utextfield(usize(250) + text + ucall(this, &Message::inputCB));
  UBox& query = uhbox(ulabel(UFont::bold + "input: ")
               + uhflex()
               + input
               + uright()
               + uitem(  utip("Clear input")
                       + UFont::bold + "C"
                       + uassign(text, "")
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

UBox& Message::createMessagePanel(bool transparent)
{
  scrollpane.add(messbox);
  scrollpane.showVScrollButtons(false);
  scrollpane.getHScrollbar()->show(false);

  UTextfield& input = utextfield(usize(250) + text + ucall(this, &Message::inputCB));
  UBox& query = uhbox(ulabel(UFont::bold + "input: ")
               + uhflex()
               + input
               + uright()
               + uitem(  utip("Clear input")
                       + UFont::bold + "C"
                       + uassign(text, "")
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

  UBox& notif = ubox(UOrient::vertical
                     + uhflex()
                     + uvflex()
                     + scrollpane
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
  if (text.empty()) return;

  history.push_back(text);	// save text into history

  if (text[0] == '!') {	// starts with a '!'
    performRequest(text);
  }
  else {
    writeMessage("chat", g.user, text.c_str());	// display in message zone
    User::userWriting(text.c_str());		// send to localuser
  }
  text = "";	// clear textfield
}

void Message::getHistoryCB(int go)
{
  history_pos = (history.size() + history_pos + go) % history.size();
  text = history[history_pos];
}

void Message::performRequest(const UStr& req)	// req starts with a '!'
{
#if HAVE_OCAML
  if (req.empty() || !isalpha(req[1])) return;

  const char* req_chars = req.c_str() + 1;	// skip the initial '!'
  text = "";
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
      text.append(p);
      text.append(" ");	// should be with red color
      p = strtok_r(NULL, " ", &brkt);
    }
    free(sentence);
  }
  else {
    g.gui.writeMessage("request", null, req_chars); // display in request history
    text.append(req);
  }
#endif //HAVE_OCAML
}

void Message::performRequest(WObject* object)
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
    if (! text.empty()) performRequest(text);
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
  if (p) x = (float) atof(p); else goto ERROR;
  p = strtok_r(NULL, ",", &brkt);
  if (p) y = (float) atof(p); else goto ERROR;
  p = strtok_r(NULL, ",", &brkt);
  if (p) z = (float) atof(p); else goto ERROR;
  p = strtok_r(NULL, ",", &brkt);
  if (p) az = (float) atof(p); else goto ERROR;

  free(positions_copy);
  return true;

ERROR:
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

void Message::convertTextToLink(const std::string& text, char **listeObjets, int size)
{
  UElem* allmsgs = new UElem();
  bool found = false;
  char* mess = strdup(text.c_str());

  if (size < 1) {
    cerr << "convertTextToLink not implemented" << endl;
    messbox.add(uhbox(ugroup(mess)));
  }
  char *brkt = null;
  char *tempmess = strtok_r(mess, " ", &brkt);

  while (tempmess) {
    found = false;
    for (int i=0; i<size ; i=i+3) {

      if (! listeObjets[i])
        break;
      if ((! strcasecmp(listeObjets[i], tempmess)) && (listeObjets[i+1])) {
        UIma& uimg = uima(listeObjets[i+1]);	// loads image
        uimg.rescale(0.25);
        ULinkbutton& ulinkb =
        ulinkbutton(  listeObjets[i]
                    + UColor::green + UFont::bold
                    + umenu(ulabel(uimg))
                    + UOn::doubleClick / ucall((char*)listeObjets[i+2], moveSatCamera)
                    );
        allmsgs->add(ulinkb);
        allmsgs->add(ustr(" "));
        found = true;
        unlink(listeObjets[i+1]);	// unlink temp file
        break;
      }
    }
    if (!found) {
      allmsgs->add(ugroup(tempmess));
      allmsgs->add(ustr(" "));
    }
    tempmess = strtok_r(NULL, " ", &brkt);
  }
  messbox.add(uhbox(ustr("augmented msg>") + allmsgs));
  free(mess);
}

void Message::postRequest(const std::string& mess, std::string& result)
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
    else { //not @
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
// chat, warning, notice...

void Message::writeMessage(const char* mode, const char* from, const char* msg)
{
  if (! msg)  return;

  uptr<UStr> text = null;
  uptr<UStr> prefix = null;
  uptr<UColor> prefix_color = null;

  if (from) {
    // chat
    std::string result;
    postRequest(msg, result);
    text = new UStr(result);

    prefix = new UStr("from ");
    prefix->append(from);
    prefix->append("> ");
    prefix_color = &::g.theme.chatColor;
  }

  else if (mode) {
    // internal
    text = new UStr(msg);

    if (! strcmp(mode, "notice")) {
      // notice
      prefix = new UStr(mode);
      prefix->append("> ");
      prefix_color = ::g.theme.noticeColor;
    }
    else if (! strcmp(mode, "progress")) {
      // progress
      prefix = new UStr("");
      prefix_color = ::g.theme.noticeColor;
      messbox.add(uhbox(ugroup(UFont::bold + prefix_color) + text));
    }
    else if (! strcmp(mode, "request")) {
      // requext
      prefix = new UStr(mode);
      prefix->append("> ");
      prefix_color = ::g.theme.requestColor;
    }
    else {
      // warning
      prefix = new UStr(mode);
      prefix->append("> ");
      prefix_color = ::g.theme.warningColor;
    }
  }
  else {
    // chat to
    error("msg: %s", msg);
    text = new UStr(msg);
    prefix = new UStr("> ");
    prefix_color = ::g.theme.messColor;
  }

  messbox.add(uhbox(ugroup(UFont::bold + prefix_color + prefix) + text));
  return;
}

/* ==================================================== ======== ======= */

Message2::Message2() {}

void Message2::putIconCB()
{
  char infos[BUFSIZ];
  memset(infos, 0, sizeof(infos));
  if (url.c_str()) {
    strcat(infos, "<url=\"");
    strcat(infos, url.c_str());
    strcat(infos, "\">&");
  }
  if (file.c_str()) {
    strcat(infos, "<file=\"");
    strcat(infos, file.c_str());
    strcat(infos, "\">&");
  }
  if (ofile.c_str()) {
    strcat(infos, "<ofile=\"");
    strcat(infos, ofile.c_str());
    strcat(infos, "\">&");
  }
  if (name.c_str()) {
    strcat(infos, "<name=\"");
    strcat(infos, name.c_str());
    strcat(infos, "\">&");
  }
  if (icon.c_str()) {
    strcat(infos, "<icon=\"");
    strcat(infos, icon.c_str());
    strcat(infos, "\">&");
  }
  doAction(ICON_TYPE, Icon::CREATE, localuser, infos, 0, 0);
}

void Message2::putIcon(const UStr& val)
{
  doAction(ICON_TYPE, Icon::CREATE, localuser, (void*)val.c_str(), 0, 0);
}
