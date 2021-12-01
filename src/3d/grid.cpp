//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
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
//
// Author: Mario Trentini (2002)
//
#include "vreng.hpp"
#include "pref.hpp"
#include "grid.hpp"
#include "scene.hpp"

// local
static Grid _grid;	// grid instance

const uint8_t Grid::GRID_HEIGHT = 10;
const uint8_t Grid::GRID_SLICE = 5;
const uint8_t Grid::SCROLL_MAX = 100;
const uint8_t Grid::SCROLL_INCR = 1;


/* Accessor */
Grid * Grid::grid()
{
  return &_grid;
}

/* Constructor */
Grid::Grid()
{
  dlist = -1;
  visible = false;
  overlap = false;
  grid3d = false;
  behavior = STICK;
}

void Grid::defaults()
{
  rotx = roty = rotz = 0;
  posx = posy = 0;
  posz = 0;	//FIXME: user->pos.z - user->pos.bbsize.v[2] + epsilon
  height = GRID_HEIGHT;
  i_width = i_depth = i_height = GRID_SLICE;
  red = green = blue = alpha = 1;
}

/* Called by world.cpp: Grid::grid()->init */
void Grid::init(int _depth, int _width, int _height)
{
  defaults();

  // bbsize of the current world
  depth = _depth;
  width = _width;

  // fills the matrix transformation
  glmat[0]=0;  glmat[4]=-1; glmat[8] =0; glmat[12]=0;		// Xogl = -Yvre
  glmat[1]=0;  glmat[5]=0;  glmat[9] =1; glmat[13]=-1.85;	// Yogl = Zvre
  glmat[2]=-1; glmat[6]=0;  glmat[10]=0; glmat[14]=0;		// Zogl = -Xvre
  glmat[3]=0;  glmat[7]=0;  glmat[11]=0; glmat[15]=1;

  draw();
}

/*
 * Rendering
 */

void Grid::draw()
{
  Scene *scene = ::g.gui.scene();
  if (!scene || !scene->isInitialized()) return;

  if (dlist != -1)
    glDeleteLists(dlist, 1);

  int grid_height = (grid3d) ? i_height : 0;

  /* draw grid in displaylist */
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);

  glColor4f(red, green, blue, alpha);
  glBegin(GL_LINES);
  for (int j = -grid_height; j <= grid_height; j++) {
    float x, y, z;
    z = (float) (j * height) / i_height;
    for (int i = -i_width; i < i_width; i++) {
      y = (float) (i * width) / i_width;
      glVertex3f(-depth, y, z);
      glVertex3f( depth, y, z);
    }
    for (int i = -i_depth; i < i_depth; i++) {
      x = (float) (i * depth) / i_depth;
      glVertex3f(x, -width, z);
      glVertex3f(x,  width, z);
    }
  }

  if (grid3d) {
    for (int j = -i_width; j < i_width; j++) {
      float x, y;
      y = (float) (j * width) / i_width;
      for (int i = -i_depth; i < i_depth; i++) {
        x = (float) (i * depth) / i_depth;
        glVertex3f(x, y, -height);
        glVertex3f(x, y,  height);
      }
    }
  }
  glEnd();
  glEndList();
}

