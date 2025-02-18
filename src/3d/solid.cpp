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
// solid.cpp
//
// Solid handling
//
// Authors: Fabrice Bellard, Philippe Dax
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "solid.hpp"
#include "draw.hpp"	// STYLE
#include "object.hpp"	// Object
#include "parse.hpp"	// printNumline
#include "format.hpp"	// getModelByUrl
#include "stat.hpp"	// new_solid
#include "pref.hpp"	// ::g.pref
#include "md2.hpp"	// Md2
#include "obj.hpp"	// Obj
#include "man.hpp"	// draw
#include "wings.hpp"	// draw
#include "car.hpp"	// draw
#include "teapot.hpp"	// draw
#include "wheel.hpp"	// Wheel
#include "render.hpp"	// ::g.render
#include "texture.hpp"	// Texture
#include "color.hpp"	// Color
#include "user.hpp"	// localuser
#include "netobj.hpp"	// declareDelta

#include <list>


// local

struct sStokens {
  const char *tokstr;
  const char *tokalias;
  int tokid;
};

static const struct sStokens stokens[] = {
  { "solid", "geom", STOK_SOLID },
  { "none", "fictif", STOK_NONE },
  { "shape", "shape", STOK_SHAPE },
  { "box", "cube", STOK_BOX },
  { "man", "human", STOK_MAN },
  { "guy", "boy", STOK_GUY },
  { "humanoid", "android", STOK_HUMANOID },
  { "car", "car", STOK_CAR },
  { "sphere", "ball", STOK_SPHERE },
  { "cone", "cylinder", STOK_CONE },
  { "torus", "torus", STOK_TORUS },
  { "rect", "rectangle", STOK_RECT },
  { "disk", "disk", STOK_DISK },
  { "line", "line", STOK_LINE },
  { "pent", "pentagon", STOK_PENTAGON },
  { "pyra", "pyramid", STOK_PYRAMID },
  { "oct", "octagon", STOK_OCTAGON },
  { "tri", "triangle", STOK_TRIANGLE },
  { "cir", "circle", STOK_CIRCLE },
  { "ell", "ellipse", STOK_ELLIPSE },
  { "pt", "point", STOK_POINT },
  { "statue", "statue", STOK_STATUE },
  { "bb", "bbox", STOK_BBOX },
  { "bs", "bsphere", STOK_BSPHERE },
  { "cross", "cross", STOK_CROSS },
  { "sphere+torus", "dsphere", STOK_SPHERE_TORUS },
  { "sphere+disk", "saucer", STOK_SPHERE_DISK },
  { "cone+disk", "hat", STOK_CONE_DISK },
  { "wheel", "wheel", STOK_WHEEL },
  { "hel","helix", STOK_HELIX},
  { "teapot","teapot", STOK_TEAPOT},
  { "ws","walls", STOK_WALLS},
  { "wings","wings", STOK_WINGS},
  { "url", "url", STOK_URL },
  { "dim", "size", STOK_SIZE },
  { "r", "radius", STOK_RADIUS },
  { "rb", "ri", STOK_RADIUS },
  { "radius2", "rc", STOK_RADIUS2 },
  { "rt", "re", STOK_RADIUS2 },
  { "radius3", "radius3", STOK_RADIUS3 },
  { "ptsize", "pointsize", STOK_PTSIZE },
  { "h", "height", STOK_HEIGHT },
  { "l", "length", STOK_LENGTH },
  { "w", "width", STOK_WIDTH },
  { "d", "depth", STOK_DEPTH },
  { "t", "thick", STOK_THICK },
  { "s", "side", STOK_SIDE },
  { "rel", "pos", STOK_REL },
  { "tx", "texture", STOK_TEXTURE },
  { "xp", "tx_back", STOK_TEX_XP },
  { "xn", "tx_front", STOK_TEX_XN },
  { "yp", "tx_left", STOK_TEX_YP },
  { "yn", "tx_right", STOK_TEX_YN },
  { "zp", "tx_top", STOK_TEX_ZP },
  { "zn", "tx_bottom", STOK_TEX_ZN },
  { "txr", "texture_r", STOK_TEXTURE_R },
  { "xpr", "tex_xpr", STOK_TEX_XP_R },
  { "xnr", "tex_xnr", STOK_TEX_XN_R },
  { "ypr", "tex_ypr", STOK_TEX_YP_R },
  { "ynr", "tex_ynr", STOK_TEX_YN_R },
  { "zpr", "tex_zpr", STOK_TEX_ZP_R },
  { "znr", "tex_znr", STOK_TEX_ZN_R },
  { "txrs", "texture_rs", STOK_TEXTURE_RS },
  { "xprs", "tex_xprs", STOK_TEX_XP_RS },
  { "xnrs", "tex_xnrs", STOK_TEX_XN_RS },
  { "yprs", "tex_yprs", STOK_TEX_YP_RS },
  { "ynrs", "tex_ynrs", STOK_TEX_YN_RS },
  { "zprs", "tex_zprs", STOK_TEX_ZP_RS },
  { "znrs", "tex_znrs", STOK_TEX_ZN_RS },
  { "txrt", "texture_rt", STOK_TEXTURE_RT },
  { "xprt", "tex_xprt", STOK_TEX_XP_RT },
  { "xnrt", "tex_xnrt", STOK_TEX_XN_RT },
  { "yprt", "tex_yprt", STOK_TEX_YP_RT },
  { "ynrt", "tex_ynrt", STOK_TEX_YN_RT },
  { "zprt", "tex_zprt", STOK_TEX_ZP_RT },
  { "znrt", "tex_znrt", STOK_TEX_ZN_RT },
  { "dif", "diffuse", STOK_DIFFUSE },
  { "amb", "ambient", STOK_AMBIENT },
  { "spe", "specular", STOK_SPECULAR },
  { "emi", "emission", STOK_EMISSION },
  { "shi", "shininess", STOK_SHININESS },
  { "a", "alpha", STOK_ALPHA },
  { "fog", "fog", STOK_FOG },
  { "st", "style", STOK_STYLE },
  { "fa", "face", STOK_FACE },
  { "sl", "slices", STOK_SLICES },
  { "sk", "stacks", STOK_STACKS },
  { "cy", "cylinders", STOK_CYLINDERS },
  { "ci", "circles", STOK_CIRCLES },
  { "sp", "spokes", STOK_SPOKES },
  { "bl", "blink", STOK_BLINK },
  { "nf", "frames", STOK_FRAMES },
  { "bf", "beginframe", STOK_BEGINFRAME },
  { "ef", "endframe", STOK_ENDFRAME },
  { "ml", "model", STOK_MODEL },
  { "sc", "scale", STOK_SCALE },
  { "sx", "scalex", STOK_SCALEX },
  { "sy", "scaley", STOK_SCALEY },
  { "sz", "scalez", STOK_SCALEZ },
  { "spotd", "spot_direction", STOK_SPOT_DIRECTION },
  { "spotc", "spot_cutoff", STOK_SPOT_CUTOFF },
  { "ca", "constant_attenuation", STOK_CONSTANT_ATTENUAT },
  { "la", "linear_attenuation", STOK_LINEAR_ATTENUAT },
  { "qa", "quadratic_attenuation", STOK_QUADRATIC_ATTENUAT },
  { NULL, NULL, 0 }
};


