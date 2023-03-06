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
#include "text.hpp"
#include "txf.hpp"	// Txf
#include "world.hpp"	// current
#include "user.hpp"	// User


const OClass Text::oclass(TEXT_TYPE, "Text", Text::creator);

const uint16_t Text::MAXLEN = 128;	// max length of text
const float Text::RATIO_SCALE = 1/60.;	// default scale ratio
const float Text::GLYPHSIZ = 0.03;	// 3 cm

// local
static const GLfloat DEF_COLOR[4] = {0, 1, 0, 1};	// green


/* creation from a file */
WObject * Text::creator(char *l)
{
  return new Text(l);
}

void Text::defaults()
{
  verso = false;	// recto
  scale = 1;		// no scale
  havefont = false;
  state = ACTIVE;
  shiftx = shifty = shiftz = shiftaz = shiftax = 0.;
  txf = NULL;
  textstr = new char[MAXLEN];
  strcpy(names.url, DEF_URL_TXF);	// font's url
  for (int i=0; i<4; i++) {
    color[i] = DEF_COLOR[i];	// color
  }
}

void Text::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "font"))  l = parseString(l, names.url, "font");
    else if (! stringcmp(l, "color")) l = parseVector3f(l, color, "color");
    else if (! stringcmp(l, "verso")) l = parseUInt8(l, &verso, "verso");
    else if (! stringcmp(l, "scale")) l = parseFloat(l, &scale, "scale");
    else                              l = parse()->parseQuotedString(l, textstr);
  }
  end_while_parse(l);

  textstr[MAXLEN - 1] = '\0';
}

void Text::makeSolid()
{
  char s[256];

  sprintf(s, "solid shape=\"bbox\" dim=\"%f .01 .1\" />", (strlen(textstr)*GLYPHSIZ / 2));
  parseSolid(s);
}

/* Loads the font */
bool Text::loadFont()
{
  // gets the font
  if ((txf = Txf::load(names.url)) == NULL) {
    error("Text: can't load font %s", names.url);
    return false;
  }
  // build the font texture
  txf->buildTexture(0, GL_TRUE);	// mipmaps true
  havefont = true;
  return havefont;
}

void Text::behavior()
{
  enableBehavior(NO_BBABLE);
  enableBehavior(UNSELECTABLE);
  enableBehavior(SPECIFIC_RENDER);

  initMobileObject(0);
}

void Text::inits()
{
  if (! loadFont()) return;

  // sanity check to avoid segfault with "'", "`"
  for (char * p=textstr; *p; p++) if (*p == '\'' || *p == '`') {
    *p = ' ';
  }
}

/* Constructors. */
Text::Text(char *l)
{
  parser(l);
  behavior();
  makeSolid();
  inits();
}

Text::Text(const char *t, Pos &pos, float _scale, const float *_color)
{
  defaults();
  strcpy(textstr, t);
  scale = _scale;
  for (int i=0; i<4; i++) {
    color[i] = _color[i];
  }
  behavior();
  makeSolid();
  inits();
}

Text::Text(const char *t, Pos &pos, float _scale, const float *_color, bool _face)
{
  defaults();
  verso = _face;
  strcpy(textstr, t);
  scale = _scale;
  for (int i=0; i<4; i++) {
    color[i] = _color[i];
  }
  behavior();
  makeSolid();
  inits();
}

/* Sets text positions */
void Text::setPos(float x, float y, float z, float az, float ax)
{
  shiftx = x;
  shifty = y;
  shiftz = z;
  shiftaz = az;
  shiftax = ax;
}

/* Returns lenght of text */
float Text::lenText(const char *_text)
{
  return (strlen(_text)+1) * GLYPHSIZ;
}

void Text::changePosition(float lasting)
{
  if (! localuser) return;

  pos.x = shiftx;
  pos.y = shifty;
  pos.z = shiftz;
  pos.az = shiftaz;
  echo("text: %.1f %.1f %.1f", pos.x,pos.y,pos.z);
}

void Text::expire()
{
  toDelete();
}

void Text::render()
{
  if (! havefont || (state == INACTIVE)) return;

  glPushMatrix();
   if (verso)
     glDisable(GL_CULL_FACE); // both faces visibles
   else
     glEnable(GL_CULL_FACE);  // front face

   txf->bindTexture();
   glEnable(GL_TEXTURE_2D);
   //dax glMaterialfv(GL_FRONT, GL_EMISSION, color);
   glMaterialfv(GL_FRONT, GL_AMBIENT, color);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   glTranslatef(pos.x + shiftx, pos.y + shifty, pos.z + shiftz);
   glRotatef(RAD2DEG(pos.az + shiftaz), 0, 0, 1);
   glRotatef(RAD2DEG(pos.ax + shiftax), 1, 0, 0);
   glScalef(RATIO_SCALE*scale, RATIO_SCALE*scale, RATIO_SCALE*scale);

   // render the text
   txf->render(textstr, (int) strlen(textstr));

   glEnable(GL_CULL_FACE);
   glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

void Text::quit()
{
  if (textstr) delete[] textstr;
}

void Text::funcs()
{}
