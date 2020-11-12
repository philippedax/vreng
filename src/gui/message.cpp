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
#include "widgets.hpp"
#include "scene.hpp"
#include "navig.hpp"
#include "theme.hpp"	// theme
#include "message.hpp"	// PutInfo
#include "wobject.hpp"	// WObject
#include "user.hpp"	// userWriting
#include "render.hpp"	// setCameraScissor
#include "cache.hpp"	// check
#include "pref.hpp"	// user
#include "icon.hpp"	// user
#if HAVE_OCAML
#include "aiinit.hpp"	// inputRequest
#endif
#include <iostream>

/* ==================================================== ======== ======= */
// MESSAGES and REQUESTS

Messages::Messages(Widgets* _gw) : gw(*_gw)
{
  messages.addAttr(UOrient::vertical + UFont::small + utop());
  messages.add(uhbox(UFont::bold + UColor::orange + "Have fun with VREng ;-)"));
  history.push_back(ustr(""));
  history_pos = 0;
}

UBox& Messages::createMessagePanel(bool transparent)
{
  scrollpane.add(messages);
  scrollpane.showVScrollButtons(false);
  scrollpane.getHScrollbar()->show(false);

  UTextfield& tf = utextfield(usize(250) + entry + ucall(this, &Messages::actionCB));

  UBox& panel =
  ubox(UOrient::vertical  + uhflex()
       + uvflex()
       + scrollpane
       + ubottom()
       + uhbox(ulabel(UFont::bold + "Message: ")
               + uhflex()
               + tf
               + uright()
               + uitem(utip("Clear") + uassign(entry, "")
                       + UFont::bold + "C")
               + uitem(utip("Previous message") + ucall(this,-1,&Messages::getHistoryCB)
                       + USymbol::up)
               + uitem(utip("Next message") + ucall(this,+1,&Messages::getHistoryCB)
                       + USymbol::down)
               )
       );
  if (transparent) {
    tf.addAttr(UBackground::none + UColor::white);
    panel.addAttr(UColor::white);
  }
  return panel;
}

void Messages::actionCB()
{
  if (entry.empty()) return;

  history.push_back(entry);  // save in history

  if (entry[0] == '!') performRequest(entry);
  else {
    writeMessage("chat", g.user, entry.c_str());  // display in message zone
    User::userWriting(entry.c_str());		       // send to World Management
  }
  entry = ""; // clear textfield
}

void Messages::getHistoryCB(int go)
{
  history_pos = (history.size() + history_pos + go) % history.size();
  entry = history[history_pos];
}

void Messages::performRequest(const UStr& req) // req starts with a '!'
{
#if HAVE_OCAML
  if (req.empty() || !isalpha(req[1])) return;

  const char* req_chars = req.c_str() + 1;  // skip the initial '!'
  entry = "";
  int r = read_request(req_chars);

  if (r > 0) {
    nclicks = r;
    for (int i=0; i<7 ;i++) {clicked[i] = MAXFLOAT;}

    char *phrase = strdup(req_chars);
    char *brkt = null;
    char *p = strtok_r(phrase, " ", &brkt);
    while (p) {
      entry.append(p); entry.append(" ");	// should be with red color
      p = strtok_r(NULL, " ", &brkt);
    }
    free(phrase);
  }
  else {
    g.gui.writeMessage("request", null, req_chars); // display in request history
    entry.append(req);
  }
#endif //HAVE_OCAML
}

void Messages::performRequest(WObject* object)
{
  if (object && nclicks > 0) {
    clicked[0]=object->pos.x;
    clicked[1]=object->pos.y;
    clicked[2]=object->pos.z;
    clicked[3]=object->pos.az;
    clicked[4]=object->pos.bbsize.v[0];
    clicked[5]=object->pos.bbsize.v[1];
    clicked[6]=object->pos.bbsize.v[2];
    nclicks--;
    if (!entry.empty()) performRequest(entry);
  }
}

void Messages::initClicked()
{
  nclicks = 0;
  for (uint32_t i=0; i < sizeof(clicked)/sizeof(float) ; i++) {
    clicked[i] = MAXFLOAT;
  }
}

