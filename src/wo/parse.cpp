//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
#include "vreng.hpp"
#include "parse.hpp"	// Parse
#include "wobject.hpp"	// WObject
#include "world.hpp"	// current
#include "solid.hpp"	// Solid
#include "color.hpp"	// getRGB
#include "timer.hpp"	// timer
#include "pref.hpp"	// g.pref.dbgtrace
#include "str.hpp"	// stringcmp


// local
static Parse *parse = NULL;

/* aliases */
struct sObj {
  const char *objalias;
  const char *objreal;
};

static const struct sObj objs[] = {
  //{ "oldname", "newname" },
  { "ftp", "download" },
  { "animx3d", "carousel" },
  { NULL, NULL }
};


Parse::Parse()
{
  parse = this;		// save pointer
  numline = 0;
  tag_type = 0;
  commented = false;
}

Parse::~Parse()
{
  parse = NULL;
}

/** Gets parse pointer : accessor */
Parse * Parse::getParse()
{
  if (! parse)
    parse = new Parse();
  return parse;
}


/** Goto next token, a token is a string delimitrd by a separator (see SEP) */
char * Parse::nextToken() const
{
  return strtok(NULL, SEP);
}

char * Parse::skipChar(char *p, char c, bool flag) const
{
  char *s = p;
  if (p && (p = strchr(p, c)))
    p++;
  else
    if (flag) error("parse error at line %d (missing '%c' in %s)", numline-1, c, s);
  return p;
}

char * Parse::skipEqual(char *p) const
{
  return skipChar(p, '=', 1);
}

char * Parse::skipOpenBracket(char *p) const
{
  return skipChar(p, '[', 1);
}

char * Parse::skipOpenParenthesis(char *p) const
{
  char *q = skipChar(p, '(', 0);
  if (q) return q;
  else {
    q = nextToken();
    return skipChar(q, '(', 0);
  }
}

char * Parse::skipCloseParenthesis(char *p) const
{
  return skipChar(p, ')', 1);
}

/* Skip double quotes or single quote */
char * Parse::skipQuotes(char *p, bool flag) const
{
  if (p && ((*p == '"') || (*p == '\''))) p++;
  return p;
}

/* find next space or next endtag */
char * Parse::nextSpace(char *p) const
{
  while (p && *p && !isspace(*p) && *p != '>') p++;
  return p;
}

char * Parse::skipSpace(char *p) const
{
  while (p && isspace(*p)) p++;
  return p;
}

char * Parse::skipSepar(char *p) const
{
  return nextToken();
}

bool Parse::isFloat(const char *p) const
{
  return (p && (isdigit((int) *p) || *p == '-' || *p == '+' || *p == '.'));
}

bool Parse::isInt(const char *p) const
{
  return (p && (isdigit((int) *p) || *p == '-' || *p == '+'));
}

