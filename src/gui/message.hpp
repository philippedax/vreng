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
#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "ubit/ubit.hpp"
#include <string>
#include <vector>

using namespace ubit;
using namespace std;


class WO;

/**
 * Message class
 */
class Message {
friend class Widgets;

public:
  vector<UStr> mess_history;	///< history log

  Message(class Widgets*);
  
  UBox& createQuery();
  ///< creates the query box

  UBox& createMessagePanel(bool transparent);
  ///< creates the message panel

  void writeMessage(const char* mode, const char* from, const char* mess);
  ///< adds a new message to the messages box.
    
  void performRequest(WO*);
  void initClicked();

  void getClicked(uint8_t *nclick, float clicked[]);
  ///< get clicked object for the request.

protected:
  class Widgets& gw;
  UBox mess_box;		///< message box
  UScrollpane mess_scrollpane;	///< message panel
  UStr mess_text; 		///< input text
  int mess_history_pos;		///< position in history
  float clicked[7];		///< Ocaml
  uint8_t nclicks;		///< Ocaml

  void inputCB();
  void getHistoryCB(int go);
  void performRequest(const UStr& req);  // req starts with a '!'
  void postRequest(const std::string& mess, std::string& result);
  void convertTextToLink(const std::string& text, char **listeObjets, int size);
};

/**
 * Message2 class
 */
class Message2 {
public:
  Message2();		///< Constructor.

  void putIcon(const UStr& val);
  ///< Puts Icon's infos.
  void putIconCB();
  ///< Puts Icon.

  UStr url;	///< document's url
  UStr file;	///< local file
  UStr ofile;	///< public file
  UStr name;	///< document's name
  UStr icon;	///< document's icon
 };

#endif
