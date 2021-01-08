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

  Xml() {};
  /**< Constructor */

  virtual ~Xml();
  /**< Destructor */

  static int returnXmlDocument(char* filename,xmlDocPtr* doc);
  /**< return the scene described in xml type **/

  static int selectXpathPoint(char *filename, float * position_, char *phrase, char *result);
  /**< search object with the position of mouse and the type **/

  static int selectProximity(char *filename,char *type, float *posx,float *posy,float *posz,float *ori);
  /**< search l'objet du type le plus proche **/

  static int selectXpathExpr(char *filename, const char *xpathExpr_, char *phrase, char *result);
  /**< search object with the name or something else **/

  static void dtdValidation(const char *pathname);
  /**< Checks DTD **/

 private:
  static int returnXpathPoint(xmlNodeSetPtr nodes,float * position, char *result);

  static int returnProximity(xmlNodeSetPtr nodes, float *posx,float *posy,float *posz,float *ori);
  /**< Calculate the promity object */

  static int returnXpathExpr(xmlNodeSetPtr nodes, char *phrase, char *result);
  /**< explorer main attributs of the selected object */

  static int returnXpathName(xmlNodeSetPtr nodes, char *result);
  /**< explorer name attributs of the selected object */

  static int string2Coord(char *stringposition,float *tmpx,float *tmpy,float *tmpz,float *tmporiz);
  /**< Convert string position to 4 floats **/
};

#endif //HAVE_LIBXML2

#endif
