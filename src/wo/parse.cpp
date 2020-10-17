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
#include "vreng.hpp"
#include "parse.hpp"	// Parse
#include "wobject.hpp"	// WObject
#include "world.hpp"	// current
#include "solid.hpp"	// Solid
#include "color.hpp"	// getRGB
#include "timer.hpp"	// times
#include "pref.hpp"	// g.pref.dbgtrace
#include "prof.hpp"	// new_world
#include "render.hpp"	// render()
#include "str.hpp"	// stringcmp


// local
static Parse *parse = NULL;

#if 0 //pdtoken
enum {
  OTOK_ERR = 0,
  OTOK_NAME,
  OTOK_DESCR,
  OTOK_CATEGORY,
  OTOK_POS,
  OTOK_SOLID,
  OTOK_TAGEND,
  OTOK_END
};

struct _otokens {
  const char *tokstr;
  const char *tokalias;
  int tokid;
};

static const struct _otokens otokens[] = {
  { "name=", "", OTOK_NAME },
  { "descr=", "description=", OTOK_DESCR },
  { "category=", "", OTOK_CATEGORY },
  { "pos=", "position=", OTOK_POS },
  { "solid", "", OTOK_SOLID },
  { "/", "", OTOK_TAGEND },
  { NULL, NULL, 0 }
};
#endif //pdtoken


Parse::Parse()
{
  parse = this;		// save pointer
  numline = 0;
  tag_type = 0;
  commented = false;
  memset(tagobj, 0, sizeof(tagobj));
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


/** Goto next token, a token is a string separated by a space */
char * Parse::nextToken()
{
  return strtok(NULL, SEP);
}

char * Parse::skipChar(char *p, char c, bool flag)
{
  char *s = p;
  if (p && (p = strchr(p, c)))
    p++;
  else
    if (flag) error("At line %d, missing '%c' in %s", numline-1, c, s);
  return p;
}

char * Parse::skipEqual(char *p)
{
  return skipChar(p, '=', 1);
}

char * Parse::skipOpenBracket(char *p)
{
  return skipChar(p, '[', 1);
}

char * Parse::skipOpenParenthesis(char *p)
{
  char *q = skipChar(p, '(', 0);
  if (q) return q;
  else {
    q = nextToken();
    return skipChar(q, '(', 0);
  }
}

char * Parse::skipCloseParenthesis(char *p)
{
  return skipChar(p, ')', 1);
}

/* Skip double quotes or single quote */
char * Parse::skipQuotes(char *p, bool flag)
{
  if (p && ((*p == '"') || (*p == '\''))) p++;
  return p;
}

/* find next space or next endtag */
char * Parse::nextSpace(char *p)
{
  while (p && *p && !isspace(*p) && *p != '>') p++;
  return p;
}

char * Parse::skipSpace(char *p)
{
  while (p && isspace(*p)) p++;
  return p;
}

char * Parse::skipComma(char *p)
{
  return nextToken();
}

bool Parse::isFloat(const char *p)
{
  return (p && (isdigit((int) *p) || *p == '-' || *p == '+' || *p == '.'));
}

bool Parse::isInt(const char *p)
{
  return (p && (isdigit((int) *p) || *p == '-' || *p == '+'));
}

#if 0 //pdtoken
char * Parse::getTok(char *l, int *tok)
{
  char *t = l;
  const struct _otokens *ptab;

  *tok = OTOK_ERR;
  for (ptab = otokens; ptab->tokstr ; ptab++) {
    if ((!stringcmp(ptab->tokstr, t))) ||
        (!strcmp(ptab->tokalias, t, strlen(ptab->tokalias)))) {
      *tok = ptab->tokid;
      char *p;
      if ((p = strchr(l, '='))) {
        l = skipEqual(l);
        *(l-1) = '\0';
      }
      return l;
    }
  }
  error("getTok: unknown \"%s\" in %s", t, l);
  return l;
}
#endif //pdtoken