/* parses a vre line : returns tag number */
int Parse::parseLine(char *_line, int *ptag_type)
{
  char *line = strdup(_line);
  char *ptok = skipSpace(line);

  if (*ptok == '<') {
    if (commented) {
      FREE(line);
      return TAG_COMMENT;
    }
    ptok++;	// ptok is now after '<'

    // check <!--
    if ((! stringcmp(ptok, "!--"))) {
      commented = true;
      FREE(line);
      return TAG_COMMENT;
    }
    else if ((! stringcmp(ptok, "!DOCTYPE"))) {
      FREE(line);
      return TAG_DOCTYPE;
    }
    // check <vre ... > </vre>
    else if (! stringcmp(ptok, "vre>") || (! stringcmp(ptok, "vre") && isspace(ptok[3]))) {
      FREE(line);
      return TAG_BEGINFILE;
    }
    else if (! stringcmp(ptok, "/vre>")) {
      FREE(line);
      return TAG_ENDFILE;
    }
    // check <head> </head>
    else if ((! stringcmp(ptok, "head>"))) {
      FREE(line);
      return TAG_HEAD;
    }
    else if ((! stringcmp(ptok, "/head>"))) {
      FREE(line);
      return TAG_HEAD;
    }
    // check <meta ... />
    else if ((! stringcmp(ptok, "meta"))) {
      FREE(line);
      return TAG_META;
    }
    // check <scene> </scene>
    else if (! stringcmp(ptok, "scene>")) {
      FREE(line);
      return TAG_SCENE;
    }
    else if ((! stringcmp(ptok, "/scene>"))) {
      FREE(line);
      return TAG_SCENE;
    }
    // check <local> </local>
    else if ((! stringcmp(ptok, "local>"))) {
      FREE(line);
      return TAG_LOCAL;
    }
    else if ((! stringcmp(ptok, "/local>"))) {
      FREE(line);
      return TAG_LOCAL;
    }

    //
    // else this is an object !!!
    //
    const OClass *oclass = NULL;
    char *nexttok, sep = 0;

    if ((nexttok = nextSpace(ptok))) {	// either ' ' or '>'
      sep = *nexttok;	// memorize sep
      *nexttok = '\0';	// replace it by null
    }
    strncpy(tagobj, ptok, TAG_LEN-1);	// memorize object tag

    // handles aliases here !!! doesn't work yet !!!
    const struct sObj *ptab;
    for (ptab = objs; ptab->objalias; ptab++) {
      if (! strcmp(ptok, ptab->objalias)) {
        strcpy(tagobj, ptab->objreal);	// memorize object tag
        //error("parse: %d %s", numline, tagobj);
        break;
      }
    }
    // identify the object
    if ((oclass = OClass::getOClass(tagobj))) {
      *ptag_type = oclass->type_id;
      if (! OClass::isValidType(*ptag_type)) {
        error("parse error at line %d (invalid type), *ptag_type=%d ptok=%s",
              numline, *ptag_type, tagobj);
      }
      if (nexttok) *nexttok = sep;	// restore sep
      FREE(line);
      return TAG_OBJECT;
    }
    FREE(line);
    return TAG_OBJECT;
  }

  // not a '<'
  if (! stringcmp(ptok, "-->")) {
    FREE(line);
    commented = false;
    return TAG_COMMENT;
  }
  if (commented) {
    FREE(line);
    return TAG_COMMENT;
  }
  FREE(line);
  return TAG_OBJECT;
}