// defaults
const uint8_t Solid::DEF_ALPHA = 1.;	// opaque
const uint8_t Solid::DEF_FOG = 0;	// no fog
const uint8_t Solid::DEF_SCALE = 1.;	// no scale
const uint8_t Solid::DEF_SHININESS = 20;
const uint8_t Solid::DEF_SPHERE_SLICES = 16;
const uint8_t Solid::DEF_SPHERE_STACKS = 16;
const uint8_t Solid::DEF_CONE_SLICES = 16;
const uint8_t Solid::DEF_CONE_STACKS = 8;
const uint8_t Solid::DEF_TORUS_CYLINDERS = 16;
const uint8_t Solid::DEF_TORUS_CIRCLES = 16;
const uint8_t Solid::DEF_DISK_SLICES = 16;
const uint8_t Solid::DEF_WHEEL_SPOKES = 12;
const uint8_t Solid::DEF_DISK_LOOPS = 8;
const uint8_t Solid::FRAME_MAX = 255;


/** Constructor: new solid. */
Solid::Solid()
{
  new_solid++;
  shape = STOK_BOX;	// shape by default: box
  object = NULL;	// object associated with this solid set by addSolid in object.cpp
  displist = NULL;	// solid displaylists

  visible = true;	// visible by default
  opaque = true;	// opaque by default
  fictif = false;	// real solid by default
  framed = false;	// mono framed by default
  flashy = false;	// no flashy by default
  flary = false;	// no flary by default
  reflexive = false;	// no reflexive by default
  blinking = false;	// no blinking by default
  blink = false;	// no blinking by default
  rendered = false;	// flag if already rendered

  nbframes = 1;		// 1 frame by default
  frame = 0;		// frame to render
  iframe = 0;		// frame index in displaylist
  userdist = 0;		// distance to localuser
  surfsize = 0;		// surface of solid
  ray_dlist = 0;	// ray displaylist
  clearV3(bbcent);	// clear bb center
  clearV3(bbsize);	// clear bb size

  for (int i=0; i<5; i++) rel[i] = 0;		// clear rel pos
  for (int i=0; i<3; i++) flashcol[i] = 1;	// clear to white

  ::g.render.relsolidList.push_back(this);	// add solid to relsolidList
  nbsolids = ::g.render.relsolidList.size();	// number of solids
}

/** Destructor: deletes solid from displaylist. */
Solid::~Solid()
{
  ::g.render.solidList.remove(this);
  ::g.render.relsolidList.clear();
  nbsolids = 0;

  if (displist) delete[] displist;
  del_solid++;
}

char * Solid::skipEqual(char *p)
{
  if (p && (p = strchr(p, '='))) p++;
  return p;
}

char * Solid::getTok(char *l, uint16_t *stok)
{
  char *t = l;
  const struct sStokens *ptab;

  *stok = STOK_ERR;
  l = skipEqual(l);
  if (l) {
    *(l-1) = '\0';	// end of token '=',  replaced by null terminated
    for (ptab = stokens; ptab->tokstr ; ptab++) {
      if ( (! strcmp(ptab->tokstr, t)) || (! strcmp(ptab->tokalias, t)) ) {
        *stok = ptab->tokid;
        return l;
      }
    }
    error("getTok: unknown token \"%s\"", t);
    object->parse()->printNumline();
    return l;
  }
  else {
    object->parse()->printNumline();
    return t;
  }
}

/** Parses <frame , return next token after frame. */
char * Solid::parseFrame(char *l)
{
  if (! strcmp(l, "frame")) {
    framed = true;
    char *p = object->parse()->nextToken();
    return p;
  }
  while (l && framed) {
    if (! stringcmp(l, "frame"))
      return object->parse()->nextToken();
    l = object->parse()->nextToken();
  }
  return l;
}