/* parse begin of line */
int Parse::parseLine(char *_line, int *ptag_type)
{
  char *pline = strdup(_line);
  char *ptok = skipSpace(pline);

  if (*ptok == '<') {
    if (commented) {
      FREE(pline);
      return TAG_COMMENT;
    }
    ptok++;	// after '<'

    // check <!--
    if ((!stringcmp(ptok, "!--"))) {
      commented = true;
      FREE(pline);
      return TAG_COMMENT;
    }
    else if ((!stringcmp(ptok, "!DOCTYPE"))) {
      FREE(pline);
      return TAG_DOCTYPE;
    }

    // check <vre ... > </vre>
    else if (!stringcmp(ptok, "vre>") || (!stringcmp(ptok, "vre") && isspace(ptok[3]))) {
      FREE(pline);
      return TAG_BEGINFILE;
    }
    else if (!stringcmp(ptok, "/vre>")) {
      FREE(pline);
      return TAG_ENDFILE;
    }

    // check <head> </head>
    else if ((!stringcmp(ptok, "head>"))) {
      FREE(pline);
      return TAG_HEAD;
    }
    else if ((!stringcmp(ptok, "/head>"))) {
      FREE(pline);
      return TAG_HEAD;
    }

    // check <meta ... />
    else if ((!stringcmp(ptok, "meta"))) {
      FREE(pline);
      return TAG_META;
    }

    // check <scene> </scene>
    else if (!stringcmp(ptok, "scene>") || (!stringcmp(ptok, "scene") && isspace(ptok[3]))) {
      FREE(pline);
      return TAG_SCENE;
    }
    else if ((!stringcmp(ptok, "/scene>"))) {
      FREE(pline);
      return TAG_SCENE;
    }

    // check <local> </local>
    else if ((!stringcmp(ptok, "local>"))) {
      FREE(pline);
      return TAG_LOCAL;
    }
    else if ((!stringcmp(ptok, "/local>"))) {
      FREE(pline);
      return TAG_LOCAL;
    }

    else {	// object description follows
      const OClass *oclass = NULL;
      char *q, c = 0;

      if ((q = nextSpace(ptok))) {	// either ' ' or '>'
        c = *q;
        *q = 0;
      }
      if ((oclass = OClass::getOClass(ptok))) {
        *ptag_type = oclass->type_id;
        if (! OClass::isValidType(*ptag_type))
          error("At line %d, *ptag_type=%d ptok=%s", numline, *ptag_type, ptok);
        memset(tagobj, 0, sizeof(tagobj));
        strncpy(tagobj, ptok, TAG_LEN-1);	// memorize object tag
        if (q) *q = c;	// restore sep
        FREE(pline);
        return TAG_OBJECT;
      }
      FREE(pline);
      return TAG_OBJECT;
    }
  }

  // not a '<'
  if ((!stringcmp(ptok, "-->"))) {
    FREE(pline);
    return TAG_COMMENT;
  }
  if (commented) {
    FREE(pline);
    return TAG_COMMENT;
  }
  FREE(pline);
  return TAG_OBJECT;
}

