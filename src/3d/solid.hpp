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
/*
 * Les fonctions suivantes sont utilisees pour gerer des solides
 * disposes dans le monde.
 * On n'a pas specifie ici:
 * - la gestion des sources lumineuses et des modeles lumineux
 * - la gestion des hierarchies d'objets
 * - les fonctions d'entree/sortie pour les geometries et les scenes
 * - comment gerer plusieurs mondes ou affichages
 */
#ifndef SOLID_HPP
#define SOLID_HPP

#include "matvec.hpp"   // V3 M4
#include <list>

/* Solid tokens. */
enum {
  STOK_ERR = 0,
  /* shapes */
  STOK_SHAPE,
  STOK_NONE,
  STOK_BOX,
  STOK_MAN,
  STOK_GUY,
  STOK_HUMANOID,
  STOK_CAR,
  STOK_SPHERE,
  STOK_CONE,
  STOK_TORUS,
  STOK_RECT,
  STOK_DISK,
  STOK_LINE,
  STOK_PENTAGON,
  STOK_PYRAMID,
  STOK_OCTAGON,
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
  STOK_WINGS,
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


/**
 * Solid class
 */
class Solid {
  friend class Render;

 protected:
  M4		matpos;		///< position matrix.
  V3		bbcent;		///< relative center of BB.
  V3		bbsize;		///< relative half sizes of BB.
  float 	pos[5];		///< relative position/orientation.
  bool		visible;	///< should this solid be drawn ?.
  bool		opaque;		///< set if the solid is opaque.
  bool		flashy;		///< flag flashy.
  bool		flary;		///< flag flary.
  bool		reflexive;	///< flag reflexive.
  bool		blinking;	///< flag blinking.
  bool		blink;		///< flag blink.
  bool		rendered;	///< flag if is already rendered
  float		userdist;	///< distance to localuser.
  float		surfsize;	///< max surface size of solid.
  float		alpha;		///< transparency.
  float		scale, scalex, scaley, scalez;

 private:
  bool		fictif;		///< set if the solid is fictif.
  uint8_t	shape;		///< basic shape.
  uint8_t	nbsolids;	///< number of solids.
  uint8_t	nbframes;	///< number of frames of this solid.
  uint8_t	frame;		///< current frame to render.
  int		texid;		///< texture id.
  GLfloat	mat_diffuse[4];
  GLfloat	mat_ambient[4];
  GLfloat	mat_specular[4];
  GLfloat	mat_emission[4];
  GLint		mat_shininess[1];
  GLfloat 	flashcol[3];	///< flash color.
  GLfloat	fog[4];		///< fog params

// defaults constantes
static const uint8_t DEF_ALPHA;		// 1.	// opaque
static const uint8_t DEF_FOG;		// 0	// no fog
static const uint8_t DEF_SCALE;		// 1.	// no scale
static const uint8_t DEF_SHININESS;	// 20
static const uint8_t DEF_SPHERE_SLICES;	// 16
static const uint8_t DEF_SPHERE_STACKS;	// 16
static const uint8_t DEF_CONE_SLICES;	// 16
static const uint8_t DEF_CONE_STACKS;	// 8
static const uint8_t DEF_TORUS_CYLINDERS; // 16
static const uint8_t DEF_TORUS_CIRCLES;	// 16
static const uint8_t DEF_DISK_SLICES;	// 16
static const uint8_t DEF_WHEEL_SPOKES;	// 12
static const uint8_t DEF_DISK_LOOPS;	//  8
static const uint8_t FRAME_MAX;		// 255

 public:

  enum render_mode {
    DISPLAY,		///< render surfaces with plain colors.
    SELECT		///< selection in the Z-buffer.
  };

  enum render_type {
    OPAQUE,		///< opaque
    TRANSPARENT,	///< transparent
    FLASH,		///< flash effect
    USER		///< localuser
  };

  /* display modes */
  enum display_mode {
    NORMAL,
    FLASHY,
    REFLEXIVE,
    PROGRESS
  };

  class WO	*wobject;	///< WO parent.
  class Texture *texture;	///< texture if exists.
  GLint*	dlists;		///< cached display list.
  GLint		ray_dlist;	///< ray display list.

  Solid();			///< constructor.
  virtual ~Solid();		///< destructor.

  // Accessors
  WO* object() const;
  /**< Returns the associated WO. */

  void getDimBB(V3& bbsize) const;
  void getRelBB(V3& bbcent, V3& bbsize) const;
  /**< Returns relative bounding-box. */