/** Sets shape from shape="...", return next token. */
char * Solid::parseShape(char *l, uint8_t *shape)
{
  const struct sStokens *ptab;
  char s[16];

  if (! stringcmp(l, "shape=")) {
    l = object->parse()->parseString(l, s, "shape");
    for (ptab = stokens; ptab->tokstr ; ptab++) {
      if ((! strcmp(ptab->tokstr, s)) || (! strcmp(ptab->tokalias, s))) {
        *shape = ptab->tokid;
        return l;
      }
    }
    error("unknown shape \"%s\"", s);
    object->parse()->printNumline();
    *shape = STOK_ERR;
  }
  return l;
}

/**
 * solid parser.
 * returns next token if it exists
 * called by parseSolid(l) in parse.cpp.
 */
char * Solid::parser(char *l)
{
  if (!l) {
    error("no solid content");
    object->parse()->printNumline();
    return NULL;
  }
  char *ll = strdup(l);	// copy origin line for debug

  if (*l == '<') l++;	// skip open-tag
  if (! stringcmp(l, "frames=")) {
    l = object->parse()->parseUInt8(l, &nbframes, "frames");
  }
  displist = new GLint[nbframes];

  ::g.render.solidList.push_back(this);	// add to solidList

  idM4(&matpos);	// init position to 0

  // axis aligned bounding boxes (AABB) are here
  V3 bbmax = setV3(0, 0, 0);
  V3 bbmin = setV3(0, 0, 0);

  // for each frame
  for (iframe = 0; iframe < nbframes; ) {
    int r = 0;

    l = parseFrame(l);
    l = parseShape(l, &shape);

    switch (shape) {
      case STOK_NONE:
      case STOK_BBOX:
      case STOK_BOX:
      case STOK_SPHERE:
      case STOK_CONE:
      case STOK_TORUS:
      case STOK_RECT:
      case STOK_DISK:
      case STOK_LINE:
      case STOK_POINT:
      case STOK_PENTAGON:
      case STOK_PYRAMID:
      case STOK_OCTAGON:
      case STOK_TRIANGLE:
      case STOK_BSPHERE:
      case STOK_CIRCLE:
      case STOK_ELLIPSE:
      case STOK_SPHERE_TORUS:
      case STOK_CROSS:
      case STOK_SPHERE_DISK:
      case STOK_CONE_DISK:
      case STOK_WHEEL:
      case STOK_HELIX:
      case STOK_TEAPOT:
      case STOK_WALLS:
      case STOK_MAN:
      case STOK_GUY:
      case STOK_HUMANOID:
      case STOK_CAR:
      case STOK_WINGS:
        r = solidParser(l, bbmax, bbmin); break;
      case STOK_STATUE:
      case STOK_MODEL:
        r = statueParser(l, bbmax, bbmin); break;
      default:
        delete[] displist;
        displist = NULL;
        return NULL;
    }

    if (r == -1) {
      free(ll);
      delete this;
      return NULL;
    }
    iframe += r;
  }

  // computes relative AABB of this solid: bbcent and bbsize
  for (int i=0; i<3; i++) {
    bbcent.v[i] = (bbmax.v[i] + bbmin.v[i]) / 2;
    bbsize.v[i] = (bbmax.v[i] - bbmin.v[i]) / 2;
  }

  // computes max surface of this solid
  surfsize = MAX( bbsize.v[0]*bbsize.v[1], bbsize.v[0]*bbsize.v[2] );
  surfsize = MAX( surfsize, bbsize.v[1]*bbsize.v[2] );	// surface max

  /* next token */
  l = object->parse()->nextToken();
  if (l && !strcmp(l, "/solid")) {	// md2
    l = object->parse()->nextToken();	// skip </solid>
  }
  return l;	// next token
}

/**
 * Single solid parser.
 * returns bbmax bbmin
 */