/* parse vre data, called by World::httpReader */
int Parse::parseVreLines(char *buf, int bufsiz)
{
  int maxlen = 0;	// length
  char *tmpline = NULL;
  static char *line = NULL;

  if (! line) {	// need a new line
    tmpline = new char[bufsiz];
    memset(tmpline, 0, bufsiz);
    maxlen = 0;
  }
  else {	// old line: copy previous line in tmpline
    maxlen = strlen(line);
    tmpline = new char[bufsiz + maxlen];
    memset(tmpline, 0, bufsiz);
    strcpy(tmpline, line);
  }

  // copy buf at the end of tmpline
  memcpy(tmpline + maxlen, buf, bufsiz);
  maxlen += bufsiz;

  int iol = 0;	// index in line;
  int bol = 0;	// index begin of line
  numline = 1;	// line number

  while (1) {
    int eol = 0;	// index end of line

    for (iol = bol; iol < maxlen; iol++) {
      if (tmpline[iol] == '\n') {
        numline++;
        if (tmpline[iol-1] == '\\') {	// end of physical line, line follows
          tmpline[iol-1] = tmpline[iol] = ' ';
        }
        else {	// end of logical line
          tmpline[iol] = ' ';	// replace '\n' by ' '
	  eol = iol;
	  iol = maxlen + 1;	// HUGLY! (to do maxlen + 2)
        }
      }
    }

    if (iol == maxlen + 2) {	// end of logical line
      // allocate a new line
      //FIXME segfault if (line) delete[] line;
      line = new char[eol - bol + 2];

      // build the target line from tmpline
      memcpy(line, tmpline + bol, eol - bol);
      line[eol - bol] = '\0';
      //trace(DBG_FORCE, "line: %s %d %d", line, eol, bol);
#ifdef WIN32 // crlf
      if (line[eol - bol - 1] == '\r') line[eol - bol - 1] = '\0';
#endif // WIN32

      // discard empty lines
      if (*line == '\0') {
        if (line)
          delete[] line;
        line = NULL;
        bol = eol + 1;	// begin of next line
        continue;
      }

      char *p = skipSpace(line);

      // discard comments begining with a '#'
      if (*p == '#') {
        delete[] line;
        bol = eol + 1;	// begin of next line
        continue;
      }
      //trace(DBG_FORCE, "line=%s", line);

      WObject *wobject = NULL;

      switch (parseLine(line, &tag_type)) {
        case TAG_BEGINFILE:
        case TAG_HEAD:
        case TAG_SCENE:
          if (line)
            delete[] line;
          line = NULL;
          break;

        case TAG_DOCTYPE:
          if (line)
            delete[] line;
          line = NULL;
          break;		// end of parsing

        case TAG_ENDFILE:
          if (line)
            delete[] line;
          line = NULL;
          return 0;		// end of parsing

        case TAG_META:
	  if ((p = strstr(line, "=\"refresh\"")))
            World::current()->setPersistent(false);
	  if ((p = strstr(line, "/>"))) {
            if (line)
              delete[] line;
            line = NULL;
	  }
          break;

        case TAG_COMMENT:
	  if ((p = strstr(line, "-->"))) {
            commented = false;
            if (line)
              delete[] line;
            line = NULL;
          }
          break;

        case TAG_LOCAL:		// <local>, </local>
          {
            const OClass *oclass;

	    if ((p = strstr(line, "-->"))) {
              commented = false;
              break;
            }
            strcpy(tagobj, "transform");
            if ((oclass = OClass::getOClass(tagobj)))
              tag_type = oclass->type_id;
            if (strstr(line, "<local>")) strcpy(line, "push ");
            else                         strcpy(line, "pop ");
            //trace(DBG_FORCE, "LOCAL: type=%d line=%s", tag_type, line);
            if ((wobject = OClass::creatorInstance(tag_type, line)) == NULL)
	      return -1;
          }
          break;

        case TAG_OBJECT:
          //trace(DBG_FORCE, "type=%d line=%s", tag_type, line);
	  if ((p = strstr(line, "-->"))) {
            commented = false;
            break;
          }

          // check end of the object </...>
          char closetag[TAG_LEN +4];
          sprintf(closetag, "</%s>", tagobj);
	  if ((p = strstr(line, closetag)) == NULL) continue;	// not end of object
          //trace(DBG_FORCE, "</TAG>  %s", line);

          /*
           * here, we must have reached the end of the object
           */
          if (tag_type != UNKNOWN_TYPE) {
            if (! OClass::isValidType(tag_type)) {
              error("At line %d, type=%d line=%s", numline, tag_type, line);
              return -1;
            }

	    // skip <type to get attributes
            char *attr = line;
            if (isspace(*line))
              attr = skipSpace(line);
            if (*attr == '<')
              attr = nextSpace(attr);
            if (*attr == '>')
              ++attr;

            if (::g.pref.dbgtrace) trace(DBG_FORCE, "[%d] %s", tag_type, line);

            /*
             * call the class creator with object attributes
             */
            progression('o');
            ::g.times.object.start();
            if ((wobject = OClass::creatorInstance(tag_type, attr)) == NULL) {
              error("At line %d, type=%d line=%s", numline, tag_type, line);
              return -1;
            }
#if 0 //dax
            if (new_world > 1)  ::g.render.render();
#endif
            ::g.times.object.stop();
          }
          break;
      }

      if (line)
        delete[] line;
      line = NULL;
      bol = eol + 1;	// begin of next line = end of current line + \n
    }
    else break;	// next buffer
  }

  if (line)
    delete[] line;
  line = NULL;

  line = new char[maxlen - bol + 2];

  // copy the end of previous tmpline into line
  memcpy(line, tmpline + bol, maxlen - bol);
  line[maxlen - bol] = '\0';

  delete[] tmpline;
  return 1;
}