  void getAbsBB(V3& bbcent, V3& bbsize);
  /**< Returns a bounding box for this solid.
   * The bounding box is aligned with the world reference. */

  void updateBB(GLfloat az);
  /**< Updates BB according to its orientation. */

  void getMaterials(GLfloat *dif, GLfloat *amb, GLfloat *spe, GLfloat *emi, GLint *shi, GLfloat *opa);
  /* Returns materials. */

  void setPosition(const M4& mpos);
  /**< Updates the solid position.
   * The position is defined by the matrix 4x4 'mpos'
   * which gives the transformation todo on local coords towards world coords.
   */

  void getPosition(M4& mpos);
  /**< Gives solid's position. */

  void updateDist();
  /**< Updates distance to localuser */

  void setVisible(bool v);
  /**< Sets the solid visible (true) or invisible (false). */

  bool isVisible() const;
  /**< Return if the solid should be drawn or not. */

  void setRendered(bool v);
  /**< Sets the solid rendered (true) or not (false). */

  bool isRendered() const;
  /**< Return if the solid should be rendered or not. */

  bool isOpaque() const;
  /**< Return if the solid is opaque or not. */

  void setTransparent(float _alpha);
  /**< Sets transparent. */

  bool isFlashy() const;
  /**< Return if the solid is flashy or not. */

  void setFlashyEdges(bool flag);
  /**< Sets the solid edges flashy. */

  bool isFlary() const;
  /**< Return if the solid is flary or not. */

  void setFlary(bool flag);
  /**< Sets the solid flary. */

  bool isReflexive() const;
  /**< Return if the solid is reflexive or not. */

  void setReflexive(bool flag);
  /**< Sets the solid reflexive. */

  // File parsing
  char* parser(char *l);	///< common Parser.

  char* parseFrame(char* l);
  char* parseShape(char* l, uint8_t *shape);
  int statueParser(char* l, V3& bbmax, V3& bbmin);
  int solidParser(char* l, V3& bbmax, V3& bbmin);

  uint8_t getFrames() const;
  /**< Gets the number of frames. */

  uint8_t getFrame() const;
  /**< Gets the index of current frames. */

  void setFrame(uint8_t frame);
  /**< Changes the "frame" of the solid (ie its geometry). */

  bool isBlinking() const;
  /**< Checks if blinking. */

  bool toggleBlinking();
  /**< Checks if rendering of a blinking solid must be done. */

  void setRay(GLint x, GLint y);
  /**< Makes a ray between user'eyes and the center of the object. */

  void resetRay();
  /**< Resets ray. */

  void setFlashyEdges(const GLfloat *_flashcol);
  /**< Sets the solid edges flashy with this color. */

  void resetFlashyEdges();
  /**< Resets the solid edges flashy. */

  void vr2gl();
  /**< Transposes vreng coordinates in opengl coordinates. */

  GLint getDlist() const;
  /**< Gets current displaylist number. */

  GLint getTexid() const;
  /**< Gets current texid. */

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Displaying 3D displaylists
  void displaySolid(render_type layer);
  /**< Issue the OpenGL commands to draw the solid in the given mode.
       It is called with "layer" to allow drawing at different layers. */

  int displayList(int layer);
  /**< Renders a solid in display-list. */

  void displayFlary();
  /**< Renders attached flare to a solid. */

  void displayRay();
  /**< Displays ray. */

 private:

  uint8_t	iframe;		///< frame index.
  bool		framed;		///< flag framed.

  char * skipEqual(char *p);
  /** Skips '=' character. */

  void setBlinking(bool flag);
  /**< Sets the solid blinking. */

  void preDraw(int texid, GLfloat alpha, GLfloat *fog, bool cull = false);
  /**< Begins transforms before drawing. */

  void postDraw(int texid, GLfloat alpha, GLfloat *fog);
  /**< Ends transforms after drawing. */

  void getMaxMinBB(V3 &max, V3 &min, bool framed);
  /**< Gets Max & Min BB. */

  void setBB(GLfloat w, GLfloat d, GLfloat h);
  /**< Sets BB sizes. */

  void doTransform(int flag);
  /**< Makes transforms rotations and translations. */

  void doBlend(int flag, GLfloat alpha);
  /**< Makes blending operations if alpha. */

  void doTexture(int flag, int texid);
  /**< Makes texture binding. */

  char* getTok(char* l, uint16_t* stok);
  /** Gets current solid token. */

};

#endif