int Solid::solidParser(char *l, V3 &bbmax, V3 &bbmin)
{
  if (!l) {
    error("no solid description");
    object->parse()->printNumline();
    return -1;
  }

  // default dimensions
  GLfloat radius = 0, radius2 = 0, radius3 = 0;
  GLfloat length = 0, width = 0, height = 0, thick = 0, side = 0;

  // default materials
  for (int i=0; i<4; i++) {
    mat_diffuse[i] = mat_ambient[i] = mat_specular[i] = 1; // white
    mat_emission[i] = 0;
  }
  mat_emission[3] = 1;
  mat_shininess[0] = DEF_SHININESS;
  alpha = DEF_ALPHA;	// opaque by fefault

  // default lights params
  GLfloat light_spot_direction[] = {1,1,1,1};
  GLfloat light_spot_cutoff[] = {180};
  GLfloat light_constant_attenuation[] = {1};
  GLfloat light_linear_attenuation[] = {0};
  GLfloat light_quadratic_attenuation[] = {0};
  fog[0] = 0;
  fog[1] = fog[2] = fog[3] = 1; // white

  // default draws params
  scale = scalex = scaley = scalez = DEF_SCALE;	// no scale
  uint8_t style = Draw::STYLE_FILL;	// default style
  uint8_t slices = DEF_SPHERE_SLICES;
  uint8_t stacks = DEF_SPHERE_STACKS;
  uint8_t cylinders = DEF_TORUS_CYLINDERS;
  uint8_t circles = DEF_TORUS_CIRCLES;
  uint8_t spokes = DEF_WHEEL_SPOKES;
  uint8_t face = 0;	// default face

  // default textures
  texid = -1;				// for quadrics (sphere, cylinder, disc, torus, ...)
  int box_tex[6] = {-1,-1,-1,-1,-1,-1};	// for parallepipedic shapes
  GLfloat box_texrep[6][2] = { {1,1}, {1,1}, {1,1}, {1,1}, {1,1}, {1,1} };
  GLfloat tex_r_s = 1, tex_r_t = 1;

  Teapot *teapot = NULL;
  Car *car = NULL;

  V3 dim = setV3(.1, .1, .1);		// default dimension : box 10x10x10 cm

  ::g.render.first_bb = true;

  switch (shape) {
    case STOK_SPHERE:
      slices    = DEF_SPHERE_SLICES;
      stacks    = DEF_SPHERE_STACKS;
      break;
    case STOK_CONE:
      slices    = DEF_CONE_SLICES;
      stacks    = DEF_CONE_STACKS;
      break;
    case STOK_TORUS:
      cylinders = DEF_TORUS_CYLINDERS;
      circles   = DEF_TORUS_CIRCLES;
      break;
    case STOK_DISK:
    case STOK_SPHERE_DISK:
    case STOK_CONE_DISK:
      slices    = DEF_DISK_SLICES;
      stacks    = DEF_DISK_LOOPS;
      break;
    case STOK_SPHERE_TORUS:
      slices    = DEF_SPHERE_SLICES;
      stacks    = DEF_SPHERE_STACKS;
      cylinders = DEF_TORUS_CYLINDERS;
      circles   = DEF_TORUS_CIRCLES;
    case STOK_WHEEL:
      spokes    = DEF_WHEEL_SPOKES;
      break;
  }

  // parse shape="..."
  while (l) {
    uint16_t stok = 0; uint8_t flgblk = 0;

    if ((*l == '\0') || (*l == '/')) {
      *l = '\0';
      break;
    } // end of solid

    l = getTok(l, &stok);
    switch (stok) {
      case STOK_SOLID:	//not used
        error("token solid");
        break;
      case STOK_URL:
        break;
      case STOK_SIZE:
        l = object->parse()->parseVector3fv(l, &dim);
        break;
      case STOK_DIFFUSE:
        l = object->parse()->parseVector3f(l, &mat_diffuse[0]);
        for (int i=0; i<3; i++) {
          mat_ambient[i] = mat_diffuse[i];
        }
        break;
      case STOK_AMBIENT:
        l = object->parse()->parseVector3f(l, &mat_ambient[0]);
        break;
      case STOK_SPECULAR:
        l = object->parse()->parseVector3f(l, &mat_specular[0]);
        break;
      case STOK_EMISSION:
        l = object->parse()->parseVector3f(l, &mat_emission[0]);
        for (int i=0; i<3; i++) {
          mat_ambient[i] = mat_emission[i];
        }
        break;
      case STOK_SHININESS:
        l = object->parse()->parseInt(l, &mat_shininess[0]);
        break;
      case STOK_ALPHA:
        l = object->parse()->parseFloat(l, &alpha);
        mat_diffuse[3] = mat_ambient[3] = alpha;
        if (alpha < 1) {
          opaque = false;
        }
        break;
      case STOK_SCALE:
        l = object->parse()->parseFloat(l, &scale);
        break;
      case STOK_SCALEX:
        l = object->parse()->parseFloat(l, &scalex);
        break;
      case STOK_SCALEY:
        l = object->parse()->parseFloat(l, &scaley);
        break;
      case STOK_SCALEZ:
        l = object->parse()->parseFloat(l, &scalez);
        break;
      case STOK_RADIUS3:
        l = object->parse()->parseFloat(l, &radius3);
        break;
      case STOK_RADIUS2:
        l = object->parse()->parseFloat(l, &radius2);
        break;
      case STOK_RADIUS:
        l = object->parse()->parseFloat(l, &radius);
        break;
      case STOK_HEIGHT:
        l = object->parse()->parseFloat(l, &height);
        break;
      case STOK_LENGTH:
        l = object->parse()->parseFloat(l, &length);
        break;
      case STOK_WIDTH:
        l = object->parse()->parseFloat(l, &width);
        break;
      case STOK_THICK:
        l = object->parse()->parseFloat(l, &thick);
        break;
      case STOK_SIDE:
        l = object->parse()->parseFloat(l, &side);
        break;
      case STOK_PTSIZE:
        l = object->parse()->parseFloat(l, &radius);
        break;
      case STOK_FOG:
        l = object->parse()->parseFloat(l, &fog[0]);
        for (int i=0; i<3; i++) {
          fog[i+1] = mat_diffuse[i];
        }
        break;
      case STOK_STYLE:
        l = object->parse()->parseUInt8(l, &style);
        break;
      case STOK_FACE:
        l = object->parse()->parseUInt8(l, &face);
        break;
      case STOK_SLICES:
        l = object->parse()->parseUInt8(l, &slices);
        break;
      case STOK_STACKS:
        l = object->parse()->parseUInt8(l, &stacks);
        break;
      case STOK_CYLINDERS:
        l = object->parse()->parseUInt8(l, &cylinders);
        break;
      case STOK_CIRCLES:
        l = object->parse()->parseUInt8(l, &circles);
        break;
      case STOK_SPOKES:
        l = object->parse()->parseUInt8(l, &spokes);
        break;
      case STOK_SPOT_CUTOFF:
        l = object->parse()->parseFloat(l, &light_spot_cutoff[0]);
        break;
      case STOK_SPOT_DIRECTION:
        l = object->parse()->parseVector3f(l, &light_spot_direction[0]);
        break;
      case STOK_CONSTANT_ATTENUAT:
        l = object->parse()->parseFloat(l, &light_constant_attenuation[0]);
        break;
      case STOK_LINEAR_ATTENUAT:
        l = object->parse()->parseFloat(l, &light_linear_attenuation[0]);
        break;
      case STOK_QUADRATIC_ATTENUAT:
        l = object->parse()->parseFloat(l, &light_quadratic_attenuation[0]);
        break;
      case STOK_BLINK:
        l = object->parse()->parseUInt8(l, &flgblk);
        if (flgblk) {
          blink = true;
          setBlinking(true);
        }
        break;
      case STOK_REL:
        l = object->parse()->parseVector5f(l, rel);
        nbsolids = ::g.render.relsolidList.size();
        break;
      case STOK_TEXTURE:
        { char *urltex = new char[URL_LEN];
          l = object->parse()->parseString(l, urltex);
          if (*urltex) {
            texture = new Texture(urltex);
            for (int i=0; i<6 ; i++) {
              box_tex[i] = texture->tex_id;
            }
            texid = texture->tex_id;
            texture->object = object;
          }
          delete[] urltex;
        }
        break;
      case STOK_TEX_XP:
      case STOK_TEX_XN:
      case STOK_TEX_YP:
      case STOK_TEX_YN:
      case STOK_TEX_ZP:
      case STOK_TEX_ZN:
        { char *urltex = new char[URL_LEN];
          l = object->parse()->parseString(l, urltex);
          if (*urltex) {
            texture = new Texture(urltex);
            box_tex[stok - STOK_TEX_XP] = texture->tex_id;
            texid = texture->tex_id;
            texture->object = object;
          }
          delete[] urltex;
        }
        break;
      case STOK_TEXTURE_R:
        l = object->parse()->parseFloat(l, &tex_r_s);
        tex_r_t = tex_r_s;
        break;
      case STOK_TEXTURE_RS:
        l = object->parse()->parseFloat(l, &tex_r_s);
        break;
      case STOK_TEXTURE_RT:
        l = object->parse()->parseFloat(l, &tex_r_t);
        break;
      case STOK_TEX_XP_R:
      case STOK_TEX_XN_R:
      case STOK_TEX_YP_R:
      case STOK_TEX_YN_R:
      case STOK_TEX_ZP_R:
      case STOK_TEX_ZN_R:
        l = object->parse()->parseFloat(l, &box_texrep[stok-STOK_TEX_XP_R][0]);
        box_texrep[stok-STOK_TEX_XP_R][1] = box_texrep[stok-STOK_TEX_XP_R][0];
        break;
      case STOK_TEX_XP_RS:
      case STOK_TEX_XN_RS:
      case STOK_TEX_YP_RS:
      case STOK_TEX_YN_RS:
      case STOK_TEX_ZP_RS:
      case STOK_TEX_ZN_RS:
        l = object->parse()->parseFloat(l, &box_texrep[stok-STOK_TEX_XP_RS][0]);
        break;
      case STOK_TEX_XP_RT:
      case STOK_TEX_XN_RT:
      case STOK_TEX_YP_RT:
      case STOK_TEX_YN_RT:
      case STOK_TEX_ZP_RT:
      case STOK_TEX_ZN_RT:
        l = object->parse()->parseFloat(l, &box_texrep[stok-STOK_TEX_XP_RT][1]);
        break;
      default:
        object->parse()->printNumline();
        return -1;
    }
  }

  /*
   * draws solid in displaylist
   */
  int dl = glGenLists(1);	// display list generation
  glNewList(dl, GL_COMPILE);

  glCullFace(GL_BACK);		// don't draw back face
  glShadeModel(GL_SMOOTH);

  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
  glMaterialiv(GL_FRONT, GL_SHININESS, mat_shininess);

  switch (shape) {

    case STOK_NONE:
      fictif = true;
      break;

    case STOK_BOX:
      preDraw(texid, alpha, fog);
      Draw::box(dim.v[0], dim.v[1], dim.v[2], box_tex, box_texrep, style);
      //dax1 if (::g.pref.bbox) Draw::bbox(dim.v[0], dim.v[1], dim.v[2]);
      postDraw(texid, alpha, fog);
      break;

    case STOK_BBOX:	// invisible bounding box
      setBB(dim.v[0], dim.v[1], dim.v[2]);
      //dax1 if (::g.pref.bbox) Draw::bbox(dim.v[0], dim.v[1], dim.v[2]);
      break;

    case STOK_SPHERE:
      preDraw(texid, alpha, fog, true);
      Draw::sphere(radius, slices, stacks, style);
      setBB(radius*M_SQRT2, radius*M_SQRT2, radius*M_SQRT2);
      //dax1 if (::g.pref.bbox) Draw::bbox(radius*M_SQRT2, radius*M_SQRT2, radius*M_SQRT2);
      postDraw(texid, alpha, fog);
      break;

    case STOK_BSPHERE:	// invisible bounding sphere
      if (! radius) {
        radius = (dim.v[0] + dim.v[1] + dim.v[2]) / 3;
      }
      dim.v[0] = dim.v[1] = dim.v[2] = 2*radius;
      setBB(2*radius, 2*radius, 2*radius);
      //dax1 if (::g.pref.bbox) Draw::bbox(2*radius, 2*radius, 2*radius);
      break;

    case STOK_MAN:
      {
       Man *man = new Man();
       man->draw();
      }
      setBB(dim.v[0], dim.v[1], dim.v[2]/2);
      break;

    case STOK_GUY:
    case STOK_HUMANOID:
      //echo("%.1f %.1f %.1f", dim.v[0], dim.v[1], dim.v[2]);
      setBB(dim.v[0], dim.v[1], dim.v[2]/2);
      break;

    case STOK_WINGS:
      {
       Wings *wings = new Wings();
       wings->draw();
      }
      setBB(dim.v[0], dim.v[1], dim.v[2]);
      break;

    case STOK_CONE:
      if (radius == radius2) {
        stacks = 1;	// cylinder
      }
      preDraw(texid, alpha, fog, true);
      if (thick) {	// double surface
        Draw::cylinder(radius - thick/2, radius2 - thick/2, height, slices, stacks, style);
        glEnable(GL_CULL_FACE);
        Draw::cylinder(radius + thick/2, radius2 + thick/2, height, slices, stacks, style);
      }
      else {		// single surface
        Draw::cylinder(radius, radius2, height, slices, stacks, style);
      }
      setBB(radius, radius, height);
      //dax1 if (::g.pref.bbox) Draw::bbox(radius, radius, height);
      postDraw(texid, alpha, fog);
      break;

    case STOK_TORUS:
      preDraw(texid, alpha, fog);
      Draw::torus(radius2, cylinders, radius, circles, style);
      //dax1 if (::g.pref.bbox) Draw::bbox(radius*M_SQRT2, radius*M_SQRT2, radius2);
      setBB(radius*M_SQRT2, radius*M_SQRT2, radius2);
      postDraw(texid, alpha, fog);
      break;

    case STOK_PENTAGON:
      preDraw(texid, alpha, fog, true);
      Draw::pentagon(side, height, style);
      setBB(side, side, height);
      postDraw(texid, alpha, fog);
      break;

    case STOK_PYRAMID:
      preDraw(texid, alpha, fog, true);
      Draw::pentagon(side, side, style);
      setBB(side, side, side);
      postDraw(texid, alpha, fog);
      break;

    case STOK_OCTAGON:
      preDraw(texid, alpha, fog, true);
      Draw::octagon(side, height, style);
      setBB(side, side, height);
      postDraw(texid, alpha, fog);
      break;

    case STOK_SPHERE_TORUS:
      preDraw(texid, alpha, fog);
      Draw::sphere(radius, slices, stacks, style);
      Draw::torus(0.05, cylinders, radius2, circles, style);
      setBB(radius2*M_SQRT1_2/2, radius2*M_SQRT1_2/2, radius*M_SQRT1_2/2);
      //dax1 if (::g.pref.bbox) Draw::bbox(radius2*M_SQRT2, radius2*M_SQRT2, radius*M_SQRT2/2);
      postDraw(texid, alpha, fog);
      break;

    case STOK_DISK:
      preDraw(texid, alpha, fog, true);
      Draw::disk(radius, radius2, slices, stacks, style);
      postDraw(texid, alpha, fog);
      break;

    case STOK_SPHERE_DISK:
      preDraw(texid, alpha, fog);
      Draw::sphere(radius, slices, stacks, style);
      Draw::disk(radius, radius2, slices, stacks, style);
      setBB(radius2*M_SQRT1_2, radius2*M_SQRT1_2, radius*M_SQRT1_2);
      //dax1 if (::g.pref.bbox) Draw::bbox(radius*M_SQRT2, radius*M_SQRT2, radius*M_SQRT2/2);
      postDraw(texid, alpha, fog);
      break;

    case STOK_CONE_DISK:
      if (radius == radius3) {
        stacks = 1; // cylinder
      }
      preDraw(texid, alpha, fog);
      Draw::cylinder(radius, radius3, height, slices, stacks, style);
      Draw::disk(radius, radius2, slices, stacks, style);
      postDraw(texid, alpha, fog);
      break;

    case STOK_CROSS:
      preDraw(texid, alpha, fog);
      Draw::box(dim.v[0], dim.v[1], dim.v[2], box_tex, box_texrep, style);
      Draw::box(dim.v[2], dim.v[1], dim.v[0], box_tex, box_texrep, style);
      postDraw(texid, alpha, fog);
      break;

    case STOK_LINE:
      doTransform(true);
      Draw::line(length, thick);
      doTransform(false);
      break;

    case STOK_RECT:
      doTransform(true);
      Draw::rect(dim.v[0], dim.v[1], style, tex_r_s, tex_r_t);
      doTransform(false);
      break;

    case STOK_CIRCLE:
      doTransform(true);
      Draw::ellipse(radius, radius, style);
      doTransform(false);
      break;

    case STOK_TRIANGLE:
      break;

    case STOK_ELLIPSE:
      doTransform(true);
      Draw::ellipse(radius, radius2, style);
      doTransform(false);
      break;

    case STOK_HELIX:
      doTransform(true);
      Draw::helix(radius, length, height, slices, thick, mat_diffuse);
      setBB(radius, radius, length/2);
      doTransform(false);
      break;

    case STOK_POINT:
      glPointSize((radius) ? radius : 1);
      Draw::point(dim.v[0], dim.v[1], dim.v[2]);
      glPointSize(1);
      break;

    case STOK_CAR:
      preDraw(texid, alpha, fog, true);
      car = new Car();
      car->draw(dim.v[0], dim.v[1], dim.v[2], box_tex, box_texrep, slices, style);
      setBB(dim.v[0], dim.v[1], dim.v[2]/2);
      //dax1 if (::g.pref.bbox) Draw::bbox(dim.v[0], dim.v[1], dim.v[2]/2);
      postDraw(texid, alpha, fog);
      break;

    case STOK_TEAPOT:
      preDraw(texid, alpha, fog, true);
      teapot = new Teapot();
      teapot->draw(dim.v[0], dim.v[1], dim.v[2], box_tex, box_texrep, slices, style);
      setBB(dim.v[0], dim.v[1], dim.v[2]);
      //dax1 if (::g.pref.bbox) Draw::bbox(dim.v[0], dim.v[1], dim.v[2]);
      postDraw(texid, alpha, fog);
      break;

    case STOK_WHEEL:
      Wheel *wheel = new Wheel(spokes, radius, mat_diffuse);
      wheel->render();
      break;
  }
  glEndList();

  // sets displist number for this frame
  displist[iframe] = dl;

  /*
   * gets bounding boxes bbmax and bbmin
   */
  switch (shape) {
    // without bounding boxes
    case STOK_POINT:
    case STOK_LINE:
    case STOK_TRIANGLE:
    case STOK_RECT:
    case STOK_CIRCLE:
    case STOK_WHEEL:
    case STOK_ELLIPSE:
      break;
    default: // with bounding boxes
      getMaxMinBB(bbmax, bbmin, iframe != 0);
  }

  updateDist();

  return 1;	// only one frame
}

