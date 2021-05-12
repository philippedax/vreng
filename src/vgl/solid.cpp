//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://www.vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
/*
 * Authors: Fabrice Bellard, Philippe Dax
 */
#include "vreng.hpp"
#include "solid.hpp"
#include "draw.hpp"	// STYLE
#include "render.hpp"	// ::g.render
#include "scene.hpp"	// getScene
#include "texture.hpp"	// Texture
#include "wobject.hpp"	// WObject
#include "netobj.hpp"	// declareObjDelta
#include "color.hpp"	// Color
#include "user.hpp"	// localuser
#include "md2.hpp"	// Md2
#include "obj.hpp"	// Obj
#include "man.hpp"	// draw
#include "car.hpp"	// draw
#include "teapot.hpp"	// draw
#include "format.hpp"	// getModelByUrl
#include "parse.hpp"	// printNumline
#include "prof.hpp"	// new_solid
#include "pref.hpp"	// ::g.pref
#include "android.hpp"	// render
#include "body.hpp"	// render
#include "guy.hpp"	// Guy
#include "flare.hpp"	// render

#include <list>
using namespace std;


/* Solid tokens. */
enum {
  STOK_ERR = 0,
  /* shapes */
  STOK_SHAPE,
  STOK_NONE,
  STOK_BOX,
  STOK_MAN,
  STOK_GUY,
  STOK_ANDROID,
  STOK_CAR,
  STOK_SPHERE,
  STOK_CONE,
  STOK_TORUS,
  STOK_RECT,
  STOK_DISK,
  STOK_LINE,
  STOK_PYRAMID,
  STOK_CIRCLE,
  STOK_TRIANGLE,
  STOK_ELLIPSE,
  STOK_POINT,
  STOK_STATUE,
  STOK_BBOX,
  STOK_BSPHERE,
  STOK_CROSS,
  STOK_SPHERE_TORUS,
  STOK_SPHERE_DISK,
  STOK_CONE_DISK,
  STOK_WHEEL,
  STOK_HELIX,
  STOK_TEAPOT,
  STOK_WALLS,
  /* dimensions */
  STOK_URL,
  STOK_SIZE,
  STOK_RADIUS,
  STOK_RADIUS2,
  STOK_RADIUS3,
  STOK_PTSIZE,
  STOK_WIDTH,
  STOK_DEPTH,
  STOK_HEIGHT,
  STOK_LENGTH,
  STOK_THICK,
  STOK_SIDE,
  /* position */
  STOK_REL,
  /* textures */
  STOK_TEXTURE,
  STOK_TEX_XP,
  STOK_TEX_XN,
  STOK_TEX_YP,
  STOK_TEX_YN,
  STOK_TEX_ZP,
  STOK_TEX_ZN,
  /* textures Repeat */
  STOK_TEXTURE_R,
  STOK_TEX_XP_R,
  STOK_TEX_XN_R,
  STOK_TEX_YP_R,
  STOK_TEX_YN_R,
  STOK_TEX_ZP_R,
  STOK_TEX_ZN_R,
  STOK_TEXTURE_RS,
  STOK_TEX_XP_RS,
  STOK_TEX_XN_RS,
  STOK_TEX_YP_RS,
  STOK_TEX_YN_RS,
  STOK_TEX_ZP_RS,
  STOK_TEX_ZN_RS,
  STOK_TEXTURE_RT,
  STOK_TEX_XP_RT,
  STOK_TEX_XN_RT,
  STOK_TEX_YP_RT,
  STOK_TEX_YN_RT,
  STOK_TEX_ZP_RT,
  STOK_TEX_ZN_RT,
  /* materials */
  STOK_DIFFUSE,
  STOK_AMBIENT,
  STOK_SPECULAR,
  STOK_EMISSION,
  STOK_SHININESS,
  STOK_ALPHA,
  STOK_FOG,
  /* drawing */
  STOK_STYLE,
  STOK_FACE,
  STOK_SLICES,
  STOK_STACKS,
  STOK_CYLINDERS,
  STOK_CIRCLES,
  STOK_SPOKES,
  STOK_BLINK,
  /* frames */
  STOK_FRAMES,
  STOK_BEGINFRAME,
  STOK_ENDFRAME,
  STOK_MODEL,
  STOK_SCALE,
  STOK_SCALEX,
  STOK_SCALEY,
  STOK_SCALEZ,
  STOK_SOLID,
  /* lightings */
  STOK_SPOT_DIRECTION,
  STOK_SPOT_CUTOFF,
  STOK_CONSTANT_ATTENUAT,
  STOK_LINEAR_ATTENUAT,
  STOK_QUADRATIC_ATTENUAT
};

// local

struct sStokens {
  const char *tokstr;
  const char *tokalias;
  int tokid;
};