/* parse vre data, called by World::httpReader */
int Parse::parseVreFile(char *buf, int bufsiz)
{
  int len = 0;	// line length
  char *tmpline = NULL;
  static char *line = NULL;

  if (! line) {	// need a new line
    tmpline = new char[bufsiz];
    memset(tmpline, 0, bufsiz);
    len = 0;
  }
  else {	// old line: copy previous line in tmpline
    len = strlen(line);
    tmpline = new char[bufsiz + len];
    memset(tmpline, 0, bufsiz);
    strcpy(tmpline, line);
  }

  // copy buf at the end of tmpline
  memcpy(tmpline + len, buf, bufsiz);
  len += bufsiz;

  int iol = 0;	// index in line;
  int bol = 0;	// index begin of line

  // parses all lines of the vre file
  while (1) {
    int eol = 0;	// index end of line

    for (iol = bol; iol < len; iol++) {
      if (tmpline[iol] == '\n') {
        numline++;
        if (tmpline[iol-1] == '\\') {	// end of physical line, line follows
          tmpline[iol-1] = tmpline[iol] = ' ';
        }
        else {		// end of logical line
          tmpline[iol] = ' ';	// replace '\n' by ' '
	  eol = iol;
	  iol = len + 1;	// HUGLY! (to do len + 2)
        }
      }
    }
    if (iol == len + 2) {	// end of logical line
      line = new char[eol - bol + 2];	// allocate a new line

      // build line from tmpline
      memcpy(line, tmpline + bol, eol - bol);
      line[eol - bol] = '\0';
      if (line[eol - bol - 1] == '\r') line[eol - bol - 1] = '\0';	// WIN32
      //trace(DBG_FORCE, "line %d: %s %d %d", numline, line, eol, bol);

      // discard empty lines
      if (*line == '\0') {
        DELETE2(line);
        bol = eol + 1;		// begin of next line
        continue;
      }
      // discard comments begining with a '#' (legacy old syntax)
      char *p = skipSpace(line);
      if (*p == '#') {
        DELETE2(line);
        bol = eol + 1;		// begin of next line
        continue;
      }

      WObject *wobject = NULL;
      uint8_t tag = parseLine(line, &tag_type);

      switch (tag) {

        case TAG_BEGINFILE:
        case TAG_HEAD:
        case TAG_SCENE:
        case TAG_DOCTYPE:
          DELETE2(line);
          bol = eol + 1;	// begin of next line
          continue;	// or break

        case TAG_META:
	  if ((p = strstr(line, "=\"refresh\""))) {
            World::current()->setPersistent(false);
          }
	  if ((p = strstr(line, "/>"))) {
            DELETE2(line);
          }
          break;

        case TAG_COMMENT:
	  if ((p = strstr(line, "-->"))) {
            DELETE2(line);
            commented = false;
          }
          break;

        case TAG_ENDFILE:
          DELETE2(line);
          return 0;		// end of parsing

        case TAG_LOCAL:		// <local>, </local>
	  if ((p = strstr(line, "-->"))) {
            commented = false;
            break;
          }
          strcpy(tagobj, "transform");
          const OClass *oclass;
          if ((oclass = OClass::getOClass(tagobj))) {
            tag_type = oclass->type_id;
          }
          if (strstr(line, "<local>")) {
            strcpy(line, "push ");	// <local>
          }
          else {
            strcpy(line, "pop ");	// </local>
          }
          //trace(DBG_FORCE, "LOCAL: type=%d line=%s", tag_type, line);
          if ((wobject = OClass::creatorInstance(tag_type, line)) == NULL) {
	    return -1;
          }
          break;

        case TAG_OBJECT:
          //trace(DBG_FORCE, "object %d: type=%d line=%s", numline, tag_type, line);
	  if ((p = strstr(line, "-->"))) {
            commented = false;
            break;
          }
          // check end of the object </...>
          char closetag[TAG_LEN + 4];
          sprintf(closetag, "</%s>", tagobj);
	  if ((p = strstr(line, closetag)) == NULL) {
            //error("closetag: %s", closetag);
            continue;	// end of object not already reached parses next lines
          }
          // here, we must have reached the end of the object description
          if (tag_type != UNKNOWN_TYPE) {
            if (! OClass::isValidType(tag_type)) {	// type not valid
              error("parse error at line %d (not valid), type=%d line=%s", numline, tag_type, line);
              return -1;
            }
	    // skip <type to get attributes (name, pos, solid, category, descr)
            char *attr = line;
            if (isspace(*line)) {
              attr = skipSpace(line);
            }
            if (*attr == '<') {
              attr = nextSpace(attr);
            }
	    if (*attr == '>') {
	      ++attr;
            }
            if (::g.pref.dbgtrace) trace(DBG_FORCE, "[%d] %s", tag_type, line);
            progression('o');
            ::g.timer.object.start();
            // call the creator of this object with object attributes
            if ((wobject = OClass::creatorInstance(tag_type, attr)) == NULL) {
              error("parse error at line %d (creator instance), type=%d line=%s", numline, tag_type, line);
              return -1;
            }
            ::g.timer.object.stop();
          }
          else		// unknown type
            error("parse error at line %d (unknown type), type=%d line=%s", numline, tag_type, line);
          break;
      }
      DELETE2(line);
      bol = eol + 1;	// begin of next line = end of current line + \n
    }
    else break;	// goto next line
  }
  DELETE2(line);
  line = new char[len - bol + 2];

  // copy the end of previous tmpline into line
  memcpy(line, tmpline + bol, len - bol);
  line[len - bol] = '\0';
  delete[] tmpline;
  return 1;
}

void Parse::printNumline()
{
  error("parse error at line %d (solid)", numline);
}