/** compute distance to localuser */
void Solid::updateDist()
{
  if (localuser) {
    float dx = ABSF(localuser->pos.x - object->pos.x);
    float dy = ABSF(localuser->pos.y - object->pos.y);
    userdist = sqrt(dx*dx + dy*dy);
  }
}

/** Parses statue and returns f (number of frames). */
int Solid::statueParser(char *l, V3 &bbmax, V3 &bbmin)
{
  int texid = -1;
  GLfloat scale = DEF_SCALE;
  uint8_t firstframe = 1;
  uint8_t lastframe = nbframes;
  uint8_t tabframes[FRAME_MAX] = { 0 };
  char *url = NULL;
  fog[0] = 0;

  alpha = DEF_ALPHA;

  ::g.render.first_bb = true;

  while (l) {
    uint16_t stok;

    if ( (*l == '\0') || (*l == '/') ) break; // end of statue

    l = getTok(l, &stok);
    if (stok == 0) break;

    switch (stok) {
      case STOK_SOLID: break;
      case STOK_MODEL:
      case STOK_URL:
        url = new char[URL_LEN];
        l = object->parse()->parseString(l, url);
        break;
      case STOK_SCALE:
        l = object->parse()->parseFloat(l, &scale);
        break;
      case STOK_BEGINFRAME:
        l = object->parse()->parseUInt8(l, &firstframe);
        break;
      case STOK_ENDFRAME:
        l = object->parse()->parseUInt8(l, &lastframe);
        break;
      case STOK_DIFFUSE:
        l = object->parse()->parseVector3f(l, &mat_diffuse[0]);
        for (int i=0; i<3; i++) {
          mat_ambient[i] = mat_diffuse[i];
        }
        break;
      case STOK_AMBIENT:
        l = object->parse()->parseVector3f(l, &mat_ambient[0]);
        break;
      case STOK_SPECULAR:
        l = object->parse()->parseVector3f(l, &mat_specular[0]);
        break;
      case STOK_ALPHA:
        l = object->parse()->parseFloat(l, &alpha);
        mat_diffuse[3] = mat_ambient[3] = alpha;
        if (alpha < 1) {
          opaque = false;
        }
        break;
      case STOK_TEXTURE:
        { char *tx = new char[URL_LEN];
          l = object->parse()->parseString(l, tx);
          if (*tx) {
            texture = new Texture(tx);
            texid = texture->tex_id;
            texture->object = object;
          }
          delete[] tx;
        }
        break;
      default:
        error("statueParser: bad stok=%d", stok);
        return -1;
    }
  }
  firstframe = (firstframe == 0) ? 1 : firstframe;
  if (lastframe < firstframe) lastframe = firstframe;

  int nf;
  for (nf=0; firstframe <= lastframe; nf++) {
    tabframes[nf] = firstframe++;
  }
  tabframes[nf] = 0;

  if (url) {	// model url exists
    int model = Format::getModelByUrl(url);
    switch (model) {

      case MODEL_MD2: {
        Md2 *md2 = new Md2(url);

        for (nf=0; tabframes[nf] && nf < FRAME_MAX; nf++) {
          md2->setScale(scale);
          if ((displist[nf] = md2->displaylist(tabframes[nf], texid)) < 0) {
            nf = -1;
            break;
          }
          getMaxMinBB(bbmax, bbmin, framed); // get bounding box
        }
        if (md2) delete md2;
        md2 = NULL;
        break;
        }

      case MODEL_OBJ: {
        Obj *obj = new Obj(url);

        obj->setScale(scale);
        obj->setColor(GL_DIFFUSE, mat_diffuse);
        obj->setColor(GL_AMBIENT, mat_diffuse);
        obj->setColor(GL_SPECULAR, mat_specular);
        displist[0] = obj->displaylist();
        nf = 1;
        if (obj) delete obj;
        obj = NULL;
        break;
        }
    }
    delete[] url;
  }
  return nf;	// number nf frames
}

