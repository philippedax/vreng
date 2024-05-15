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
// parse.cpp
//
// Parsing vre files
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "parse.hpp"	// Parse
#include "wobject.hpp"	// WO
#include "world.hpp"	// current
#include "solid.hpp"	// Solid
#include "color.hpp"	// getRGB
#include "timer.hpp"	// timer
#include "pref.hpp"	// g.pref.dbgtrace
#include "str.hpp"	// stringcmp


// local
static Parse *parse = NULL;

/* aliases struct */
struct sAlias {
  const char *objalias;
  const char *objreal;
};

//
// aliases
//
static const struct sAlias aliases[] = {
  //{ "old_name", "new_name" },
  { "ftp",        "download" },
  { "animx3d",    "carousel" },
  { "android",    "humanoid" },
  { "head",       "face" },
  { "travelator", "travolator" },
  { NULL, NULL }
};


/** Constructor */
Parse::Parse()
{
  parse = this;		// save pointer
  numline = 0;
  tag_type = 0;
  commented = false;
  inscene = false;
}

/** Destructor */
Parse::~Parse()
{
  parse = NULL;
}

/** Gets parse pointer - accessor */
Parse * Parse::getParse()
{
  if (! parse)
    parse = new Parse();
  return parse;
}

/**
 * Gets next token,
 * a token is a string delimited by a separator (TOK_SEP=" ,<>\t\n")
 */
char * Parse::nextToken() const
{
  return strtok(NULL, TOK_SEP);
}

/** Skips a char 'c' */
char * Parse::skipChar(char *p, char c) const
{
  char *s = p;
  if (p && (p = strchr(p, c)))
    p++;
  else
    error("parse error at line %d (missing '%c' in %s)", numline-1, c, s);
  return p;
}

/** Skips an '=' */
char * Parse::skipEqual(char *p) const
{
  return skipChar(p, '=');
}

/** Skips double quotes or single quote */
char * Parse::skipQuotes(char *p) const
{
  if (p && ((*p == '"') || (*p == '\''))) p++;
  return p;
}

/** Finds next space or next endtag */
char * Parse::nextSpace(char *p) const
{
  while (p && *p && !isspace(*p) && *p != '>') p++;
  return p;
}

/** Skips a space */
char * Parse::skipSpace(char *p) const
{
  while (p && isspace(*p)) p++;
  return p;
}

/** Checks if is a float */
bool Parse::isFloat(const char *p) const
{
  return (p && (isdigit(static_cast<int>(*p)) || *p == '-' || *p == '+' || *p == '.'));
}

/** Checks if is an int */
bool Parse::isInt(const char *p) const
{
  return (p && (isdigit(static_cast<int>(*p)) || *p == '-' || *p == '+'));
}