void Parse::printNumline()
{
  error("error at line: %d", numline);
}


/* parse tag : tokenize the line */
char * WObject::tokenize(char *l)
{
  // remove close tag
  Parse *parser = parse();
  char closetag[Parse::TAG_LEN +2];
  if (*parser->tagobj) {
    sprintf(closetag, "</%s", parser->tagobj);
    char *p = strstr(l, closetag);	// </type>
    if (p) *p = 0;
  }

  //trace(DBG_FORCE, "l=%s", l);

  // save solid string into geometry for MySql purposes
  char *p = strstr(l, "<solid");
  if (p) {
    char *q, *s;
    geometry = new char[strlen(l)];
    strcpy(geometry, ++p);
    for (s = geometry; (q = strstr(s, "/>")) ; ) {
      s = q;	// end of geometry found
      p = strstr(s, "<solid");	// search next solid
      if (p) {
        s = p;
        continue;
      }
      else {
        *s = '\0';
        break;
      }
    }
    *s = '\0';
    for (s = geometry; (p = strchr(s, '<')) ; ) {
      *p = ' ';
      s = ++p;
    }
  }

  // tokenize the object line, make all tokens of this line
  return strtok(l, SEP);
}

/* skip token */
char * Parse::skipAttribute(char *l)
{
  l = skipQuotes(l);	// first quote
  while (l) {
    char *p;
    p = strchr(l, '"');	// search second quote
    if (! p)	// not there
      l = nextToken(); 	// call next token
    else {	// found here
      return nextToken();
    }
  }
  return l;
}

/* parse attributes */
char * Parse::parseAttributes(char *l, WObject *wobject)
{
  while (l) {
    if      (!stringcmp(l, "name=")) l = parseName(l, wobject->names.named);
    else if (!stringcmp(l, "pos=")) { l = parsePosition(l, wobject->pos); continue; }
    else if (!stringcmp(l, "solid")) l = parseSolid(l, wobject);
    else if (!stringcmp(l, "category=")) l = parseDescr(l, wobject->names.category);
    else if (!stringcmp(l, "descr=") || !stringcmp(l, "description=")) l = parseDescr(l, wobject->names.infos);
    else if (!strcmp(l, "/")) l = nextToken();
    else return l;	// child
  }
  return l;
}