void Solid::doTransform(int flag)
{
  switch (flag) {
  case true:  // pre
    glPushMatrix();
    glRotatef(RAD2DEG(rel[3]), 0, 0, 1);	// az
    glRotatef(RAD2DEG(rel[4]), 1, 0, 0);	// ax
    glTranslatef(rel[0], rel[1], rel[2]);	// x y z
    glScalef(scalex, scaley, scalez);
    break;
  case false: // post
    glPopMatrix();
    break;
  }
}

void Solid::doBlend(int flag, GLfloat _alpha)
{
  if (_alpha < 1) {
    opaque = false;
    switch (flag) {
    case true:  // pre
      glEnable(GL_BLEND);
      glDepthMask(GL_FALSE);	// turn off the depth buffer for writing
      break;
    case false: // post
      glDepthMask(GL_TRUE);	// turn on the depth buffer for writing
      glDisable(GL_BLEND);
      break;
    }
  }
}

void Solid::doTexture(int flag, int _texid)
{
  if (_texid >= 0) {
    switch (flag) {
    case true:  // pre
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, _texid);
      break;
    case false: // post
      glDisable(GL_TEXTURE_2D);
      break;
    }
  }
}

void Solid::preDraw(int texid, GLfloat alpha, GLfloat *fog, bool cull)
{
  // order is important !!!
  doBlend(true, alpha);
  doTexture(true, texid);
  doTransform(true);
  if (cull) glDisable(GL_CULL_FACE);
}

