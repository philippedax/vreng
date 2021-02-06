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
#include "solid.hpp"	// getDlist


const OClass Text::oclass(TEXT_TYPE, "Text", Text::creator);

const uint16_t Text::MAXLEN = 64;	// max length of text
const float Text::SCALE = 1/60.;	// default scale
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
  strcpy(names.url, DEF_URL_FONT);	// font's url
  for (int i=0; i<4; i++) color[i] = DEF_COLOR[i];	// color
  verso = false;	// recto
  scale = 1;		// no scale
  loaded = false;
  state = ACTIVE;
  shiftx = shifty = shiftz = shiftaz = shiftax = 0.;
  txf = NULL;
  text = new char[MAXLEN];
}

void Text::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (! stringcmp(l, "font"))  l = parse()->parseString(l, names.url, "font");
    else if (! stringcmp(l, "color")) l = parse()->parseVector3f(l, color, "color");
    else if (! stringcmp(l, "verso")) l = parse()->parseUInt8(l, &verso, "verso");
    else if (! stringcmp(l, "scale")) l = parse()->parseFloat(l, &scale, "scale");
    else                              l = parse()->parseQuotedString(l, text);
  }
  end_while_parse(l);

  text[MAXLEN - 1] = '\0';
}

void Text::makeSolid()
{
  char s[256];

  sprintf(s, "solid shape=\"bbox\" dim=\"%.f .01 .1\" />", (strlen(text)/2)*GLYPHSIZ);
  parse()->parseSolid(s, SEP, this);
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
  loaded = true;
  return loaded;
}

void Text::behavior()
{
  enableBehavior(NO_BBABLE);
  enableBehavior(UNSELECTABLE);
  enableBehavior(SPECIFIC_RENDER);
  setRenderPrior(PRIOR_MEDIUM);

  initStillObject();
}

void Text::inits()
{
  if (! loadFont()) return;

  // sanity check to avoid segfault with "'", "`"
  for (char * p=text; *p; p++) if (*p == '\'' || *p == '`') *p = ' ';
}

Text::Text(char *l)
{
  parser(l);
  behavior();
  inits();
}

Text::Text(const char *t, Pos &pos, float _scale, float *_color)
{
  defaults();
  strcpy(text, t);
  scale = _scale;
  for (int i=0; i<4; i++) color[i] = _color[i];
  behavior();
  inits();
}

void Text::setShifts(float x, float y, float z, float az, float ax)
{
  shiftx = x;
  shifty = y;
  shiftz = z;
  shiftaz = az;
  shiftax = ax;
}

void Text::render()
{
  if (! loaded || (state == INACTIVE)) return;

  glPushAttrib(GL_ALL_ATTRIB_BITS);	// FIXME! if commented, guys have bad color
  glPushMatrix();
  //dax1 glEnable(GL_LIGHTING);
  if (verso)
    glDisable(GL_CULL_FACE); // both faces visibles
  else
    glEnable(GL_CULL_FACE);  // front face

  txf->bindTexture();
  glEnable(GL_TEXTURE_2D);

  //dax glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
  glMaterialfv(GL_FRONT, GL_EMISSION, color);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glTranslatef(pos.x + shiftx, pos.y + shifty, pos.z + shiftz);
  glRotatef(RAD2DEG(pos.az + shiftaz), 0, 0, 1);
  glRotatef(RAD2DEG(pos.ax + shiftax), 1, 0, 0);
  glScalef(SCALE*scale, SCALE*scale, SCALE*scale);

  // render the text
  txf->render(text, (int) strlen(text));

#if 1 //dax1
  // bubble case
  if (dlists[0]) {	// is bubble present ?
    glPushMatrix();
     glRotatef(RAD2DEG(pos.az), 0, 0, 1);
     glTranslatef(pos.x, pos.y, pos.z);
     glEnable(GL_BLEND);
     // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     glDepthMask(GL_FALSE);

     glCallList(dlists[0]);	// display bubble
     glCallList(dlists[1]);	// display bubble arrow

     glDisable(GL_BLEND);
     glDepthMask(GL_TRUE);
    glPopMatrix();
  }
#endif

  glEnable(GL_CULL_FACE);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glPopMatrix();
  glPopAttrib();
}

void Text::quit()
{
  if (text) delete[] text;
}

void Text::funcs()
{}