void Grid::render()
{
  if (! visible)
    return;

  glPushMatrix();
  glDisable(GL_LIGHTING);
  if (overlap)
    glDisable(GL_DEPTH_TEST); // no overlap by other objects
  GLfloat gl_proj[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, gl_proj);
  gl_proj[12] = gl_proj[13] = gl_proj[14] = 0;
  glLoadMatrixf(gl_proj);
  if (behavior == SFOLLOW) // load default view matrix
    glLoadMatrixf(glmat);
  else if (behavior == STICK) {
    glPopMatrix();
    glPushMatrix();
  }
  glLineWidth(1);
  glTranslatef(posx, posy, posz);
  glRotatef(rotz, 0, 0, 1);
  glRotatef(rotx, 0, 1, 0);
  glRotatef(roty, 1, 0, 0);
  glCallList(dlist);
  if (overlap)
    glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

/*
 * Dialog functions
 */

void Grid::genValues()
{
  s_width = &uhscrollbar(UOn::change / ucall(this, &Grid::setWidth));
  s_width->setValue(GRID_SLICE);
  s_width->setUnitIncrement(1);
  s_width->setBlockIncrement(1);
  s_height = &uhscrollbar(UOn::change / ucall(this, &Grid::setHeight));
  s_height->setValue(GRID_SLICE);
  s_height->setUnitIncrement(1);
  s_height->setBlockIncrement(1);
  s_depth = &uhscrollbar(UOn::change / ucall(this, &Grid::setDepth));
  s_depth->setValue(GRID_SLICE);
  s_depth->setUnitIncrement(1);
  s_depth->setBlockIncrement(1);

  s_red = &uhscrollbar(UOn::change / ucall(this, &Grid::setRed));
  s_red->setValue(SCROLL_MAX);
  s_red->setUnitIncrement(10);
  s_red->setBlockIncrement(10);
  s_green = &uhscrollbar(UOn::change / ucall(this, &Grid::setGreen));
  s_green->setValue(SCROLL_MAX);
  s_green->setUnitIncrement(10);
  s_green->setBlockIncrement(10);
  s_blue = &uhscrollbar(UOn::change / ucall(this, &Grid::setBlue));
  s_blue->setValue(SCROLL_MAX);
  s_blue->setUnitIncrement(10);
  s_blue->setBlockIncrement(10);

  s_x = &uhscrollbar(UOn::change / ucall(this, &Grid::setPosX));
  s_x->setUnitIncrement(1);
  s_x->setBlockIncrement(1);
  s_y = &uhscrollbar(UOn::change / ucall(this, &Grid::setPosY));
  s_y->setUnitIncrement(1);
  s_y->setBlockIncrement(1);
  s_z = &uhscrollbar(UOn::change / ucall(this, &Grid::setPosZ));
  s_z->setUnitIncrement(1);
  s_z->setBlockIncrement(1);

  s_rotx = &uhscrollbar(UOn::change / ucall(this, &Grid::setRotX));
  s_rotx->setUnitIncrement(1);
  s_rotx->setBlockIncrement(1);
  s_roty = &uhscrollbar(UOn::change / ucall(this, &Grid::setRotY));
  s_roty->setUnitIncrement(1);
  s_roty->setBlockIncrement(1);
  s_rotz = &uhscrollbar(UOn::change / ucall(this, &Grid::setRotZ));
  s_rotz->setUnitIncrement(1);
  s_rotz->setBlockIncrement(1);
}

/** Called by the GUI */
UBox * Grid::gridBox()
{
  URadioSelect &behavior = uradioSelect();
  
  genValues();	// if commented : ubit crashes UArgs::operator+()
  
  UBox& grid1 =
  uvbox(usize(160,400)
        + uvbox(UBorder::etchedIn
                + uhbox(UColor::blue + UFont::bold + "View")
                + ucheckbox("2D Grid" + ucall(this, &Grid::toggleGrid2d))
                + ucheckbox("3D Grid" + ucall(this, &Grid::toggleGrid3d))
                + ucheckbox("Overlap" + ucall(this, &Grid::toggleOverlap))
               )
        + uvbox(UBorder::etchedIn
                + uhbox(UColor::blue + UFont::bold + "Behavior")
                + ucheckbox(behavior
                            + "Stick to the world"
                            + UOn::select / ucall(this, (int) STICK, &Grid::toggleBehavior)
                           ).setSelected()
                + ucheckbox(behavior
                            + "Follow"
                            + UOn::select / ucall(this, (int) FOLLOW, &Grid::toggleBehavior))
                + ucheckbox(behavior
                            + "Strict follow"
                            + UOn::select / ucall(this, (int) SFOLLOW, &Grid::toggleBehavior))
               )
        + uvbox(UBorder::etchedIn
                + ubutton("Reset" + ucall(this, &Grid::defaults)))
   );

  UBox& grid2 = 
  uvbox(usize(160,400)
        + uvbox(UBorder::etchedIn
                + uhbox(UColor::blue + UFont::bold + "Slice ")
                + uhbox("Width:  " + uhflex() + s_width)
                + uhbox("Height: " + uhflex() + s_height)
                + uhbox("Depth:  " + uhflex() + s_depth)
               )
        + uvbox(UBorder::etchedIn
                + uhbox(UColor::blue + UFont::bold + "Orientation")
                + uhbox("Rot X: " + uhflex() + s_rotx)
                + uhbox("Rot Y: " + uhflex() + s_roty)
                + uhbox("Rot Z: " + uhflex() + s_rotz)
               )
        + uvbox(UBorder::etchedIn
                + uhbox(UColor::blue + UFont::bold + "Position")
                + uhbox("Pos X: " + uhflex() + s_x)
                + uhbox("Pos Y: " + uhflex() + s_y)
                + uhbox("Pos Z: " + uhflex() + s_z)
               )
        + uvbox(UBorder::etchedIn
                + uhbox(UColor::blue + UFont::bold + "Color ")
                + uhbox("R: " + uhflex() + *s_red)
                + uhbox("G: " + uhflex() + *s_green)
                + uhbox("B: " + uhflex() + *s_blue)
               )
       );
  
  return &udialog(ulabel("Grid")
                  + UBackground::white
                  + uhbox(upadding(8,8) + grid1 + " " + uhflex() + grid2)
                  + ubutton(UFont::bold + uhcenter() + " Close " + ucloseWin())
                 );
}

/*
 * Callbacks
 */

void Grid::toggleBehavior(int _behavior)
{
  behavior = _behavior;
}

void Grid::toggleOverlap()
{
  overlap ^= 1;
}

void Grid::toggleGrid2d()
{
  visible ^= 1;
}

void Grid::toggleGrid3d()
{
  visible ^= 1;
  grid3d ^= 1;
  draw();
}

void Grid::reset()
{
  visible = false;
}

/* changing width and depth of the grid */
void Grid::setWidth(UEvent &e)
{
  i_width = (int)((UScrollbar *)e.getSource())->getValue();
  // TODO: don't use getValue: find another way to read the value (argument ?)
  if (i_width <= 0)
    i_width = 1;
  draw();
}

void Grid::setHeight(UEvent &e)
{
  i_height = (int)((UScrollbar *)e.getSource())->getValue();
  if (i_height <= 0)
    i_height = 1;
  draw();
}

void Grid::setDepth(UEvent &e)
{
  i_depth = (int)((UScrollbar *)e.getSource())->getValue();
  if (i_depth <= 0)
    i_depth = 1;
  draw();
}

void Grid::setRed(UEvent &e)
{
  red = ((UScrollbar *)e.getSource())->getValue() / SCROLL_MAX;
  draw();
}

void Grid::setGreen(UEvent &e)
{
  green = ((UScrollbar *)e.getSource())->getValue() / SCROLL_MAX;
  draw();
}

void Grid::setBlue(UEvent &e)
{
  blue = ((UScrollbar *)e.getSource())->getValue() / SCROLL_MAX;
  draw();
}

void Grid::setPosX(UEvent &e)
{
  posx = ((UScrollbar *)e.getSource())->getValue() - 50;
}

void Grid::setPosY(UEvent &e)
{
  posy = ((UScrollbar *)e.getSource())->getValue() - 50;
}

void Grid::setPosZ(UEvent &e)
{
  posz = ((UScrollbar *)e.getSource())->getValue() - 50;
}

void Grid::setRotX(UEvent &e)
{
  rotx = ((UScrollbar *)e.getSource())->getValue();
}

void Grid::setRotY(UEvent &e)
{
  roty = ((UScrollbar *)e.getSource())->getValue();
}

void Grid::setRotZ(UEvent &e)
{
  rotz = ((UScrollbar *)e.getSource())->getValue();
}