void Solid::postDraw(int texid, GLfloat alpha, GLfloat *fog)
{
  glEnable(GL_CULL_FACE);
  doTransform(false);
  doTexture(false, texid);
  doBlend(false, alpha);
}

/** Gets max min BB. */
void Solid::getMaxMinBB(V3 &max, V3 &min, bool is_framed)
{
  ::g.render.getBB(max, min, is_framed);
}

/** Sets BB sizes. */
void Solid::setBB(GLfloat w, GLfloat d, GLfloat h)
{
  ::g.render.setBB(w, d, h);
}

/** returns relative center and size of BB. */
void Solid::getRelBB(V3 &center, V3 &size) const
{
  center = bbcent;
  size = bbsize;
}

/** returns size of BB. */
void Solid::getDimBB(V3 &dim) const
{
  dim = bbsize;
}

/** Returns absolute center and size of AABB. */
void Solid::getAbsBB(V3 &center, V3 &size)
{
  V3 vtmp[2], vrel, vabs, vmin, vmax;

  for (int i=0; i<3; i++) {
    vtmp[0].v[i] = bbcent.v[i] - bbsize.v[i]; // min
    vtmp[1].v[i] = bbcent.v[i] + bbsize.v[i]; // max
  }
  for (int n=0; n < 8; n++) {	// 8 points of AABB parallelepidic
    vrel.v[0] = vtmp[n     % 2].v[0];
    vrel.v[1] = vtmp[(n/2) % 2].v[1];
    vrel.v[2] = vtmp[(n/4) % 2].v[2];

    // add object position
    mulM4V3(&vabs, &matpos, &vrel);	// vabs = posmat * vrel
    if (n == 0) {
      vmin = vmax = vabs;
    }
    else {
      for (int i=0; i<3; i++) {
	vmin.v[i] = MIN(vabs.v[i], vmin.v[i]);
	vmax.v[i] = MAX(vabs.v[i], vmax.v[i]);
      }
    }
  }
  for (int i=0; i<3; i++) {
    center.v[i] = (vmax.v[i] + vmin.v[i]) / 2;
    size.v[i]   = (vmax.v[i] - vmin.v[i]) / 2;
  }
}