/* parse object description names.infos, names.category */
char * Parse::parseDescr(char *l, char *strdst)
{
  l = skipEqual(l);
  if (*l == '"') l++;	// double quoted string

  char tmp[BUFSIZ], *p;
  for (p = tmp; *l != '"' ; ) {
    if (*l) *p++ = *l++;
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

/* parse object name: fill names.named */
char * Parse::parseName(char *l, char *name)
{
  return parseQuotedString(l, name, "name");
}

/* parse spacial position: x y z az ax */
char * Parse::parsePosition(char *ptok, Pos &pos)
{
  if (!stringcmp(ptok, "pos=")) ptok = skipEqual(ptok);
  ptok = skipQuotes(ptok);	// get pos.x
  if (ptok) if (*ptok == 0) ptok = nextToken();
  if (isFloat(ptok)) pos.x = (float) atof(ptok);
  else return nextToken();

  ptok = skipComma(ptok);	// get pos.y
  if (isFloat(ptok)) pos.y = (float) atof(ptok);
  else return nextToken();

  ptok = skipComma(ptok);	// get pos.z
  if (isFloat(ptok)) {
    pos.z = (float) atof(ptok);
    if (ptok[strlen(ptok) - 1] == '"') {	// "x,y,z"
      return nextToken();
    }
  }
  else return nextToken();

  ptok = skipComma(ptok);	// get pos.az or next token
  if (!ptok) return nextToken();
  if (isFloat(ptok)) {
    pos.az = (float) atof(ptok);
    if (ptok[strlen(ptok) - 1] == '"') {	// "x,y,z,az"
      return nextToken();
    }
  }
  else // pos.az not float
    return ptok;

  ptok = skipComma(ptok);	// get pos.ax
  if (!ptok) return nextToken();
  if (isFloat(ptok)) {
    pos.ax = (float) atof(ptok);
    if (ptok[strlen(ptok) - 1] == '"') {	// "x,y,z,az,ax"
      return nextToken();	// normal end
    }
  }
  else // pos.ax not float
    if (!ptok) return ptok;
  return ptok;	//dummy
}

/* parse colors: r g b a */
char * Parse::parseColor(char *ptok, Pos &p)
{
  if (!stringcmp(ptok, "color=")) ptok = skipEqual(ptok);
  ptok = skipQuotes(ptok);
  if (ptok) if (*ptok == 0) ptok = nextToken();

  p.x  = (float) atof(ptok);
  ptok = skipComma(ptok);	// get p.y
  p.y  = (float) atof(ptok);
  ptok = skipComma(ptok);	// get p.z
  p.z  = (float) atof(ptok);
  if (ptok[strlen(ptok) - 1] == '"') return nextToken();
  ptok = skipComma(ptok);	// get p.az
  if (!ptok) return nextToken();
  if (!ptok || ptok[strlen(ptok) - 1] == '"') return nextToken();
  if (ptok) p.az = (float) atof(ptok);
  return nextToken();
}

char * Parse::parsePath(char *ptok, float path[][5], uint16_t *segs)
{
  if (!stringcmp(ptok, "path=")) ptok = skipEqual(ptok);
  ptok = skipQuotes(ptok, 0);	// we don't check the second '"'
  if (ptok) if (*ptok == 0) ptok = nextToken();

  for (int i=0; ptok && (*ptok != '"'); i++) {
    path[i][0] = (float) atof(ptok); ptok = nextToken();
    path[i][1] = (float) atof(ptok); ptok = nextToken();
    path[i][2] = (float) atof(ptok); ptok = nextToken();
    path[i][3] = (float) atof(ptok); ptok = nextToken();
    path[i][4] = (float) atof(ptok);
    if (ptok[strlen(ptok) - 1] == '"') {
      *segs = i;
      return nextToken();
    }
    *segs = i;
    ptok = nextToken();
  }
  return nextToken();
}

char * Parse::parseSolid(char *ptok, WObject *wobject)
{
  if (!ptok || !strlen(ptok)) {
    error("At line %d, no solid", numline-1);
    return nextToken();
  }
  if (! strcmp(ptok, "solid"))
    ptok = nextToken();	// skip tag solid

  Solid *s = new Solid();

  if (wobject)
    wobject->addSolid(s);	// add solid to solidList
  else
    error("no wobject");

  ptok = s->parser(ptok);	// parse solid here
  return ptok;
}

char * Parse::parseSolid(char *ptok, const char *separ, WObject *wobject)
{
  if (*ptok == '<') ptok++;
  strtok(ptok, separ);
  return parseSolid(ptok, wobject);
}

void Parse::parseSolids(char *ptok, const char *separ, WObject *wobject)
{
  strtok(ptok, separ);
  while (ptok) ptok = parseSolid(ptok, wobject);
}

char * Parse::parseRotation(char *ptok, Pos &p)
{
  if (!stringcmp(ptok, "rot=") || !stringcmp(ptok, "rotation=")) ptok = skipEqual(ptok);
  ptok = skipQuotes(ptok);
  if (ptok) if (*ptok == 0) ptok = nextToken();

  p.az = (float) atof(ptok); ptok = skipComma(ptok);
  p.x  = (float) atof(ptok); ptok = skipComma(ptok);
  p.y  = (float) atof(ptok); ptok = skipComma(ptok);
  p.z  = (float) atof(ptok);
  if (!ptok || ptok[strlen(ptok) - 1] == '"') return nextToken();
  return nextToken();
}

char * Parse::parseTranslation(char *ptok, Pos &p)
{
  if (!stringcmp(ptok, "trans=") || !stringcmp(ptok, "translation=")) ptok = skipEqual(ptok);
  ptok = skipQuotes(ptok);
  if (ptok) if (*ptok == 0) ptok = nextToken();

  p.x  = (float) atof(ptok); ptok = skipComma(ptok);
  p.y  = (float) atof(ptok); ptok = skipComma(ptok);
  p.z  = (float) atof(ptok);
  if (!ptok || ptok[strlen(ptok) - 1] == '"') return nextToken();
  return nextToken();
}

char * Parse::parseUrl(char *ptok, char *url)
{
  if (!stringcmp(ptok, "url=")) return parseString(ptok, url, "url");
  else                           return parseString(ptok, url);
}

char * Parse::parseWorldAndChannel(char *ptok, char *url, char *chan)
{
  if (!strcmp(ptok, "world")) {	// <world>
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

char * Parse::parseChannel(char *ptok, char *channel)
{
  if (ptok) {
    if (!stringcmp(ptok, "channel=")) ptok = skipEqual(ptok);
    ptok = skipQuotes(ptok);

    if (isdigit((int) *ptok)) strcpy(channel, ptok);
  }
  return nextToken();
}

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

char * Parse::parseString(char *ptok, char *str, const char *keystr)
{
  if (ptok && !stringcmp(ptok, keystr)) {
    ptok = skipEqual(ptok);
    return parseString(ptok, str);
  }
  return nextToken();
}

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
    if (isFloat(ptok)) *value = (float) atof(ptok);
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
  if (! isFloat(ptok)) return ptok;	// parse color name

  for (int i=0; i<n-1; i++) {		// n-1 intervals
    vector[i] = (float) atof(ptok);
    ptok = skipComma(ptok);
  }
  vector[n-1] = (float) atof(ptok);	// last value
  if (!ptok) return ptok;	// ptok NULL, nextToken will be done by 3f or 5f
  if (ptok[strlen(ptok) - 1] == '"') return nextToken();
  return ptok;		// abnormal string
}

char * Parse::parseVector3f(char *ptok, float *vector)
{
  if (!ptok) return nextToken();	// sanity
  char *p = ptok;
  ptok = parseVectorf(ptok, vector, 3);
  if (!ptok) return nextToken();
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
  if (!ptok) return nextToken();	// sanity
  ptok = parseVectorf(ptok, vector, 5);
  if (!ptok) return nextToken();
  return ptok;
}

char * Parse::parseVector3fv(char *ptok, V3 *vector)
{
  if (!ptok) return nextToken();
  ptok = skipQuotes(ptok);
  if (ptok) {
    vector->v[0] = (float) atof(ptok);
    ptok = skipComma(ptok);
    vector->v[1] = (float) atof(ptok);
    ptok = skipComma(ptok);
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