static const struct sStokens stokens[] = {
  { "solid", "solid", STOK_SOLID },
  { "none", "fictif", STOK_NONE },
  { "shape", "shape", STOK_SHAPE },
  { "box", "cube", STOK_BOX },
  { "man", "woman", STOK_MAN },
  { "guy", "boy", STOK_GUY },
  { "android", "human", STOK_ANDROID },
  { "car", "car", STOK_CAR },
  { "sphere", "ball", STOK_SPHERE },
  { "cone", "cylinder", STOK_CONE },
  { "torus", "torus", STOK_TORUS },
  { "rect", "rectangle", STOK_RECT },
  { "disk", "disk", STOK_DISK },
  { "line", "line", STOK_LINE },
  { "pyr", "pyramid", STOK_PYRAMID },
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


/* New Solid. */
Solid::Solid()
{
  new_solid++;
  shape = STOK_BOX;	// shape by default: box
  dlists = NULL;	// solid display lists
  wobject = NULL;	// wobject associated with this solid set by addSolid in wobject.cpp
  bbcent = setV3(0, 0, 0);
  bbsize = setV3(0, 0, 0);
  idxframe = 0;		// frame index in displaylist
  frame = 0;		// frame to render
  nbframes = 1;		// 1 frame by default
  isframed = false;	// mono framed by default
  isflashy = false;
  isflary = false;
  isreflex = false;
  isblinking = false;
  blink = false;
  is_visible = true;	// visible by default
  is_opaque = true;	// opaque by default
  is_fictif = false;	// true solid
  userdist = 0;		// distance to localuser
  surfsize = 0;		// surface of solid
  ray_dlist = 0;	// ray display-list
  rendered = false;	// flag if alredy rendered
  ::g.render.relsolidList.push_back(this);	// add solid to relsolidList
  nbsolids = ::g.render.relsolidList.size();	// number of solids

  for (int i=0; i<5; i++) pos[i] = 0;
  for (int i=0; i<3; i++) flashcol[i] = 1;  // white
}

/* Deletes solid from display-list. */
Solid::~Solid()
{
  ::g.render.delFromList(this);
  ::g.render.relsolidList.clear();
  nbsolids = 0;

  delete[] dlists;
  del_solid++;
}

char * Solid::skipEqual(char *p)
{
  if (p && (p = strchr(p, '='))) p++;
  return p;
}

char * Solid::getTok(char *l, uint16_t *tok)
{
  char *t = l;
  const struct sStokens *ptab;

  *tok = STOK_ERR;
  l = skipEqual(l);
  if (l) {
    *(l-1) = '\0';	// end of token '=',  replaced by null terminated
    for (ptab = stokens; ptab->tokstr ; ptab++) {
      if ( (!strcmp(ptab->tokstr, t)) || (!strcmp(ptab->tokalias, t)) ) {
        *tok = ptab->tokid;
        return l;
      }
    }
    error("getTok: unknown \"%s\" in %s", t, l);
    return l;
  }
  else {
    error("getTok: in %s", t);
    return t;
  }
}

/* Parses <frame , return next token after frame. */
char * Solid::parseFrame(char *l)
{
  if (! strcmp(l, "frame")) {
    isframed = true;
    char *p = wobject->parse()->nextToken();
    return p;
  }
  while (l && isframed) {
    if (! stringcmp(l, "frame"))
      return wobject->parse()->nextToken();
    l = wobject->parse()->nextToken();
  }
  return l;
}

/* Sets shape from shape="...", return next token. */
char * Solid::parseShape(char *l, uint8_t *shape)
{
  const struct sStokens *ptab;
  char s[16];

  if (! stringcmp(l, "shape=")) {
    l = wobject->parse()->parseString(l, s, "shape");
    for (ptab = stokens; ptab->tokstr ; ptab++) {
      if ((! strcmp(ptab->tokstr, s)) || (! strcmp(ptab->tokalias, s))) {
        *shape = ptab->tokid;
        return l;
      }
    }
    error("unknown shape \"%s\" in %s'", s, l);
    *shape = STOK_ERR;
  }
  return l;
}

/*
 * solid parser.
 * returns next token if it exists
 * called by parseSolid(l) in parse.cpp.
 */
char * Solid::parser(char *l)
{
  if (!l) {
    error("no solid content");
    wobject->parse()->printNumline();
    return NULL;
  }
  char *ll = strdup(l);	// copy origin line for debug

  if (*l == '<') l++;	// skip open-tag
  if (! stringcmp(l, "frames=")) {
    l = wobject->parse()->parseUInt8(l, &nbframes, "frames");
  }
  dlists = new GLint[nbframes];

  ::g.render.addToList(this);	// add to solidList
  idM4(&matpos);	// init position to 0

  // axis aligned bounding boxes (AABB) are here
  V3 bbmax = setV3(0, 0, 0);
  V3 bbmin = setV3(0, 0, 0);

  // for each frame
  for (idxframe = 0; idxframe < nbframes; ) {
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
      case STOK_PYRAMID:
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
      case STOK_ANDROID:
      case STOK_CAR:
        r = solidParser(l, bbmax, bbmin); break;
      case STOK_STATUE:
      case STOK_MODEL:
        r = statueParser(l, bbmax, bbmin); break;
      default:
        delete[] dlists;
        dlists = NULL;
        return NULL;
    }

    if (r == -1) {
      error("parser error: shape=%hu ll=%s", shape, ll);
      free(ll);
      delete this;
      return NULL;
    }
    idxframe += r;
  }

  // computes relative AABB of this solid: bbcent and bbsize
  for (int i=0; i<3; i++) {
    bbcent.v[i] = (bbmax.v[i] + bbmin.v[i]) / 2;
    bbsize.v[i] = (bbmax.v[i] - bbmin.v[i]) / 2;
  }

  // computes max surface of this solid
  surfsize = MAX( bbsize.v[0]*bbsize.v[1], bbsize.v[0]*bbsize.v[2] );
  surfsize = MAX (surfsize, bbsize.v[1]*bbsize.v[2] );	// surface max
  //dax surfsize = bbsize.v[0] * bbsize.v[1] * bbsize.v[2];	// volume notused

  /* next token */
  l = wobject->parse()->nextToken();
  if (l && !strcmp(l, "/solid")) {	// md2
    l = wobject->parse()->nextToken();	// skip </solid>
  }

  return l;
}