/* skip attributes */
char * Parse::skipAttribute(char *l)
{
  l = skipQuotes(l);	// first quote
  while (l) {
    char *p;
    p = strchr(l, '"');	// search second quote
    if (! p) {		// not there
      l = nextToken(); 	// call next token
    }
    else {		// found here
      return nextToken();
    }
  }
  return l;
}

/* parse attributes: name pos solid category description / */
char * Parse::parseAttributes(char *l, WObject *wobject)
{
  while (l) {
    if      (! stringcmp(l, "name=")) {
      l = parseName(l, wobject->names.given);
    }
    else if (! stringcmp(l, "pos=")) {
      l = parsePosition(l, wobject->pos);
      continue;
    }
    else if (! stringcmp(l, "solid")) {
      l = parseSolid(l, wobject);
    }
    else if (! stringcmp(l, "category=")) {
      l = parseDescr(l, wobject->names.category);
    }
    else if (! stringcmp(l, "descr=") || ! stringcmp(l, "description=")) {
      l = parseDescr(l,wobject->names.infos);
    }
    else if (! strcmp(l, "/")) {
      l = nextToken();
    }
    else {
      return l;
    }
  }
  return l;
}

/* parse object description names.infos, names.category */
char * Parse::parseDescr(char *l, char *strdst)
{
  l = skipEqual(l);
  if (*l == '"') l++;	// double quoted string

  char tmp[256], *p;
  for (p = tmp; *l != '"' ; ) {
    if (*l) {
      *p++ = *l++;
    }
    else {
      *p++ = ' ';
      l = nextToken();
    }
  }
  *p = '\0';
  strdst = new char[strlen(tmp) + 1];
  strcpy(strdst, tmp);
  return nextToken();
}

/* parse object name: fill names.given */
char * Parse::parseName(char *l, char *name)
{
  return parseQuotedString(l, name, "name");
}

/* parse spacial position and orientation : x y z az ax ay */
char * Parse::parsePosition(char *ptok, Pos &pos)
{
  pos.x = pos.y = pos.z = pos.az = pos.ax = pos.ay = 0;

  if (! stringcmp(ptok, "pos=")) {
    ptok = skipEqual(ptok);
  }

  ptok = skipQuotes(ptok);	// get pos.x
  if (isFloat(ptok))
    pos.x = (float) atof(ptok);
  else
    return nextToken();

  ptok = skipSepar(ptok);	// get pos.y
  if (isFloat(ptok))
    pos.y = (float) atof(ptok);
  else
    return nextToken();

  ptok = skipSepar(ptok);	// get pos.z
  if (isFloat(ptok)) {
    pos.z = (float) atof(ptok);
    if (ptok[strlen(ptok) - 1] == '"') {	// "x,y,z"
      return nextToken();
    }
  }
  else {
    return nextToken();
  }

  ptok = skipSepar(ptok);	// get pos.az
  if (!ptok) {
    return nextToken();
  }
  if (isFloat(ptok)) {
    pos.az = (float) atof(ptok);
    if (ptok[strlen(ptok) - 1] == '"') {	// "x,y,z,az"
      return nextToken();
    }
  }
  else { // pos.az not float
    return ptok;
  }

  ptok = skipSepar(ptok);	// get pos.ax
  if (!ptok) {
    return nextToken();
  }
  if (isFloat(ptok)) {
    pos.ax = (float) atof(ptok);
    if (ptok[strlen(ptok) - 1] == '"') {	// "x,y,z,az,ax"
      return nextToken();
    }
  }
  else { // pos.ax not float
    if (!ptok) return ptok;
  }

  ptok = skipSepar(ptok);	// get pos.ay
  if (!ptok) {
    return nextToken();
  }
  if (isFloat(ptok)) {
    pos.ay = (float) atof(ptok);
    if (ptok[strlen(ptok) - 1] == '"') {	// "x,y,z,az,ax,ay"
      return nextToken();	// normal end
    }
  }
  else { // pos.ay not float
    if (!ptok)
      return ptok;
  }

  return ptok;	//dummy
}