void Solid::updateBB(GLfloat az)
{
  ::g.render.updBB(az);
}

/** returns materials. */
void Solid::getMaterials(GLfloat *dif, GLfloat *amb, GLfloat *spe, GLfloat *emi, GLint *shi, GLfloat *alp)
{
  for (int i=0; i<4; i++) {
    dif[i] = mat_diffuse[i];
    amb[i] = mat_ambient[i];
    spe[i] = mat_specular[i];
    emi[i] = mat_emission[i];
  }
  shi[0] = mat_shininess[0];
  *alp = alpha;
}

void Solid::setPosition(const M4 &mpos)
{
  matpos = mpos;
}

void Solid::getPosition(M4& mpos)
{
  mpos = matpos;
}

void Solid::setVisible(bool _isvis)
{
  visible = _isvis;
}

void Solid::setTransparent(float _alpha)
{
  alpha = _alpha;
  if (alpha < 1)
    opaque = false;
  else
    opaque = true;
}

void Solid::setFlary(bool flag)
{
  flary = flag;
}

void Solid::setReflexive(bool flag)
{
  reflexive = flag;
}

/** Draws a ray between user'eyes and the object's impact
 * window: +--> x	viewport: y
 *         |			  ^
 *         v			  |
 *         y			  +--> x
 */
void Solid::setRay(GLint wx, GLint wy)
{
  if (! localuser) return;

  // user's eye
  GLfloat ex = localuser->pos.x + User::NEAR; //eye + near
  GLfloat ey = localuser->pos.y;
  GLfloat ez = localuser->pos.z + localuser->height/2 - 0.10; //eye's height
  // center of object
  GLfloat ox = object->pos.x;
  GLfloat oy = object->pos.y;
  GLfloat oz = object->pos.z;
  // ray color
  const GLfloat *color = Color::white;  // white

  Draw::ray(&ray_dlist, ex, ey, ez, ox, oy, oz, color, 0x3333);  // alternative

  localuser->ray = setV3(ox, oy, oz);
  localuser->netop->declareDelta(User::PROPRAY);  // publish ray property to network
}

void Solid::resetRay()
{
  if (ray_dlist) {
    ray_dlist = 0;
    if (localuser) {
      localuser->ray = setV3(0, 0, 0);
      localuser->netop->declareDelta(User::PROPRAY);
    }
  }
}

void Solid::setFlashy(bool flag)
{
  flashy = flag;
}

void Solid::setFlashy(const GLfloat *color)
{
  flashy = true;
  for (int i=0; i<3; i++) {
    flashcol[i] = color[i];
  }
}

void Solid::resetFlashy()
{
  flashy = false;
}

bool Solid::toggleBlinking()
{
  if (blink) {
    blink = false;
    return true;
  }
  else {
    blink = true;
    return false;
  }
}

void Solid::setBlinking(bool flag)
{
  blinking = flag;
}

void Solid::setFrame(uint8_t _frame)
{
  frame = _frame % nbframes;
}

GLint Solid::getDlist() const
{
  return displist[0];
}

GLint Solid::getTexid() const
{
  return texid;
}