/*
 * Single solid parser.
 * returns bbmax bbmin
 */
int Solid::solidParser(char *l, V3 &bbmax, V3 &bbmin)
{
  if (!l) {
    error("no solid description");
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
  alpha = DEF_ALPHA;	// opaque

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
  texid = -1;			// for quadrics (sphere, cylinder, disc, torus, ...)
  int box_tex[6] = {-1,-1,-1,-1,-1,-1};	// for parallepipedic shapes
  GLfloat box_texrep[6][2] = { {1,1}, {1,1}, {1,1}, {1,1}, {1,1}, {1,1} };
  GLfloat tex_r_s = 1, tex_r_t = 1;

  Teapot *teapot = NULL;

  V3 dim = setV3(.1, .1, .1);	// default dimension : box 10x10x10 cm

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

  // parse after shape
  while (l) {
    uint16_t tok = 0; uint8_t flgblk = 0;

    if ((*l == '\0') || (*l == '/')) { *l = '\0'; break; } // end of solid

    l = getTok(l, &tok);
    switch (tok) {
      case STOK_SOLID:	//not used
        error("token solid");
        break;
      case STOK_URL:
        break; //TODO
      case STOK_SIZE:
        l = wobject->parse()->parseVector3fv(l, &dim);
        break;
      case STOK_DIFFUSE:
        l = wobject->parse()->parseVector3f(l, &mat_diffuse[0]);
        for (int i=0; i<3; i++) mat_ambient[i] = mat_diffuse[i];
        break;
      case STOK_AMBIENT:
        l = wobject->parse()->parseVector3f(l, &mat_ambient[0]);
        break;
      case STOK_SPECULAR:
        l = wobject->parse()->parseVector3f(l, &mat_specular[0]);
        break;
      case STOK_EMISSION:
        l = wobject->parse()->parseVector3f(l, &mat_emission[0]);
        for (int i=0; i<3; i++) mat_ambient[i] = mat_emission[i];
        break;
      case STOK_SHININESS:
        l = wobject->parse()->parseInt(l, &mat_shininess[0]);
        break;
      case STOK_ALPHA:
        l = wobject->parse()->parseFloat(l, &alpha);
        mat_diffuse[3] = mat_ambient[3] = alpha;
        if (alpha < 1)
          is_opaque = false;
        break;
      case STOK_SCALE:
        l = wobject->parse()->parseFloat(l, &scale);
        break;
      case STOK_SCALEX:
        l = wobject->parse()->parseFloat(l, &scalex);
        break;
      case STOK_SCALEY:
        l = wobject->parse()->parseFloat(l, &scaley);
        break;
      case STOK_SCALEZ:
        l = wobject->parse()->parseFloat(l, &scalez);
        break;
      case STOK_RADIUS3:
        l = wobject->parse()->parseFloat(l, &radius3);
        break;
      case STOK_RADIUS2:
        l = wobject->parse()->parseFloat(l, &radius2);
        break;
      case STOK_RADIUS:
        l = wobject->parse()->parseFloat(l, &radius);
        break;
      case STOK_HEIGHT:
        l = wobject->parse()->parseFloat(l, &height);
        break;
      case STOK_LENGTH:
        l = wobject->parse()->parseFloat(l, &length);
        break;
      case STOK_WIDTH:
        l = wobject->parse()->parseFloat(l, &width);
        break;
      case STOK_THICK:
        l = wobject->parse()->parseFloat(l, &thick);
        break;
      case STOK_SIDE:
        l = wobject->parse()->parseFloat(l, &side);
        break;
      case STOK_PTSIZE:
        l = wobject->parse()->parseFloat(l, &radius);
        break;
      case STOK_FOG:
        l = wobject->parse()->parseFloat(l, &fog[0]);
        for (int i=0; i<3; i++) {
          fog[i+1] = mat_diffuse[i];
        }
        break;
      case STOK_STYLE:
        l = wobject->parse()->parseUInt8(l, &style);
        break;
      case STOK_FACE:
        l = wobject->parse()->parseUInt8(l, &face);
        break;
      case STOK_SLICES:
        l = wobject->parse()->parseUInt8(l, &slices);
        break;
      case STOK_STACKS:
        l = wobject->parse()->parseUInt8(l, &stacks);
        break;
      case STOK_CYLINDERS:
        l = wobject->parse()->parseUInt8(l, &cylinders);
        break;
      case STOK_CIRCLES:
        l = wobject->parse()->parseUInt8(l, &circles);
        break;
      case STOK_SPOKES:
        l = wobject->parse()->parseUInt8(l, &spokes);
        break;
      case STOK_SPOT_CUTOFF:
        l = wobject->parse()->parseFloat(l, &light_spot_cutoff[0]);
        break;
      case STOK_SPOT_DIRECTION:
        l = wobject->parse()->parseVector3f(l, &light_spot_direction[0]);
        break;
      case STOK_CONSTANT_ATTENUAT:
        l = wobject->parse()->parseFloat(l, &light_constant_attenuation[0]);
        break;
      case STOK_LINEAR_ATTENUAT:
        l = wobject->parse()->parseFloat(l, &light_linear_attenuation[0]);
        break;
      case STOK_QUADRATIC_ATTENUAT:
        l = wobject->parse()->parseFloat(l, &light_quadratic_attenuation[0]);
        break;
      case STOK_BLINK:
        l = wobject->parse()->parseUInt8(l, &flgblk);
        if (flgblk) {
          blink = true;
          setBlinking(true);
        }
        break;
      case STOK_REL:
        l = wobject->parse()->parseVector5f(l, pos);
        //dax ::g.render.relsolidList.push_back(this);	// add rel solid to relsolidList
        nbsolids = ::g.render.relsolidList.size();
        break;
      case STOK_TEXTURE:
        { char *urltex = new char[URL_LEN];
          l = wobject->parse()->parseString(l, urltex);
          if (*urltex) {
            texture = new Texture(urltex);
            for (int i=0; i<6 ; i++)
              box_tex[i] = texture->id;
            texid = texture->id;
            texture->object = wobject;
            texture->solid = this;
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
          l = wobject->parse()->parseString(l, urltex);
          if (*urltex) {
            texture = new Texture(urltex);
            box_tex[tok - STOK_TEX_XP] = texture->id;
            texid = texture->id;
            texture->object = wobject;
            texture->solid = this;
          }
          delete[] urltex;
        }
        break;
      case STOK_TEXTURE_R:
        l = wobject->parse()->parseFloat(l, &tex_r_s);
        tex_r_t = tex_r_s;
        break;
      case STOK_TEXTURE_RS:
        l = wobject->parse()->parseFloat(l, &tex_r_s);
        break;
      case STOK_TEXTURE_RT:
        l = wobject->parse()->parseFloat(l, &tex_r_t);
        break;
      case STOK_TEX_XP_R:
      case STOK_TEX_XN_R:
      case STOK_TEX_YP_R:
      case STOK_TEX_YN_R:
      case STOK_TEX_ZP_R:
      case STOK_TEX_ZN_R:
        l = wobject->parse()->parseFloat(l, &box_texrep[tok-STOK_TEX_XP_R][0]);
        box_texrep[tok-STOK_TEX_XP_R][1] = box_texrep[tok-STOK_TEX_XP_R][0];
        break;
      case STOK_TEX_XP_RS:
      case STOK_TEX_XN_RS:
      case STOK_TEX_YP_RS:
      case STOK_TEX_YN_RS:
      case STOK_TEX_ZP_RS:
      case STOK_TEX_ZN_RS:
        l = wobject->parse()->parseFloat(l, &box_texrep[tok-STOK_TEX_XP_RS][0]);
        break;
      case STOK_TEX_XP_RT:
      case STOK_TEX_XN_RT:
      case STOK_TEX_YP_RT:
      case STOK_TEX_YN_RT:
      case STOK_TEX_ZP_RT:
      case STOK_TEX_ZN_RT:
        l = wobject->parse()->parseFloat(l, &box_texrep[tok-STOK_TEX_XP_RT][1]);
        break;
      default:
        error("solidParser: bad token=%hu", tok);
        return -1;
    }
  }

  /*
   * draw solid in displaylist
   */
  // display list generation
  int dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);

  glCullFace(GL_BACK);		// don't draw back face
  glShadeModel(GL_SMOOTH);

  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
  glMaterialiv(GL_FRONT, GL_SHININESS, mat_shininess);

  switch (shape) {

    case STOK_NONE:
      is_fictif = true;
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
      if (! radius)
        radius = (dim.v[0] + dim.v[1] + dim.v[2]) / 3;
      dim.v[0] = dim.v[1] = dim.v[2] = 2*radius;
      setBB(2*radius, 2*radius, 2*radius);
      //dax1 if (::g.pref.bbox) Draw::bbox(2*radius, 2*radius, 2*radius);
      break;

    case STOK_MAN:
      if (localuser->man) {
        localuser->man->draw();
      }
      else {
        Man *man = new Man(dim.v[0], dim.v[1], dim.v[2]);
        man->draw();
      }
      setBB(dim.v[0]/2, dim.v[1]/2, dim.v[2]/2);
      break;

    case STOK_GUY:
    case STOK_ANDROID:
      setBB(dim.v[0]/2, dim.v[1]/2, dim.v[2]/2);
      break;

    case STOK_CONE:
      if (radius == radius2)
        stacks = 1;	// cylinder
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

    case STOK_PYRAMID:
      preDraw(texid, alpha, fog, true);
      Draw::pyramid(side, height, style);
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
      if (radius == radius3) stacks = 1; // cylinder
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
      Car::draw(dim.v[0], dim.v[1], dim.v[2], box_tex, box_texrep, slices, style);
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
      //dax5 Wheel *wheel = new Wheel(spokes, radius, mat_diffuse);
      break;
  }
  glEndList();

  // sets dlists number for this frame
  dlists[idxframe] = dlist;

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
    case STOK_ELLIPSE: // 2D shapes without BBox
      break;
    default: // with bounding boxes
      getBB(bbmax, bbmin, idxframe != 0);
  }

  updateDist();

  return 1;	// only one frame
}