/* parse colors: r g b a */
char * Parse::parseColor(char *ptok, Pos &p)
{
  if (! stringcmp(ptok, "color=")) {
    ptok = skipEqual(ptok);
  }
  ptok = skipQuotes(ptok);
  if (ptok) {
    if (*ptok == 0)
      ptok = nextToken();
  }

  p.x  = (float) atof(ptok);
  ptok = skipSepar(ptok);	// get p.y
  p.y  = (float) atof(ptok);
  ptok = skipSepar(ptok);	// get p.z
  p.z  = (float) atof(ptok);
  if (ptok[strlen(ptok) - 1] == '"') {
    return nextToken();
  }
  ptok = skipSepar(ptok);	// get p.az
  if (!ptok) {
    return nextToken();
  }
  if (!ptok || ptok[strlen(ptok) - 1] == '"') {
    return nextToken();
  }
  if (ptok) p.az = (float) atof(ptok);

  return nextToken();
}

/* parse a guide */
char * Parse::parseGuide(char *ptok, float path[][5], uint8_t *segs)
{
  if ( (! stringcmp(ptok, "path=")) || (! stringcmp(ptok, "guide=")) ) {
    ptok = skipEqual(ptok);
  }
  ptok = skipQuotes(ptok, 0);	// don't check the second '"'
  if (ptok) {
    if (*ptok == 0) {
      ptok = nextToken();
    }
  }

  for (int i=0; ptok && (*ptok != '"'); i++) {
    path[i][0] = (float) atof(ptok); ptok = nextToken(); // x
    path[i][1] = (float) atof(ptok); ptok = nextToken(); // y
    path[i][2] = (float) atof(ptok); ptok = nextToken(); // z
    path[i][3] = (float) atof(ptok); ptok = nextToken(); // speed
    path[i][4] = (float) atof(ptok); // delay
    if (ptok[strlen(ptok) - 1] == '"') {
      *segs = i;
      return nextToken();
    }
    *segs = i;
    ptok = nextToken();
  }
  return nextToken();
}

/* tag <solid : creates a solid and calls its parser */
char * Parse::parseSolid(char *ptok, WObject *wobject)
{
  if (!ptok || !strlen(ptok)) {
    error("parse error at line %d (no solid)", numline-1);
    return nextToken();
  }
  if (*ptok == '<') ptok++;
  if (! strcmp(ptok, "solid")) {
    ptok = nextToken();		// skip tag solid
  }

  Solid *solid = new Solid();

  if (wobject) {
    wobject->addSolid(solid);	// add solid to solidList
  }
  else {
    error("no wobject");
  }

  ptok = solid->parser(ptok);	// calls its parser

  return ptok;
}

/* parse a solid */
char * Parse::parseSolid(char *ptok, const char *sep, WObject *wobject)
{
  if (*ptok == '<') ptok++;
  strtok(ptok, sep);
  return parseSolid(ptok, wobject);
}

/* parse several solids */
void Parse::parseSolids(char *ptok, const char *sep, WObject *wobject)
{
  strtok(ptok, sep);
  while (ptok) {
    ptok = parseSolid(ptok, wobject);
  }
}

/* parse a rotation */
char * Parse::parseRotation(char *ptok, Pos &p)
{
  if (! stringcmp(ptok, "rot=") || ! stringcmp(ptok, "rotation=")) {
    ptok = skipEqual(ptok);
  }
  ptok = skipQuotes(ptok);
  if (ptok) {
    if (*ptok == 0)
      ptok = nextToken();
  }

  p.az = (float) atof(ptok); ptok = skipSepar(ptok);
  p.x  = (float) atof(ptok); ptok = skipSepar(ptok);
  p.y  = (float) atof(ptok); ptok = skipSepar(ptok);
  p.z  = (float) atof(ptok);
  if (!ptok || ptok[strlen(ptok) - 1] == '"') {
    return nextToken();
  }
  return nextToken();
}

