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


class WObject;

/**
 * Message class
 */
class Message {
public:
  Message(class Widgets*);
  
  UBox& createMessagePanel(bool transparent);

  void writeMessage(const char* mode, const char* from, const char* mess);
  ///< adds a new message to the messages box.
    
  void performRequest(WObject*);
  void initClicked();
  
  void getClicked(int *click, float clicked[]);
  ///< get clicked object for the request.

protected:
  void actionCB();
  void getHistoryCB(int go);
  void performRequest(const UStr& req);  // req starts with a '!'
  void postRequest(const std::string& mess, std::string& result);
  void convertTextToLink(const std::string& text, char **listeObjets, int size);

  class Widgets& gw;
  UBox messbox;
  UScrollpane scrollpane;
  UStr entry; 
  std::vector<UStr> history; ///< history log
  int history_pos;
  float clicked[7];	///< Ocaml
  int nclicks;		///< Ocaml
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