/* compute distance to localuser */
void Solid::updateDist()
{
  if (localuser) {
    float dx = ABSF(localuser->pos.x - object()->pos.x);
    float dy = ABSF(localuser->pos.y - object()->pos.y);
    userdist = sqrt(dx*dx + dy*dy);
  }
}

/*
 * Parses statue and returns f (number of frames).
 */
int Solid::statueParser(char *l, V3 &bbmax, V3 &bbmin)
{
  int texid = -1;
  GLfloat scale = DEF_SCALE;
  uint8_t firstframe = 1;
  uint8_t lastframe = nbframes;
  uint8_t tabframes[FRAME_MAX] = { 0 };
  char *urlmdl = NULL;
  fog[0] = 0;

  alpha = DEF_ALPHA;

  ::g.render.first_bb = true;

  while (l) {
    uint16_t tok;

    if ( (*l == '\0') || (*l == '/') ) break; // end of solid

    l = getTok(l, &tok);
    if (tok == 0) break;

    switch (tok) {
      case STOK_SOLID: break;
      case STOK_MODEL:
      case STOK_URL:
        urlmdl = new char[URL_LEN];
        l = wobject->parse()->parseString(l, urlmdl);
        break;
      case STOK_SCALE:
        l = wobject->parse()->parseFloat(l, &scale);
        break;
      case STOK_BEGINFRAME:
        l = wobject->parse()->parseUInt8(l, &firstframe);
        break;
      case STOK_ENDFRAME:
        l = wobject->parse()->parseUInt8(l, &lastframe);
        break;
      case STOK_DIFFUSE:
        l = wobject->parse()->parseVector3f(l, &mat_diffuse[0]);
        for (int i=0; i<3; i++) {
          mat_ambient[i] = mat_diffuse[i];
        }
        break;
      case STOK_AMBIENT:
        l = wobject->parse()->parseVector3f(l, &mat_ambient[0]);
        break;
      case STOK_SPECULAR:
        l = wobject->parse()->parseVector3f(l, &mat_specular[0]);
        break;
      case STOK_ALPHA:
        l = wobject->parse()->parseFloat(l, &alpha);
        mat_diffuse[3] = mat_ambient[3] = alpha;
        if (alpha < 1)
          is_opaque = false;
        break;
      case STOK_TEXTURE:
        { char *urltex = new char[URL_LEN];
          l = wobject->parse()->parseString(l, urltex);
          if (*urltex) {
            texture = new Texture(urltex);
            texid = texture->id;
            texture->object = wobject;
          }
          delete[] urltex;
        }
        break;
      default:
        error("statueParser: bad tok=%d", tok);
        return -1;
    }
  }
  firstframe = (firstframe == 0) ? 1 : firstframe;
  if (lastframe < firstframe) lastframe = firstframe;

  int nf;
  for (nf=0; firstframe <= lastframe; nf++)
    tabframes[nf] = firstframe++;
  tabframes[nf] = 0;

  if (urlmdl) {	// model url exists
    int model = Format::getModelByUrl(urlmdl);
    switch (model) {

      case MODEL_MD2: {
        Md2 *md2 = new Md2(urlmdl);

        for (nf=0; tabframes[nf] && nf < FRAME_MAX; nf++) {
          md2->setScale(scale);
          if ((dlists[nf] = md2->displaylist(tabframes[nf], texid)) < 0) {
            nf = -1;
            break;
          }
          getBB(bbmax, bbmin, isframed); // get bounding box
        }
        if (md2) delete md2;
        md2 = NULL;
        break;
        }

      case MODEL_OBJ: {
        Obj *obj = new Obj(urlmdl, 0);

        obj->setScale(scale);
        obj->setColor(GL_DIFFUSE, mat_diffuse);
        obj->setColor(GL_AMBIENT, mat_diffuse);
        obj->setColor(GL_SPECULAR, mat_specular);
        dlists[0] = obj->displaylist();
        nf = 1;
        if (obj) delete obj;
        obj = NULL;
        break;
        }
    }
    delete[] urlmdl;
  }
  return nf;	// number nf frames
}