/** Parses a vre line : returns tag number */
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
      //echo("#%s", ptok);
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
      return TAG_BEGINVRE;
    }
    else if (! stringcmp(ptok, "/vre")) {
      FREE(line);
      return TAG_ENDVRE;
    }

    // check <head> </head>
    else if ((! stringcmp(ptok, "head"))) {
      FREE(line);
      if (! inscene) return TAG_HEAD;
    }
    else if ((! stringcmp(ptok, "/head"))) {
      FREE(line);
      if (! inscene) return TAG_HEAD;
    }

    // check <meta ... />
    else if ((! stringcmp(ptok, "meta"))) {
      FREE(line);
      return TAG_META;
    }
    else if ((! stringcmp(ptok, "/meta"))) {
      FREE(line);
      return TAG_META;
    }

    // check <scene> </scene>
    else if (! stringcmp(ptok, "scene")) {
      FREE(line);
      inscene = true;	// fix ambigous <head> tag
      return TAG_SCENE;
    }
    else if ((! stringcmp(ptok, "/scene"))) {
      FREE(line);
      inscene = false;	// fix ambigous <head> tag
      return TAG_SCENE;
    }

    // check <local> </local>
    else if ((! stringcmp(ptok, "local"))) {
      FREE(line);
      return TAG_LOCAL;
    }
    else if ((! stringcmp(ptok, "/local"))) {
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

    //
    // handles aliases here
    //
    const struct sAlias *palias;
    for (palias = aliases; palias->objalias; palias++) {
      if (! strcmp(ptok, palias->objalias)) {
        strcpy(tagobj, palias->objreal);	// substitute by real tag
        //echo("parse: %d %s", numline, tagobj);
        break;
      }
    }

    //
    // identifies the object
    //
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
    else {
      error("parse error at line %d (invalid type), *ptag_type=%d ptok=%s",
            numline, *ptag_type, tagobj);
      echo("line: %s", line);
      fatal("bad tag %s", tagobj);
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

/**
 * Parses vre data,
 * called by World::reader
 */
int Parse::parseVreFile(char *buf, int buflen)
{
  int len = 0;	// line length
  char *workline = NULL;
  static char *line = NULL;

  if (! line) {	// need a new line
    workline = new char[buflen];
    len = 0;
  }
  else {	// old line: copy previous line in workline
    len = strlen(line);
    workline = new char[buflen + len];
    strcpy(workline, line);
  }

  // copy buf at the end of workline
  memcpy(workline + len, buf, buflen);
  len += buflen;

  int iol = 0;	// index in line;
  int bol = 0;	// index begin of line

  //
  // parses all lines of the vre file
  //
  while (1) {
    int eol = 0;		// index end of line

    for (iol = bol; iol < len; iol++) {
      if (workline[iol] == '\n') {
        numline++;
        if (workline[iol-1] == '\\') {	// end of physical line, line follows
          workline[iol-1] = workline[iol] = ' ';
        }
        else {			// end of logical line
          workline[iol] = ' ';	// replace '\n' by ' '
	  eol = iol;
	  iol = len + 1;	// HUGLY! (to do len + 2)
        }
      }
    }
    if (iol == len + 2) {		// end of logical line
      line = new char[eol - bol + 2];	// allocate a new line

      // build line from workline
      memcpy(line, workline + bol, eol - bol);
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

      WO *wobject = NULL;
      uint8_t tag = parseLine(line, &tag_type);

      switch (tag) {
        case TAG_DOCTYPE:
        case TAG_BEGINVRE:
          DELETE2(line);
          bol = eol + 1;	// begin of next line
          continue;
        case TAG_HEAD:
          DELETE2(line);
          bol = eol + 1;	// begin of next line
          continue;
        case TAG_META:
	  if ((p = strstr(line, "=\"refresh\""))) {
            World::current()->persistent = false;
          }
	  if ((p = strstr(line, "/>"))) {
            DELETE2(line);
          }
          break;
        case TAG_COMMENT:
	  if ((p = strstr(line, "-->"))) {
            //echo("end comment %s", line);
            DELETE2(line);
            commented = false;
          }
          break;
        case TAG_SCENE:
          //inscene = true;
          DELETE2(line);
          bol = eol + 1;	// begin of next line
          continue;	// or break
        case TAG_ENDVRE:
          DELETE2(line);
          return 0;		// end of parsing
        case TAG_LOCAL:		// <local>, </local>
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
          // check end of the object </...>
          char closetag[TAG_LEN + 4];
          sprintf(closetag, "</%s>", tagobj);
	  if ((p = strstr(line, closetag)) == NULL) {
            //echo("closetag: %s", closetag);
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
            //debug if (::g.pref.dbgtrace) echo("[%d] %s", tag_type, line);
            progression('o');
            ::g.timer.object.start();
            // call the creator() method of this object with object attributes
            if ((wobject = OClass::creatorInstance(tag_type, attr)) == NULL) {
              error("parse error at line %d (creator), type=%d line=%s", numline, tag_type, line);
              return -1;
            }
            ::g.timer.object.stop();
          }
          else {	// unknown type
            error("parse error at line %d (unknown type), type=%d line=%s", numline, tag_type, line);
          }
          break;
      }
      DELETE2(line);
      bol = eol + 1;	// begin of next line = end of current line + \n
    }
    else {
      break;		// goto next line
    }
  }
  DELETE2(line);
  line = new char[len - bol + 2];

  // copy the end of previous workline into line
  memcpy(line, workline + bol, len - bol);
  line[len - bol] = '\0';
  delete[] workline;
  return 1;
}

void Parse::printNumline()
{
  error("parse error at line %d", numline);
}

/** skips attributes */
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

/**
 * Parses attributes:
 *   name pos url solid category description
 *   be carefull : the line has been tokenized by separators TOK_SEP=" ,<>\t\n"
 */
char * Parse::parseAttributes(char *l, WO *wobject)
{
  while (l) {
    if      (! stringcmp(l, "name=")) {
      if (! wobject->name.given)
        wobject->name.given = new char[OBJNAME_LEN];	// alloc name.given
      l = parseName(l, wobject->name.given);
    }
    else if (! stringcmp(l, "pos=")) {
      l = parsePosition(l, wobject->pos);
      continue;
    }
    else if ( ! stringcmp(l, "solid") || ! stringcmp(l, "geom") ) {
      l = parseSolid(l, wobject);
    }
    else if (! stringcmp(l, "category=")) {
      l = parseDescr(l, wobject->name.category);
    }
    else if ( ! stringcmp(l, "descr=") || ! stringcmp(l, "description=") ) {
      l = parseDescr(l, wobject->name.infos);
    }
    else if (! strcmp(l, "/")) {
      l = nextToken();
    }
#if 0 //dax
    else if (! stringcmp(l, "!--")) {	// <!--
      echo("< %s", l);
      commented = true;
      while (stringcmp(l, "--")) {	// -->
        l = nextToken();		// commented
        echo("# %s", l);
      }
      echo("> %s", l);
      commented = false;
      l = nextToken();
    }
#endif
    else {
      return l;
    }
  }
  return l;
}

/** Parses object description name.infos, name.category */
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

/** Parses object name: fill name.given */
char * Parse::parseName(char *l, char *name)
{
  return parseQuotedString(l, name, "name");
}

/** Parses caption text */
char * Parse::parseCaption(char *l, char *caption)
{
  return parseQuotedString(l, caption, "caption");
}

char * Parse::parseCaption(char *l, char *caption, const char *keystr)
{
  return parseQuotedString(l, caption, keystr);
}

/**
 * Parses spacial position and orientation : x y z az ax ay
 */
char * Parse::parsePosition(char *ptok, Pos &pos)
{
  pos.x = pos.y = pos.z = pos.az = pos.ax = pos.ay = 0;

  if (! stringcmp(ptok, "pos=")) {
    ptok = skipEqual(ptok);
  }
  ptok = skipQuotes(ptok);	// get pos.x
  if (isFloat(ptok))
    pos.x = static_cast<float>(atof(ptok));
  else
    return nextToken();
  ptok = nextToken();		// get pos.y
  if (isFloat(ptok))
    pos.y = static_cast<float>(atof(ptok));
  else
    return nextToken();
  ptok = nextToken();		// get pos.z
  if (isFloat(ptok)) {
    pos.z = static_cast<float>(atof(ptok));
    if (ptok[strlen(ptok) - 1] == '"') {	// "x,y,z"
      return nextToken();
    }
  }
  else {
    return nextToken();
  }
  ptok = nextToken();		// get pos.az
  if (!ptok) {
    return nextToken();
  }
  if (isFloat(ptok)) {
    pos.az = static_cast<float>(atof(ptok));
    if (ptok[strlen(ptok) - 1] == '"') {	// "x,y,z,az"
      return nextToken();
    }
  }
  else { // pos.az not float
    return ptok;
  }
  ptok = nextToken();		// get pos.ax
  if (!ptok) {
    return nextToken();
  }
  if (isFloat(ptok)) {
    pos.ax = static_cast<float>(atof(ptok));
    if (ptok[strlen(ptok) - 1] == '"') {	// "x,y,z,az,ax"
      return nextToken();
    }
  }
  else { // pos.ax not float
    if (!ptok) return ptok;
  }
  ptok = nextToken();		// get pos.ay
  if (!ptok) {
    return nextToken();
  }
  if (isFloat(ptok)) {
    pos.ay = static_cast<float>(atof(ptok));
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

/** Parses colors: r g b a */
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

  p.x  = static_cast<float>(atof(ptok));
  ptok = nextToken();		// get p.y
  p.y  = static_cast<float>(atof(ptok));
  ptok = nextToken();		// get p.z
  p.z  = static_cast<float>(atof(ptok));
  if (ptok[strlen(ptok) - 1] == '"') {
    return nextToken();
  }
  ptok = nextToken();		// get p.az
  if (!ptok) {
    return nextToken();
  }
  if (!ptok || ptok[strlen(ptok) - 1] == '"') {
    return nextToken();
  }
  if (ptok) p.az = static_cast<float>(atof(ptok));

  return nextToken();
}

/** Parses a guide */
char * Parse::parseGuide(char *ptok, float path[][5], uint8_t *segs)
{
  if ( (! stringcmp(ptok, "path=")) || (! stringcmp(ptok, "guide=")) ) {
    ptok = skipEqual(ptok);
  }
  ptok = skipQuotes(ptok);	// don't check the second '"'
  if (ptok) {
    if (*ptok == 0) {
      ptok = nextToken();
    }
  }
  for (int i=0; ptok && (*ptok != '"'); i++) {
    path[i][0] = static_cast<float>(atof(ptok)); ptok = nextToken(); // x
    path[i][1] = static_cast<float>(atof(ptok)); ptok = nextToken(); // y
    path[i][2] = static_cast<float>(atof(ptok)); ptok = nextToken(); // z
    path[i][3] = static_cast<float>(atof(ptok)); ptok = nextToken(); // speed
    path[i][4] = static_cast<float>(atof(ptok)); // delay
    if (ptok[strlen(ptok) - 1] == '"') {
      *segs = i;
      return nextToken();
    }
    *segs = i;
    ptok = nextToken();
  }
  return nextToken();
}

/**
 * Parses a solid
 * tag <solid || <geom
 * creates a solid and calls its parser
 */
char * Parse::parseSolid(char *ptok, WO *wobject)
{
  if ( !ptok || !strlen(ptok) ) {
    error("parse error at line %d (no solid)", numline-1);
    return nextToken();
  }
  if (*ptok == '<') ptok++;
  if (! strcmp(ptok, "solid") || ! strcmp(ptok, "geom") ) {
    ptok = nextToken();		// skip tag solid
  }

  Solid *solid = new Solid();	// creates the solid

  if (wobject) {
    wobject->addSolid(solid);	// add solid to solidList
  }
  else {
    error("parseSolid: no wobject");
  }

  //
  // calls the solid's parser
  //
  ptok = solid->parser(ptok);	// calls its parser
  return ptok;
}

/** Parses a solid */
char * Parse::parseSolid(char *ptok, const char *sep, WO *wobject)
{
  if (*ptok == '<') ptok++;
  strtok(ptok, sep);
  return parseSolid(ptok, wobject);
}

/** Parses several solids */
void Parse::parseSolids(char *ptok, const char *sep, WO *wobject)
{
  strtok(ptok, sep);		// a solid is tokenized
  while (ptok) {
    ptok = parseSolid(ptok, wobject);
  }
}

/** Parses a rotation */
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
  p.az = static_cast<float>(atof(ptok)); ptok = nextToken();
  p.x  = static_cast<float>(atof(ptok)); ptok = nextToken();
  p.y  = static_cast<float>(atof(ptok)); ptok = nextToken();
  p.z  = static_cast<float>(atof(ptok));
  if (!ptok || ptok[strlen(ptok) - 1] == '"') {
    return nextToken();
  }
  return nextToken();
}

/** Parses a translation */
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
  p.x  = static_cast<float>(atof(ptok)); ptok = nextToken();
  p.y  = static_cast<float>(atof(ptok)); ptok = nextToken();
  p.z  = static_cast<float>(atof(ptok));
  if (!ptok || ptok[strlen(ptok) - 1] == '"') {
    return nextToken();
  }
  return nextToken();
}

/** Parses an url */
char * Parse::parseUrl(char *ptok, char *url)
{
  //echo("url: %s", ptok);
  //dax1 url = new char[URL_LEN];
  if (! stringcmp(ptok, "url="))
    return parseString(ptok, url, "url");
  else
    return parseString(ptok, url);
}

/** Parses a world and a channel */
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

/** Parses a channel */
char * Parse::parseChannel(char *ptok, char *channel)
{
  if (ptok) {
    if (! stringcmp(ptok, "channel="))
      ptok = skipEqual(ptok);
    ptok = skipQuotes(ptok);

    if (isdigit(static_cast<int>(*ptok))) strcpy(channel, ptok);
  }
  return nextToken();
}

/** Parses a string */
// we assume that there is no space in the string, else loop bug for the caller
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

/** Parses a string */
char * Parse::parseString(char *ptok, char *str, const char *keystr)
{
  if (ptok && !stringcmp(ptok, keystr)) {
    ptok = skipEqual(ptok);
    return parseString(ptok, str);
  }
  return nextToken();
}

/** Parses a quoted text */
char * Parse::parseQuotedString(char *ptok, char *str)
{
  char *p, *s = str;

  if (*ptok == '"') ptok++;
  for (p = ptok; p && *p != '"' ; ) {
    if (*p) {
      *s++ = *p++;
    }
    else {
      *s++ = ' ';	// force space
      p = nextToken();	// rest of text
    }
  }
  *s = '\0';
  return nextToken();
}

/** Parses a quoted text */
char * Parse::parseQuotedString(char *ptok, char *str, const char *keystr)
{
  if (ptok && ! stringcmp(ptok, keystr)) {
    ptok = skipEqual(ptok);
    return parseQuotedString(ptok, str);
  }
  return ptok;
}

/** Parses an int */
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

/** Parses a bool */
char * Parse::parseBool(char *ptok, bool *value)
{
  if (ptok) {
    ptok = skipQuotes(ptok);
    if (ptok && isdigit(static_cast<int>(*ptok))) {
      *value = atoi(ptok) % 2;
    }
    else {
      if (! stringcmp(ptok, "true") || ! stringcmp(ptok, "on")) {
        *value = 1;
      }
      if (! stringcmp(ptok, "false") || ! stringcmp(ptok, "off")) {
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

/** Parses an uint8_t */
char * Parse::parseUInt8(char *ptok, uint8_t *value)
{
  if (ptok) {
    ptok = skipQuotes(ptok);
    if (ptok && isdigit(static_cast<int>(*ptok))) *value = atoi(ptok) & 0xff;
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

/** Parses an uint16_t */
char * Parse::parseUInt16(char *ptok, uint16_t *value)
{
  if (ptok) {
    ptok = skipQuotes(ptok);
    if (ptok && isdigit(static_cast<int>(*ptok))) *value = atoi(ptok) & 0xffff;
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

/** Parses a float */
char * Parse::parseFloat(char *ptok, float *value)
{
  if (ptok) {
    ptok = skipQuotes(ptok);
    if (isFloat(ptok)) {
      *value = static_cast<float>(atof(ptok));
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

/** Parses a vector of floats */
char * Parse::parseVectorf(char *ptok, float *vector, int n)
{
  ptok = skipQuotes(ptok);
  if (! isFloat(ptok)) {
    return ptok;	// parse color name
  }
  for (int i=0; i<n-1; i++) {		// n-1 intervals
    vector[i] = static_cast<float>(atof(ptok));
    ptok = nextToken();
  }
  vector[n-1] = static_cast<float>(atof(ptok));	// last value
  if (!ptok) {
    return ptok;	// ptok NULL, nextToken will be done by 3f or 5f
  }
  if (ptok[strlen(ptok) - 1] == '"') {
    return nextToken();
  }
  return ptok;		// abnormal string
}

/** Parses a vector of 3 floats */
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

char * Parse::parseVector3f(char *ptok, float *vector, const char *keystr)
{
  if (ptok && !stringcmp(ptok, keystr)) {
    ptok = skipEqual(ptok);
    return parseVector3f(ptok, vector);
  }
  return nextToken();
}

/** Parses a vector V3 of floats */
char * Parse::parseVector3fv(char *ptok, V3 *vector)
{
  if (!ptok) {
    return nextToken();
  }
  ptok = skipQuotes(ptok);
  if (ptok) {
    vector->v[0] = static_cast<float>(atof(ptok));
    ptok = nextToken();
    vector->v[1] = static_cast<float>(atof(ptok));
    ptok = nextToken();
    vector->v[2] = static_cast<float>(atof(ptok));
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

/** Parses a vector of 5 floats */
char * Parse::parseVector5f(char *ptok, float *vector)
{
  if (!ptok) {
    return nextToken();	// sanity
  }
  ptok = parseVectorf(ptok, vector, 5);
  if (!ptok)
    return nextToken();
  return ptok;
}
