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
#ifndef PARSE_HPP
#define PARSE_HPP

#include "str.hpp"	// stringcmp()

#define SEP	" ,<>\t\n"	///< token separators: space comma tags tab nl
#define MAX_ATTR 32

// macros
#define begin_while_parse(l) \
next_attr: \
  int cnt_attr; \
  for (cnt_attr = 0; l && cnt_attr < MAX_ATTR ; cnt_attr++)

#define end_while_parse(l) \
  if (cnt_attr >=  MAX_ATTR) { \
    error("%s: bad attribute", l); \
    l = strtok(NULL, SEP); \
    goto next_attr; \
  }


struct Pos;

/**
 * Parse class
 */
class Parse {

private:

  /**
   * Tags enum
   */
  enum {
    TAG_DOCTYPE,	///< <!DOCTYPE ... >
    TAG_BEGINFILE,	///< <vre ... >
    TAG_HEAD,		///< <head> ... </head>
    TAG_META,		///< <meta ... />
    TAG_COMMENT,	///< <!-- ... -->
    TAG_SCENE,		///< <scene> ... </scene>
    TAG_OBJECT,		///< <object ... </object>
    TAG_LOCAL,		///< <local> ... </local>
    TAG_ENDFILE		///< </vre>
  } tags;

  int tag_type;		///< tag type
  bool commented;	///< flag commented or not

public:
  static const int TAG_LEN = 16;	// tag length

  int numline;		///< line number
  char tagobj[TAG_LEN];	///< tag

  Parse();
  /**< Constructor */

  virtual ~Parse();
  /**< Destructor */

  static Parse * getParse();
  /**< Gets the parse instance */

  virtual char * nextToken();
  /**< Gets the next token */

  virtual char * skipChar(char *p, char c, bool flag);
  /**< Skip this character */

  virtual char * skipEqual(char *p);
  /**< Skip equal character */

  virtual char * skipOpenBracket(char *p);
  /**< Skip open-bracket character */

  virtual char * skipOpenParenthesis(char *p);
  /**< Skip open-parenthesis character */

  virtual char * skipCloseParenthesis(char *p);
  /**< Skip close-parenthesis character */

  virtual char * skipQuotes(char *p, bool flag=true);
  /**< Skip double quotes or single quote */

  virtual char * nextSpace(char *p);
  /**< find next space or next endtag */

  virtual char * skipSpace(char *p);
  /**< Skip space character */

  virtual char * skipComma(char *p);
  /**< Skip comma character */

  virtual char * parseName(char *l, char *name);
  /**< Returns a named name */

  virtual char * parseDescr(char *l, char *infos);
  /**< Returns a description in infos */

  virtual char * skipAttribute(char *l);
  /**< Skip the following attribute="value" */

  virtual char * parseAttributes(char *l, class WObject *po);
  /**< Parses attribute="value" */

  virtual char * parseSolid(char *ptok, class WObject *po);
  /**<
   * Creates a new solid.
   * string 'solid' gives the solid's geometry
   * Returns the solid's handle.
   */

  virtual char * parseSolid(char *geom);
  /**< Parses a builtin solid */

  virtual char * parseSolid(char *geom, const char *separ, class WObject *po);
  /**< Parses a builtin solid */

  virtual void parseSolids(char *geom, const char *separ, class WObject *po);
  /**< Parses several solids */

  virtual char * parsePosition(char *ptok, Pos &p);
  /**< Returns the spacial positions x y z az ax */

  virtual char * parseColor(char *ptok, Pos &p);
  /**< Returns a color under r,g,b,a format */

  virtual char * parseRotation(char *ptok, Pos &p);
  /**< Returns a rotation under r,X,Y,Z format */

  virtual char * parseTranslation(char *ptok, Pos &p);
  /**< Returns a translation under tx,ty,tz format */

  virtual char * parseUrl(char *ptok, char *url);
  /**< Returns an url */

  virtual char * parseWorldAndChannel(char *ptok, char *url, char *chan);
  /**< Returns a world url and a numeric string */

  virtual char * parseChannel(char *ptok, char *channel);
  /**< Returns a channel under addr/port/ttl format */

  virtual char * parseString(char *ptok, char *str);
  /**< Returns a string */

  virtual char * parseString(char *ptok, char *str, const char *attrstr);
  /**< Returns a string under attr=string format */

  virtual char * parseQuotedString(char *ptok, char *str);
  /**< Returns a string under "string ..." format */

  virtual char * parseQuotedString(char *ptok, char *str, const char *attrstr);
  /**< Returns a string under attr="string ..." format */

  virtual char * parseInt(char *ptok, int *value);
  /**< Returns an integer */

  virtual char * parseInt(char *ptok, int *value, const char *attrstr);
  /**< Returns an integer under attr=value format */

  virtual char * parseUInt8(char *ptok, uint8_t *value);
  /**< Returns an 8 bits integer */

  virtual char * parseUInt8(char *ptok, uint8_t *value, const char *attrstr);
  /**< Returns an 8 bits integer under attr=value format */

  virtual char * parseUInt16(char *ptok, uint16_t *value);
  /**< Returns an 16 bits integer */

  virtual char * parseUInt16(char *ptok, uint16_t *value, const char *attrstr);
  /**< Returns an 16 bits integer under attr=value format */

  virtual char * parseFloat(char *ptok, float *value);
  /**< Returns a float */

  virtual char * parseFloat(char *ptok, float *value, const char *attrstr);
  /**< Returns a float under attr=value format */

  virtual char * parseVector3f(char *ptok, float *vector);
  /**< Returns a vector under vx,vy,vz format */

  virtual char * parseVector3f(char *ptok, float *vector, const char *attrstr);
  /**< Returns a vector under attr=vx,vy,vz format */

  virtual char * parseVector3fv(char *ptok, V3 *vector);
  /**< Returns a vector under vx,vy,vz format */

  virtual char * parseVector3fv(char *ptok, V3 *vector, const char *attrstr);
  /**< Returns a vector under attr=vx,vy,vz format */

  virtual char * parseVector5f(char *ptok, float *vector);
  /**< Returns a vector under vx,vy,vz,vaz,vax format */

  virtual char * parsePath(char *ptok, float path[][5], uint16_t *segs);
  /**< Returns an array of position describing the path */

  virtual int parseVreLines(char *buf, int bufsiz);
  /**< parse vre data, called by vreHttpReader */

  void printNumline();
  /**< prints numline where an error occurs */

private:
  virtual int parseLine(char *_line, int *ptag_type);
  /**< parse begin of line */

  inline bool isFloat(const char *p);
  /**< Check if string begins with a float */

  inline bool isInt(const char *p);
  /**< Check if string begins with an integer */

  virtual char * parseVectorf(char *ptok, float *vector, int n);
  /**< Returns a vector of dimension n */


#if 0 //not used
  virtual char * parsePositionOrientation(char *ptok);
  /**< Returns the spacial positions x y z az ax */
  virtual char * parseX(char *ptok);
  /**< Returns x position */
  virtual char * parseY(char *ptok);
  /**< Returns y position */
  virtual char * parseZ(char *ptok);
  /**< Returns z position */
  virtual char * parseAZ(char *ptok);
  /**< Returns az orientation */
  virtual char * parseAX(char *ptok);
  /**< Returns ax orientation */
#endif //not used

};

#endif