/* parse a translation */
char * Parse::parseTranslation(char *ptok, Pos &p)
{
  if (! stringcmp(ptok, "trans=") || ! stringcmp(ptok, "translation=")) {
    ptok = skipEqual(ptok);
  }
  ptok = skipQuotes(ptok);
  if (ptok) {
    if (*ptok == 0)
      ptok = nextToken();
  }

  p.x  = (float) atof(ptok); ptok = skipSepar(ptok);
  p.y  = (float) atof(ptok); ptok = skipSepar(ptok);
  p.z  = (float) atof(ptok);
  if (!ptok || ptok[strlen(ptok) - 1] == '"') {
    return nextToken();
  }
  return nextToken();
}

/* parse an url */
char * Parse::parseUrl(char *ptok, char *url)
{
  if (! stringcmp(ptok, "url="))
    return parseString(ptok, url, "url");
  else
    return parseString(ptok, url);
}

/* parse a world and a channel */
char * Parse::parseWorldAndChannel(char *ptok, char *url, char *chan)
{
  if (! strcmp(ptok, "world")) {	// <world>
    ptok = nextToken();
    ptok = parseString(ptok, url);
    ptok = parseChannel(ptok, chan);
    return nextToken();		// skip </world>
  }
  else {
    ptok = parseString(ptok, url);
    ptok = parseChannel(ptok, chan);
    return ptok;
  }
}

/* parse a channel */
char * Parse::parseChannel(char *ptok, char *channel)
{
  if (ptok) {
    if (! stringcmp(ptok, "channel="))
      ptok = skipEqual(ptok);
    ptok = skipQuotes(ptok);

    if (isdigit((int) *ptok)) strcpy(channel, ptok);
  }
  return nextToken();
}

/* parse a string */
char * Parse::parseString(char *ptok, char *str)
{
  if (ptok) {
    ptok = skipQuotes(ptok);
    char *p = strchr(ptok, '"');
    if (p) *p = 0;
    strcpy(str, ptok);
  }
  return nextToken();
}

/* parse a string */
char * Parse::parseString(char *ptok, char *str, const char *keystr)
{
  if (ptok && !stringcmp(ptok, keystr)) {
    ptok = skipEqual(ptok);
    return parseString(ptok, str);
  }
  return nextToken();
}

/* parse a quoted string */
char * Parse::parseQuotedString(char *ptok, char *str)
{
  char *p, *s = str;

  if (*ptok == '"') ptok++;
  for (p = ptok; p && *p != '"' ; ) {
    if (*p) *s++ = *p++;
    else {
      *s++ = ' ';	// force space
      p = nextToken();
    }
  }
  *s = '\0';
  return nextToken();
}

/* parse a quoted string */
char * Parse::parseQuotedString(char *ptok, char *str, const char *keystr)
{
  if (ptok && !stringcmp(ptok, keystr)) {
    ptok = skipEqual(ptok);
    return parseQuotedString(ptok, str);
  }
  return ptok;
}

char * Parse::parseInt(char *ptok, int *value)
{
  if (ptok) {
    ptok = skipQuotes(ptok);
    if (isInt(ptok)) *value = atoi(ptok);
  }
  return nextToken();
}

char * Parse::parseInt(char *ptok, int *value, const char *keystr)
{
  if (ptok && !stringcmp(ptok, keystr)) {
    ptok = skipEqual(ptok);
    return parseInt(ptok, value);
  }
  return nextToken();
}

char * Parse::parseBool(char *ptok, bool *value)
{
  if (ptok) {
    ptok = skipQuotes(ptok);
    if (ptok && isdigit((int) *ptok)) {
      *value = atoi(ptok) & 0xff;
    }
    else {
      if (! stringcmp(ptok, "true")) {
        *value = 1;
      }
      if (! stringcmp(ptok, "false")) {
        *value = 0;
      }
    }
  }
  return nextToken();
}

char * Parse::parseBool(char *ptok, bool *value, const char *keystr)
{
  if (ptok && !stringcmp(ptok, keystr)) {
    ptok = skipEqual(ptok);
    return parseBool(ptok, value);
  }
  return nextToken();
}