void Messages::getClicked(int *_nclicks, float _clicked[])
{
  *_nclicks = nclicks;
  for (uint32_t i=0; i < sizeof(clicked)/sizeof(float) ; i++) {
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
static void moveSatCamera(char* posbuf)
{
  float x, y, z, az;

  string2Coord(posbuf, x, y, z, az);
  g.render.setCameraScissor(x, y, z, az);
}

void Messages::convertTextToLink(const std::string& text, char **listeObjets, int size)
{
  UElem* allmsgs = new UElem();
  bool found = false;
  char* mess = strdup(text.c_str());

  if (size < 1) {
    cerr << "convertTextToLink not implemented" << endl;
    messages.add(uhbox(ugroup(mess)));
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
        ulinkbutton(listeObjets[i]
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
  messages.add(uhbox(ustr("augmented msg>") + allmsgs));
  free(mess);
}

void Messages::postRequest(const std::string& mess, std::string& result)
{
  int sizeMax = 256;
  int sizerecu = mess.length();
  float posx, posy, posz, posaz; //coordonnees trouvees
  int afficher = 0;
  int nbObj = 0;
  char posbuf[sizerecu +1];
  char namebuf[sizerecu +1];
  char tmpbuf[sizerecu +1];
  char **listeObjets = new char*[sizeMax];

  posbuf[0] = '\0';
  namebuf[0] = '\0';
  tmpbuf[0] = '\0';

  for (int i=0; i<sizerecu ; i++) {
    if (mess[i] == '@') {
      if (nbObj > (sizeMax-2)) {
        cerr << "message has too many request" <<endl;
        break;
      }
      if (afficher == 0) {
        posbuf[0] = '\0';
        namebuf[0] = '\0';
      }
      else if (afficher == 1) {
        listeObjets[nbObj] = strdup(namebuf);
        nbObj++;
      }
      else if (afficher == 2) {
        //on enregistre l'image ds un fichier.
        listeObjets[nbObj] =  strdup("request.jpg");
        listeObjets[nbObj+1] = strdup(posbuf);

        //on convertit le string en coords.
        string2Coord(posbuf, posx, posy, posz, posaz);

        //recupere la position et fait un snapshot de la position demandée	
        ::g.render.calculateFov(posx,posy,posz,posaz, listeObjets[nbObj]);
        nbObj += 2;
      }
      afficher = (afficher+1) % 3;
      continue;
    }
    else { //not @
      if (afficher == 0) {
        //sprintf(msgAffiche, "%s%c", msgAffiche, mess[i]);
        result += mess[i];
      }
      else if (afficher == 1) {
        //sprintf(msgAffiche, "%s%c", msgAffiche, mess[i]);
        result += mess[i];
	strcpy(tmpbuf, namebuf);
        sprintf(namebuf, "%s%c", tmpbuf, mess[i]);
      }
      else if (afficher == 2) {
	strcpy(tmpbuf, posbuf);
        sprintf(posbuf, "%s%c", tmpbuf, mess[i]);
      }
    }
  }

  if (nbObj > 0) convertTextToLink(result, listeObjets, nbObj);
  //TODO ev : traiter le message non augmente

  if (listeObjets) delete[] listeObjets;
  listeObjets = NULL;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// chat, warning, notice...

void Messages::writeMessage(const char* mode, const char* from, const char* msg)
{
  if (!msg) return;

  uptr<UStr> text = null;
  uptr<UStr> prefix = null;
  uptr<UColor> prefix_color = null;

  if (from) {
    std::string result;
    postRequest(msg, result);
    text = new UStr(result);

    prefix = new UStr("from ");
    prefix->append(from);
    prefix->append("> ");
    prefix_color = &::g.theme.chatColor;
  }

  else if (mode) {
    text = new UStr(msg);

    if (! strcmp(mode, "notice")) {
      prefix = new UStr(mode);
      prefix->append("> ");
      prefix_color = ::g.theme.noticeColor;
    }
    else if (! strcmp(mode, "progress")) {
      prefix = new UStr("");
      prefix_color = ::g.theme.noticeColor;
      messages.add(uhbox(ugroup(UFont::bold + prefix_color) + text));
    }
    else if (! strcmp(mode, "request")) {
      prefix = new UStr(mode);
      prefix->append("> ");
      prefix_color = ::g.theme.requestColor;
    }
    else {
      prefix = new UStr(mode);
      prefix->append("> ");
      prefix_color = ::g.theme.warningColor;
    }
  }
  else {
    text = new UStr(msg);
    prefix = new UStr("> ");
    prefix_color = ::g.theme.messColor;
  }

  messages.add(uhbox(ugroup(UFont::bold + prefix_color + prefix) + text));
  return;
}

/* ==================================================== ======== ======= */

void Widgets::putMessage(UMessageEvent& e)
{
  const UStr* arg = e.getMessage();
  if (!arg || arg->empty()) return;

  UStr file_name = arg->basename();
  UStr val;

  if (file_name.empty()) val = "<url=\"" & *arg & "\">";
  else                   val = "<url=\"" & *arg & "\">&<name=\"" & file_name & "\">";
  if (! Cache::check(arg->c_str())) return;    // bad url
  putinfo.putIcon(val);
}

void Widgets::openMessage(UMessageEvent &e)
{
  const UStr* msg = e.getMessage();
  if (!msg || msg->empty()) return;
  gui.gotoWorld(*msg);
}

void Widgets::moveMessage(UMessageEvent &e)
{
  const UStr* msg = e.getMessage();
  if (!msg || msg->empty()) return;
  const UStr& arg = *msg;

  if (arg == "left 1")            setKey(KEY_SG, TRUE);
  else if (arg == "left 0")       setKey(KEY_SG, FALSE);
  else if (arg == "right 1")      setKey(KEY_SD, TRUE);
  else if (arg == "right 0")      setKey(KEY_SD, FALSE);
  else if (arg == "forward 1")    setKey(KEY_AV, TRUE);
  else if (arg == "forward 0")    setKey(KEY_AV, FALSE);
  else if (arg == "backward 1")   setKey(KEY_AR, TRUE);
  else if (arg == "backward 0")   setKey(KEY_AR, FALSE);
  else if (arg == "up 1")         setKey(KEY_JU, TRUE);
  else if (arg == "up 0")         setKey(KEY_JU, FALSE);
  else if (arg == "down 1")       setKey(KEY_JD, TRUE);
  else if (arg == "down 0")       setKey(KEY_JD, FALSE);
  else if (arg == "turn_left 1")  setKey(KEY_GA, TRUE);
  else if (arg == "turn_left 0")  setKey(KEY_GA, FALSE);
  else if (arg == "turn_right 1") setKey(KEY_DR, TRUE);
  else if (arg == "turn_right 0") setKey(KEY_DR, FALSE);
}

void Widgets::getMessage(UMessageEvent &e)
{
  const UStr* msg = e.getMessage();
  if (!msg || msg->empty()) return;

  // a completer
  //cerr << "get: " << *selected_object_url << endl;
}

PutInfo::PutInfo() {}

void PutInfo::putIconCB()
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

void PutInfo::putIcon(const UStr& val)
{
  doAction(ICON_TYPE, Icon::CREATE, localuser, (void*)val.c_str(), 0, 0);
}