void Solid::doTransform(bool flag)
{
  switch ((int)flag) {
  case true:  // pre
    glPushMatrix();
    glRotatef(RAD2DEG(pos[3]), 0, 0, 1);	// az
    glRotatef(RAD2DEG(pos[4]), 1, 0, 0);	// ax
    glTranslatef(pos[0], pos[1], pos[2]);	// x y z
    glScalef(scalex, scaley, scalez);
    break;
  case false: // post
    glPopMatrix();
    break;
  }
}

void Solid::doBlend(bool flag, GLfloat _alpha)
{
  if (_alpha < 1) {
    is_opaque = false;
    switch ((int)flag) {
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

void Solid::doTexture(bool flag, int _texid)
{
  if (_texid >= 0) {
    switch ((int)flag) {
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

// Gets max min BB.
void Solid::getBB(V3 &max, V3 &min, bool is_framed)
{
  ::g.render.getBB(max, min, is_framed);
}

// Sets BB dims.
void Solid::setBB(GLfloat w, GLfloat d, GLfloat h)
{
  ::g.render.setBB(w*1, d*1, h*1);
}

/* returns relative center and size of BB. */
void Solid::getRelBB(V3 &center, V3 &size) const
{
  center = bbcent;
  size = bbsize;
}

/* returns size of BB. */
void Solid::getDimBB(V3 &dim) const
{
  dim = bbsize;
}

/* returns relative center of BB. */
void Solid::getCentBB(V3 &center) const
{
  center = bbcent;
}

/* returns materials. */
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

/* Returns absolute center and size of AABB. */
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
    if (n == 0)
      vmin = vmax = vabs;
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
  ::g.render.updateBB(az);
}

// accessor - get WObject parent from Solid
WObject* Solid::object() const
{
  return wobject;
}

void Solid::setPosition(const M4 &mpos)
{
  matpos = mpos;
}

void Solid::getPosition(M4& mpos)
{
  mpos = matpos;
}

void Solid::setVisible(bool _isvisible)
{
  is_visible = _isvisible;
}

bool Solid::isVisible() const
{
  return is_visible;
}

void Solid::setRendered(bool _rendered)
{
  rendered = _rendered;
}

bool Solid::isRendered() const
{
  return rendered;
}

bool Solid::isOpaque() const
{
  return is_opaque;
}

bool Solid::isFlashy() const
{
  return isflashy;
}

bool Solid::isFlary() const
{
  return isflary;
}

void Solid::setTransparent(float _alpha)
{
  alpha = _alpha;
  if (alpha < 1)
    is_opaque = false;
  else
    is_opaque = true;
}

void Solid::setFlary(bool flag)
{
  isflary = flag;
}

void Solid::setReflexive(bool flag)
{
  isreflex = flag;
}

bool Solid::isReflexive() const
{
  return isreflex;
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
  GLfloat ox = wobject->pos.x;
  GLfloat oy = wobject->pos.y;
  GLfloat oz = wobject->pos.z;
  // ray color
  const GLfloat *color = Color::white;  // white

  Draw::ray(&ray_dlist, ex, ey, ez, ox, oy, oz, color, 0x3333);  // alternative

  localuser->ray = setV3(ox, oy, oz);
  localuser->noh->declareObjDelta(User::PROPRAY);  // publish ray property to network
}

void Solid::resetRay()
{
  if (ray_dlist) {
    ray_dlist = 0;
    if (localuser) {
      localuser->ray = setV3(0, 0, 0);
      localuser->noh->declareObjDelta(User::PROPRAY);
    }
  }
}

void Solid::setFlashyEdges(bool flag)
{
  isflashy = flag;
}

void Solid::setFlashyEdges(const GLfloat *color)
{
  isflashy = true;
  for (int i=0; i<3; i++)  flashcol[i] = color[i];
}

void Solid::resetFlashyEdges()
{
  isflashy = false;
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
  isblinking = flag;
}

bool Solid::isBlinking() const
{
  return isblinking;
}

uint8_t Solid::getFrames() const
{
  return nbframes;
}

uint8_t Solid::getFrame() const
{
  return frame;
}

void Solid::setFrame(uint8_t _frame)
{
  frame = _frame % nbframes;
}

/* transpose vreng to opengl coordinates system */
void Solid::vr2gl()
{
  GLfloat gl_mat[16];

  M4toV16(&matpos, gl_mat);
  glMultMatrixf(gl_mat);
}

GLint Solid::getDlist() const
{
  return dlists[0];
}

GLint Solid::getTexid() const
{
  return texid;
}

//---------------------------------------------------------------------------
//
// Rendering solids
//

void Solid::displaySolid(render_type type)
{
  if (isBlinking() && (! toggleBlinking()))
    return;		// pass one turn

  switch (type) {

    case OPAQUE:	// Display opaque solids
      if (isReflexive())
        displayList(REFLEXIVE);
      else
        displayList(NORMAL);
      break;

    case TRANSPARENT:	// Display transparent solids 
      if (isReflexive())
        displayList(REFLEXIVE);
      else
        displayList(NORMAL);
      break;

    case FLASH:		// Display flashy edges
      if (isFlashy())
        displayList(FLASHY);
      if (isFlary())
        displayFlary();	// Display attached flares
      if (ray_dlist)
        displayRay();
      break;

    case USER:		// Display local user last
      displayList(NORMAL);
      break;
  }
}

void Solid::displayRay()
{
  glPushAttrib(GL_LINE_BIT);
  glPushMatrix();
   glDisable(GL_LIGHTING);
   glEnable(GL_LINE_STIPPLE);

   glCallList(ray_dlist);

   glDisable(GL_LINE_STIPPLE);
   glEnable(GL_LIGHTING);
  glPopMatrix();
  glPopAttrib();
}

void Solid::displayFlary()
{
  if (object()->flare) {
    displayList(NORMAL);  // object alone

    glPushMatrix();
     glRotatef(RAD2DEG(localuser->pos.az), 0, 0, -1);
     glTranslatef(object()->pos.x, object()->pos.y, object()->pos.z);

     object()->flare->render(object()->pos);

    glPopMatrix();
  }
}


/* Renders a solid calling its dlists. */
int Solid::displayList(int display_mode = NORMAL)
{
  GLint bufs = GL_NONE;

  if (! dlists)  return 0;
  if (dlists[frame] <= 0 )  return 0;

  glPushMatrix();
  {
   vr2gl();	// transpose vreng coord to opengl coord

   switch (display_mode) {

   case NORMAL:
     glPushMatrix();

     if (wobject->type == USER_TYPE) {	// if localuser
       User *user = (User *) wobject;
       glPushMatrix();
       glTranslatef(user->pos.x, user->pos.y, user->pos.z);     // x y z
       if (user->guy || user->android) {
         glRotatef(RAD2DEG(-user->pos.ax), 1, 0, 0);
         glRotatef(RAD2DEG(-user->pos.ay), 0, 1, 0);
         glRotatef(RAD2DEG(-user->pos.az), 0, 0, 1);
       }
       if (user->man) {
         glRotatef(RAD2DEG(-user->man->pos.ax), 1, 0, 0);
         glRotatef(RAD2DEG(-user->man->pos.ay), 0, 1, 0);
         glRotatef(RAD2DEG(-user->man->pos.az), 0, 0, 1);
         //glScalef(.5, .5, .5);
       }
       else {	// box
         glRotatef(RAD2DEG(-user->pos.ax), 1, 0, 0);
         glRotatef(RAD2DEG(-user->pos.ay), 0, 1, 0);
         glRotatef(RAD2DEG(-user->pos.az), 0, 0, 1);
         //glScalef(.5, .5, .5);
       }

       glCallList(dlists[frame]);	// display the localuser here !!!

       glPopMatrix();
     }
     else {			// normal solid
       glEnable(GL_DEPTH_TEST);
       glDepthMask(GL_TRUE);
       glDepthFunc(GL_LESS);

       glPushMatrix();
        glTranslatef(pos[0], pos[1], pos[2]);     // x y z
        glRotatef(RAD2DEG(pos[3]), 0, 0, 1);      // az
        glRotatef(RAD2DEG(pos[4]), 1, 0, 0);      // ax
        if (scalex != 1 || scaley != 1 || scalez != 1)
          glScalef(scalex, scaley, scalez);
       glPopMatrix();

       if (texid >= 0) {
         glEnable(GL_TEXTURE_2D);
         glBindTexture(GL_TEXTURE_2D, texid);
       }
       if (alpha < 1) {		// transparent
         glDepthMask(GL_FALSE);
         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// without effect
       }
       if (*fog > 0) {
         error("fog=%f %s", *fog, object()->getInstance());
         glEnable(GL_FOG);
         glFogi(GL_FOG_MODE, GL_EXP);
         glFogf(GL_FOG_DENSITY, *fog);
         glFogfv(GL_FOG_COLOR, fog+1);
       }

       glCallList(dlists[frame]);	// display the object here !!!

     }
     glPopMatrix();
     break;

   case FLASHY:
     glPushMatrix();
      glPolygonOffset(2., 1.);		// factor=2 unit=1
      glDisable(GL_POLYGON_OFFSET_FILL);// wired mode
      glColor3fv(flashcol);
      glLineWidth(1);
      glScalef(1.03, 1.03, 1.03);	// 3%100 more
      glPolygonMode(GL_FRONT, GL_LINE);

      glCallList(dlists[frame]);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
     glPopMatrix();
     break;

   case REFLEXIVE:
    glGetIntegerv(GL_DRAW_BUFFER, &bufs); 	// get the current color buffer being drawn to
    glPushMatrix();
     glClearStencil(0);				// set the clear value
     glClear(GL_STENCIL_BUFFER_BIT);		// clear the stencil buffer
     glStencilFunc(GL_ALWAYS, 1, 1);		// always pass the stencil test
     glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); // operation to modify the stencil buffer
     glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
     glDrawBuffer(GL_NONE);			// disable drawing to the color buffer
     glEnable(GL_STENCIL_TEST);			// enable stencil

     glCallList(dlists[frame]);			// display the mirror inside the stencil

     glStencilFunc(GL_ALWAYS, 1, 1);		// always pass the stencil test
     glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
     glDrawBuffer((GLenum) bufs);		// reenable drawing to color buffer
     glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); 	// make stencil buffer read only

     glPushMatrix();
      GLdouble eqn[4] = {-0,-0,-1,0};		// clipping plane
      glClipPlane(GL_CLIP_PLANE0, eqn);
      glEnable(GL_CLIP_PLANE0);			// enable clipping
      glStencilFunc(GL_EQUAL, 1, 1);		// draw only where the stencil == 1

      object()->render();			// display the mirrored scene by mirror itself

      glDisable(GL_CLIP_PLANE0);
     glPopMatrix();
     glDisable(GL_STENCIL_TEST);		// disable the stencil

     glDepthMask(GL_FALSE);
     glEnable(GL_BLEND);			// mirror shine effect
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     glDepthFunc(GL_LEQUAL);

     glCallList(dlists[frame]);			// display the physical mirror

     glDepthFunc(GL_LESS);
     glDisable(GL_BLEND);
     glDepthMask(GL_TRUE);
    glPopMatrix();
    break;	// reflexive
   }

   if (*fog > 0) {
     glDisable(GL_FOG);
   }
   if (texid >= 0) {
     glDisable(GL_TEXTURE_2D);
   }
   if (alpha < 1) {	// transparent
     glDisable(GL_BLEND);
     glDepthMask(GL_TRUE);
   }
  }
  glPopMatrix();

  return dlists[frame];		// displaylist number
}

/* Display mirrored scene */
void Solid::displayMirroredScene()
{
#if 0 //dax done by mirror object

  // 1) faire une translation pour amener le plan de reflexion à la position miroir
  glTranslatef(-object()->pos.x, 0, 0);
  // 2) le miroir est dans le plan YZ; faire une reflexion par -1 en X
  glScalef(-1, 1, 1);
  // 3) mettre un plan de clipping a la position du miroir afin d'eliminer
  //    les reflexions des objets qui sont à l'arriere du miroir
  // 4) faire la translation inverse
  glTranslatef(object()->pos.x, 0, 0);
  // D) displays scene (opaque solids only)
  for (list<WObject*>::iterator o = objectList.begin(); o != objectList.end(); o++) {
    if ((*o) && (*o)->isVisible() && (*o)->isOpaque()) {
      glPushMatrix();
       // rotation inverse lorsque que le miroir tourne parallelement a notre vision.
       glRotatef(RAD2DEG(object()->pos.az), 0,0,1);
       glRotatef(-RAD2DEG(object()->pos.ay), 0,1,0);
       glTranslatef(-object()->pos.x, object()->pos.y, -object()->pos.z);
       glScalef(1, -1, 1);
       (*o)->getSolid()->displayList(NORMAL);
      glPopMatrix();
    }
  }
  glPushMatrix();
   glRotatef(RAD2DEG(object()->pos.az), 0,0,1);
   glRotatef(-RAD2DEG(object()->pos.ay), 0,1,0);
   glTranslatef(-object()->pos.x, object()->pos.y, -object()->pos.z);

   // Displays avatar
   if  (localuser->android) localuser->android->getSolid()->displayList(NORMAL);
   else if (localuser->guy) localuser->guy->getSolid()->displayList(NORMAL);
   else if (localuser->man) localuser->getSolid()->displayList(NORMAL);
   else glCallList(localuser->getSolid()->displayList(NORMAL));
  glPopMatrix();

#endif //dax
}
