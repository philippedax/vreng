//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2024 Philippe Dax
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
// parse.hpp
//---------------------------------------------------------------------------
#ifndef PARSE_HPP
#define PARSE_HPP

#include "matvec.hpp"   // V3 M4
#include "str.hpp"	// stringcmp()


#define TOK_MAX		255
#define TOK_SEP		" ,<>\t\n"	///< token separators: space comma tags tab nl

//
// internal macros
//
// parsing stops when l == 0
//
#define begin_while_parse(l) \
next_attr: \
  int cnt_attr; \
  for ( cnt_attr = 0; l && cnt_attr <= TOK_MAX ; cnt_attr++ )

#define end_while_parse(l) \
  if (cnt_attr >= TOK_MAX) { \
    error("bad attr %s", l); \
    l = strtok(NULL, TOK_SEP); \
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
    TAG_BEGINVRE,	///< <vre ... >
    TAG_META,		///< <meta> ... </meta>>
    TAG_COMMENT,	///< <!-- ... -->
    TAG_SCENE,		///< <scene> ... </scene>
    TAG_OBJECT,		///< <object ... </object>
    TAG_LOCAL,		///< <local> ... </local>
    TAG_ENDVRE		///< </vre>
  } tags;

  int tag_id;		///< tag type
  bool commented;	///< flag commented or not

  char * nextSpace(char *p) const;
  /**< find next space or next endtag */

  char * skipChar(char *p, char c) const;
  /**< Skip this character */

  char * skipSpace(char *p) const;
  /**< Skip space character */

  char * parseDescr(char *l, char *infos);
  /**< Returns a description in infos */

  int parseLine(char *_line, int *ptag_id);
  /**< parse begin of line */

  inline bool isFloat(const char *p) const;
  /**< Check if string begins with a float */

  inline bool isInt(const char *p) const;
  /**< Check if string begins with an integer */

  char * parseVectorf(char *ptok, float *vector, int n);
  /**< Returns a vector of dimension n */

 public:
  static const int TAG_LEN = 16;	// tag length

  uint32_t numline;	///< line number
  char tag[TAG_LEN];	///< tag

  Parse();
  /**< Constructor */

  virtual ~Parse();
  /**< Destructor */

  static Parse * getParse();
  /**< Gets the parse instance */

  void errorAtLine();
  void errorAtLine(const char *attr);
  /**< print parse error at line */

  int parseVreFile(char *buf, int bufsiz);
  /**< parse vre data, called by vreHttpReader */

  char * nextToken() const;
  /**< Gets the next token */

  char * parseAttributes(char *l, class Object *po);
  /**< Parses attribute="value" */

  char * parseName(char *l, char *name);
  /**< Returns a named name */

  char * parsePosition(char *ptok, Pos &p);
  /**< Returns the spacial positions x y z az ax */

  char * parseUrl(char *ptok, char *url);
  /**< Returns an url */

  char * parseColor(char *ptok, Pos &p);
  /**< Returns a color under r,g,b,a format */

  char * parseSolid(char *ptok, class Object *po);
  /**<
   * Creates a new solid.
   * string 'solid' gives the solid's geometry
   */

  char * parseSolid(char *geom, const char *separ, class Object *po);
  /**< Parses a builtin solid */

  void parseSolids(char *geom, const char *separ, class Object *po);
  /**< Parses several solids */

  char * parseWorldAndChannel(char *ptok, char *url, char *chan);
  /**< Returns a world url and a numeric string */

  char * parseChannel(char *ptok, char *channel);
  /**< Returns a channel under addr/port/ttl format */

  char * parseGuide(char *ptok, float path[][5], uint8_t *segs);
  /**< Returns an array of position describing the guide */

  char * parseString(char *ptok, char *str);
  /**< Returns a string */

  char * parseString(char *ptok, char *str, const char *attrstr);
  /**< Returns a string under attr=string format */

  char * parseQuotedString(char *ptok, char *str);
  /**< Returns a string under "string ..." format */

  char * parseQuotedString(char *ptok, char *str, const char *attrstr);
  /**< Returns a string under attr="string ..." format */

  char * parseInt(char *ptok, int *value);
  /**< Returns an integer */

  char * parseInt(char *ptok, int *value, const char *attrstr);
  /**< Returns an integer under attr=value format */

  char * parseBool(char *ptok, bool *value);
  /**< Returns an 8 bits integer */

  char * parseBool(char *ptok, bool *value, const char *attrstr);
  /**< Returns an 8 bits integer under attr=value format */

  char * parseUInt8(char *ptok, uint8_t *value);
  /**< Returns an 8 bits integer */

  char * parseUInt8(char *ptok, uint8_t *value, const char *attrstr);
  /**< Returns an 8 bits integer under attr=value format */

  char * parseUInt16(char *ptok, uint16_t *value);
  /**< Returns an 16 bits integer */

  char * parseUInt16(char *ptok, uint16_t *value, const char *attrstr);
  /**< Returns an 16 bits integer under attr=value format */

  char * parseFloat(char *ptok, float *value);
  /**< Returns a float */

  char * parseFloat(char *ptok, float *value, const char *attrstr);
  /**< Returns a float under attr=value format */

  char * parseVector3f(char *ptok, float *vector);
  /**< Returns a vector under vx,vy,vz format */

  char * parseVector3f(char *ptok, float *vector, const char *attrstr);
  /**< Returns a vector under attr=vx,vy,vz format */

  char * parseVector3fv(char *ptok, V3 *vector);
  /**< Returns a vector under vx,vy,vz format */

  char * parseVector3fv(char *ptok, V3 *vector, const char *attrstr);
  /**< Returns a vector under attr=vx,vy,vz format */

  char * parseVector5f(char *ptok, float *vector);
  /**< Returns a vector under vx,vy,vz,vaz,vax format */

  char * skipAttribute(char *l);
  /**< Skip the following attribute="value" */

  char * parseRotation(char *ptok, Pos &p);
  /**< Returns a rotation under r,X,Y,Z format */

  char * parseTranslation(char *ptok, Pos &p);
  /**< Returns a translation under tx,ty,tz format */

  char * parseCaption(char *l, char *caption);
  /**< Returns a caption text */

  char * parseCaption(char *l, char *caption, const char *keystr);
  /**< Returns a caption text */

  char * skipEqual(char *p) const;
  /**< Skip equal character */

  char * skipQuotes(char *p) const;
  /**< Skip double quotes or single quote */
};

#endif