char * Parse::parseUInt8(char *ptok, uint8_t *value)
{
  if (ptok) {
    ptok = skipQuotes(ptok);
    if (ptok && isdigit((int) *ptok)) *value = atoi(ptok) & 0xff;
  }
  return nextToken();
}

char * Parse::parseUInt8(char *ptok, uint8_t *value, const char *keystr)
{
  if (ptok && !stringcmp(ptok, keystr)) {
    ptok = skipEqual(ptok);
    return parseUInt8(ptok, value);
  }
  return nextToken();
}

char * Parse::parseUInt16(char *ptok, uint16_t *value)
{
  if (ptok) {
    ptok = skipQuotes(ptok);
    if (ptok && isdigit((int) *ptok)) *value = atoi(ptok) & 0xffff;
  }
  return nextToken();
}

char * Parse::parseUInt16(char *ptok, uint16_t *value, const char *keystr)
{
  if (ptok && !stringcmp(ptok, keystr)) {
    ptok = skipEqual(ptok);
    return parseUInt16(ptok, value);
  }
  return nextToken();
}

char * Parse::parseFloat(char *ptok, float *value)
{
  if (ptok) {
    ptok = skipQuotes(ptok);
    if (isFloat(ptok)) {
      *value = (float) atof(ptok);
    }
  }
  return nextToken();
}

char * Parse::parseFloat(char *ptok, float *value, const char *keystr)
{
  if (ptok && !stringcmp(ptok, keystr)) {
    ptok = skipEqual(ptok);
    return parseFloat(ptok, value);
  }
  return nextToken();
}

char * Parse::parseVectorf(char *ptok, float *vector, int n)
{
  ptok = skipQuotes(ptok);
  if (! isFloat(ptok)) {
    return ptok;	// parse color name
  }

  for (int i=0; i<n-1; i++) {		// n-1 intervals
    vector[i] = (float) atof(ptok);
    ptok = skipSepar(ptok);
  }
  vector[n-1] = (float) atof(ptok);	// last value
  if (!ptok) {
    return ptok;	// ptok NULL, nextToken will be done by 3f or 5f
  }
  if (ptok[strlen(ptok) - 1] == '"') {
    return nextToken();
  }
  return ptok;		// abnormal string
}

char * Parse::parseVector3f(char *ptok, float *vector)
{
  if (!ptok) {
    return nextToken();	// sanity
  }
  char *p = ptok;
  ptok = parseVectorf(ptok, vector, 3);
  if (!ptok) {
    return nextToken();
  }
  if (ptok != p+1) {	// numeric
    return ptok;	// nextToken done by parseVectorf
  }
  if (! isFloat(ptok)) {
    char colorname[24];
    ptok = parseString(ptok, colorname);
    Color::getRGB(colorname, vector);
    return ptok;	// next token done by parseString
  }
  return nextToken();
}

char * Parse::parseVector5f(char *ptok, float *vector)
{
  if (!ptok)
    return nextToken();	// sanity
  ptok = parseVectorf(ptok, vector, 5);
  if (!ptok)
    return nextToken();
  return ptok;
}

char * Parse::parseVector3fv(char *ptok, V3 *vector)
{
  if (!ptok)
    return nextToken();
  ptok = skipQuotes(ptok);
  if (ptok) {
    vector->v[0] = (float) atof(ptok);
    ptok = skipSepar(ptok);
    vector->v[1] = (float) atof(ptok);
    ptok = skipSepar(ptok);
    vector->v[2] = (float) atof(ptok);
  }
  return nextToken();
}

char * Parse::parseVector3f(char *ptok, float *vector, const char *keystr)
{
  if (ptok && !stringcmp(ptok, keystr)) {
    ptok = skipEqual(ptok);
    return parseVector3f(ptok, vector);
  }
  return nextToken();
}

char * Parse::parseVector3fv(char *ptok, V3 *vector, const char *keystr)
{
  if (ptok && !stringcmp(ptok, keystr)) {
    ptok = skipEqual(ptok);
    return parseVector3fv(ptok, vector);
  }
  return nextToken();
}
