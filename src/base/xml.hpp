//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// xml.hpp
//---------------------------------------------------------------------------
#ifndef XML_HPP
#define XML_HPP

#if HAVE_LIBXML2 && HAVE_LIBXML_PARSER_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>


/**
 * Xml class
 */
class Xml {
 public:
  Xml() {};		/**< Constructor */
  virtual ~Xml();	/**< Destructor */

  static void dtdValidation(const char *pathname);
  /**< Checks DTD */

  static int selectXpathPoint(char *filename, float * position_, char *phrase, char *result);
  /**< search object with the position of mouse and the type */

  static int selectProximity(char *filename,char *type, float *posx,float *posy,float *posz,float *ori);
  /**< search l'objet du type le plus proche */

  static int selectXpathExpr(char *filename, const char *xpathExpr, char *phrase, char *res);
  /**< search object with the name or something else */

 private:
  static int getXpathPoint(xmlNodeSetPtr nodes,float * position, char *result);

  static int getProximity(xmlNodeSetPtr nodes,float *posx,float *posy,float *posz,float *ori);
  /**< Calculate the promimity object */

  static int getXpathExpr(xmlNodeSetPtr nodes, char *phrase, char *result);
  /**< explorer main attributes of the selected object */

  static int getXpathName(xmlNodeSetPtr nodes, char *result);
  /**< Explores name attributes of the selected object */

  static int string2Coord(char *stringposition,float *tmpx,float *tmpy,float *tmpz,float *tmporiz);
  /**< Convert string position to 4 floats */
};

#endif //HAVE_LIBXML2

#endif
