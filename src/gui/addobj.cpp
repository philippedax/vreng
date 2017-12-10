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
#include "gui.hpp"
#include "theme.hpp"
#include "widgets.hpp"
#include "wobject.hpp"
#include "mirage.hpp"	// Mirage
#include "wall.hpp"	// Wall
#include "ball.hpp"	// Ball
#include "ground.hpp"	// Ground
#include "model.hpp"	// Model
#include "step.hpp"	// Step
#include "thing.hpp"	// Thing
#include "gate.hpp"	// Gate
#include "user.hpp"	// User

#ifdef HUGE
#undef HUGE
#endif

/* items */
enum {
  NONE,
  THING, MIRAGE, WALL, GROUND, BALL, STEP, GATE,
  CUBE, SPHERE, CONE, CYLINDER, DISK, TORUS,
  MODEL, MAN, CAR, SHRUB, TREE, PENGUIN,
  CHAIR_WOOD, TABLE_WOOD, TABLE_METAL, TABLE_GLASS,
  BLACK, RED, GREEN, BLUE, PURPLE, YELLOW, CYAN, WHITE,
  TINY, SMALL, MEDIUM, BIG, HUGE,
  WOOD, MARBLE, BRICK, STUC, GRASS, PAPER, WATER, CLOUD, CHECK,
  OPAQUE, OPAQUE8, OPAQUE6, OPAQUE4, OPAQUE2, OPAQUE0,
  END
};

// Local
static uint8_t obj = THING;	// thing
static char shape[16] = "cube";	// box
static char texture[128] = "";	// empty
static V3 color;		// black
static float alpha = 1;		// opaque
static float size = .5;		// small

static const char chair_wood[] = "\
<solid dim=\".25 .25 .01\" dif=\".5 .3 .1\" zp=\"/gif/wood.gif\" />\n\
<solid dim=\".02 .25 .25\" rel=\".24 0 .24 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\"-.24 .24 -.24 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\".24 .24 -.24 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\"-.24 -.24 -.24 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\".24 -.24 -.24 0 0\" dif=\".5 .3 .1\" />\n\
";

static const char table_wood[] = "\
<solid dim=\".40 .80 .02\" yp=\"/gif/blondwood.gif\" xp=\"/gif/blondwood.gif\" xn=\"/gif/blondwood.gif\" yn=\"/gif/blondwood.gif\" zp=\"/gif/blondwood.gif\" />\n\
<solid dim=\".02 .02 .40\" rel=\"-.36 .76 -.39 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".02 .02 .40\" rel=\".36 .76 -.39 0 0\"  dif=\".5 .3 .1\" />\n\
<solid dim=\".02 .02 .40\" rel=\"-.36 -.76 -.39 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".02 .02 .40\" rel=\".36 -.76 -.39 0 0\" dif=\".5 .3 .1\" />\n\
";

static const char table_metal[] = "\
<solid dim=\".70 .3 .02\" dif=\".5 .5 .5\" />\n\
<solid dim=\".02,.02 .35\" dif=\".5 .5 .5\" rel=\"-.68 -.28 -.35 0 0\" />\n\
<solid dim=\".02 .02 .35\" dif=\".5 .5 .5\" rel=\"-.68 .28 -.35 0 0\" />\n\
<solid dim=\".02 .02 .35\" dif=\".5 .5 .5\" rel=\".68 -.28 -.35 0 0\" />\n\
<solid dim=\".02 .02 .35\" dif=\".5 .5 .5\" rel=\".68 .28 -.35 0 0\" />\n\
";

static const char table_glass[] = "\
<solid dim=\".5,.3 .02\" dif=\"0 1 0\" spe=\"0 1 1\" a=\".4\" />\n\
<solid shape=\"cone\" rb=\".1\" rt=\".1\" h=\".30\" dif=\"0 1 0\" spe=\"0 1 1\" a=\".5\" rel=\"0 0 -.30 0 0\" />\n\
";

static void resetForm() {
  obj = THING;
  strcpy(shape, "cube");
  *texture = '\0';
  alpha = 1;
  size = .5;
  color = newV3(1, 1, 1);
}

/** pour l'addition d'objet */
static void setForm(int item) {
  switch (item) {
    // objs :
    case THING : case MIRAGE : case WALL : case GROUND : case BALL : case STEP :
      obj = item; break;

    // shapes
    case SPHERE :   sprintf(shape, "sphere"); break;
    case CUBE :     sprintf(shape, "cube"); break;
    case CONE :     sprintf(shape, "cone"); break;
    case CYLINDER : sprintf(shape, "cylinder"); break;
    case DISK :     sprintf(shape, "disk"); break;
    case TORUS :    sprintf(shape, "torus"); break;

    // colors
    case BLACK :  color = newV3(0,0,0); break;
    case RED :    color = newV3(1,0,0); break;
    case GREEN :  color = newV3(0,1,0); break;
    case BLUE :   color = newV3(0,0,1); break;
    case PURPLE : color = newV3(1,0,1); break;
    case YELLOW : color = newV3(1,1,0); break;
    case CYAN :   color = newV3(0,1,1); break;
    case WHITE :  color = newV3(1,1,1); break;

    // textures
    case WOOD :   sprintf(texture, "/gif/wood.gif"); break;
    case MARBLE : sprintf(texture, "/gif/marble.gif"); break;
    case STUC :   sprintf(texture, "/gif/stuc.gif"); break;
    case BRICK :  sprintf(texture, "/gif/brick.gif"); break;
    case GRASS :  sprintf(texture, "/gif/grass.gif"); break;
    case PAPER :  sprintf(texture, "/gif/paper.gif"); break;
    case WATER :  sprintf(texture, "/gif/water.gif"); break;
    case CLOUD :  sprintf(texture, "/gif/clouds.gif"); break;
    case CHECK :  sprintf(texture, "/gif/check.gif"); break;

    // alpha
    case OPAQUE  : alpha = 1; break;
    case OPAQUE8 : alpha = .8; break;
    case OPAQUE6 : alpha = .6; break;
    case OPAQUE4 : alpha = .4; break;
    case OPAQUE2 : alpha = .2; break;
    case OPAQUE0 : alpha = .0; break;

    // sizes
    case TINY :   size = .125; break;
    case SMALL :  size = .25; break;
    case MEDIUM : size = .5; break;
    case BIG :    size = 1; break;
    case HUGE :   size = 2; break;

    // models
    case MAN :     sprintf(shape, "man"); break;
    case CAR :     sprintf(shape, "car"); break;
    case SHRUB :   sprintf(shape, "shrub"); break;
    case TREE :    sprintf(shape, "tree"); break;
    case PENGUIN : sprintf(shape, "penguin"); break;

    // solids
    case CHAIR_WOOD :  sprintf(shape, "chair_wood"); break;
    case TABLE_WOOD :  sprintf(shape, "table_wood"); break;
    case TABLE_METAL : sprintf(shape, "table_metal"); break;
    case TABLE_GLASS : sprintf(shape, "table_glass"); break;
  }
}

void GuiWidgets::addNewObjectCB()
{
  char s[1024], url[128];
  float r = .2 * size;
  float scale = 1;
  V3 c = color;

  s[0] = url[0] = '\0';

  // Shapes
  if (! strcmp(shape, "cube")) {
    sprintf(s, "solid shape=\"box\" dim=\"%f %f %f\" dif=\"%f %f %f\" tx_front=\"%s\" a=\"%f\"/>", r,r,r, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  else if (! strcmp(shape, "sphere")) {
    sprintf(s, "solid shape=\"sphere\" r=\"%f\" dif=\"%f %f %f\" tx=\"%s\" a=\"%f\"/>", r, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  else if (! strcmp(shape, "cone")) {
    sprintf(s, "solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%f %f %f\" tx=\"%s\" a=\"%f\"/>", r,r/2,2*r, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  else if (! strcmp(shape, "cylinder")) {
    sprintf(s, "solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%f %f %f\" tx=\"%s\" a=\"%f\"/>", r,r,2*r, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  else if (! strcmp(shape, "disk")) {
    sprintf(s, "solid shape=\"disk\" ri=\"%f\" re=\"%f\" dif=\"%f %f %f\" tx=\"%s\" a=\"%f\"/>", r,2*r, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  else if (! strcmp(shape, "torus")) {
    sprintf(s, "solid shape=\"torus\" ri=\"%f\" re=\"%f\" dif=\"%f %f %f\" tx=\"%s\" a=\"%f\"/>", r,r/5, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  if (! strcmp(shape, "man")) {
    sprintf(s, "solid shape=\"man\" dim=\".8 .15 .85\" a=\"%f\"/>", alpha);
  }
  if (! strcmp(shape, "car")) {
    sprintf(s, "solid shape=\"car\" dim=\".7 .7 .7\" dif=\"%f %f %f\" a=\"%f\"/>", c.v[0],c.v[1],c.v[2], alpha);
  }

  // Models
  else if (! strcmp(shape, "shrub")) {
    obj = MODEL;
    sprintf(url, "/3ds/tree_1.3ds");
    scale = .05;
  }
  else if (! strcmp(shape, "tree")) {
    obj = MODEL;
    sprintf(url, "/3ds/tree_2.3ds");
    scale = .07;
  }
  else if (! strcmp(shape, "penguin")) {
    obj = MODEL;
    sprintf(url, "/lwo/penguin.lwo");
    scale = .3;
  }
  else if (! strcmp(shape, "chair_wood")) {
    sprintf(s, "%s", chair_wood);
  }
  else if (! strcmp(shape, "table_wood")) {
    sprintf(s, "%s", table_wood);
  }
  else if (! strcmp(shape, "table_metal")) {
    sprintf(s, "%s", table_metal);
  }
  else if (! strcmp(shape, "table_glass")) {
    sprintf(s, "%s", table_glass);
  }

  // Calls constructor of the selectioned object
  if (! localuser) return;

  switch (obj) {
  case MIRAGE:
    new Mirage(localuser, s); break;
  case WALL:
    new Wall(localuser, s); break;
  case GROUND:
    sprintf(s, "solid dim=\"%f %f %f\" dif=\"%f %f %f\" zp=\"%s\"/>", 10.,10.,.1, c.v[0],c.v[1],c.v[2], texture);
    new Ground(localuser, s); break;
  case BALL:
    new Ball(localuser, s); break;
  case THING:
    new Thing(localuser, s); break;
  case STEP:
    new Step(localuser, s); break;
  case MODEL:
    new Model(localuser, url, scale); break;
  }

  resetForm();	// reset to default values
}

UDialog& GuiWidgets::createAddobjDialog()
{
  URadioSelect
    &sel_obj   = uradioSelect(),
    &sel_shape = uradioSelect(),
    &sel_color = uradioSelect(),
    &sel_tex   = uradioSelect(),
    &sel_alpha = uradioSelect(),
    &sel_size  = uradioSelect(),
    &sel_model = uradioSelect(),
    &sel_solid = uradioSelect() ;

  UBox* addobjBox = new UBox
    (UFont::bold
     + UFont::x_large
     + ulabel("Addobj")
     + UFont::medium
     + UFont::plain
     + UColor::navy + UFont::bold
     + uhbox(UBorder::shadowOut)
     + UFont::bold
     + ulabel("Simple solids")
     + UFont::plain
     + uhbox(UBorder::shadowOut)
     + uhbox("Object : " + UFont::plain
              + ucheckbox("Thing" + sel_obj
                          + UOn::select / ucall((int)THING, setForm)
                          ).setSelected()
              + ucheckbox("Mirage" + sel_obj
                          + UOn::select / ucall((int)MIRAGE, setForm))
              + ucheckbox("Wall" + sel_obj
                          + UOn::select / ucall((int)WALL, setForm))
              + ucheckbox("Ground" + sel_obj
                          + UOn::select / ucall((int)GROUND, setForm))
              + ucheckbox("Ball" + sel_obj
                          + UOn::select / ucall((int)BALL, setForm))
              + ucheckbox("Step" + sel_obj
                          + UOn::select / ucall((int)STEP, setForm))
              //+ ucheckbox("Gate" + sel_obj
              //            + UOn::select / ucall((int)GATE, setForm))
            )
     + uhbox(UBorder::shadowOut)
     + uhbox("Shape :  " + UFont::plain
             + ucheckbox("Cube" + sel_shape
                         + UOn::select / ucall((int)CUBE, setForm)
                         ).setSelected()
             + ucheckbox("Sphere" + sel_shape
                         + UOn::select / ucall((int)SPHERE, setForm))
             + ucheckbox("Cone" + sel_shape
                         + UOn::select / ucall((int)CONE, setForm))
             + ucheckbox("Cylinder" + sel_shape
                         + UOn::select / ucall((int)CYLINDER, setForm))
             + ucheckbox("Disk" + sel_shape
                         + UOn::select / ucall((int)DISK, setForm))
             + ucheckbox("Torus" + sel_shape
                         + UOn::select / ucall((int)TORUS, setForm))
            )
     + uhbox(UBorder::shadowOut)
     + uhbox("Color :  " + UFont::plain
             + ucheckbox("Black" + sel_color
                         + UOn::select / ucall((int)BLACK, setForm)
                         ).setSelected()
             + ucheckbox("White" + sel_color
                         + UOn::select / ucall((int)WHITE, setForm))
             + ucheckbox("Red" + sel_color
                         + UOn::select / ucall((int)RED, setForm))
             + ucheckbox("Green" + sel_color
                         + UOn::select / ucall((int)GREEN, setForm))
             + ucheckbox("Blue" + sel_color
                         + UOn::select / ucall((int)BLUE, setForm))
             + ucheckbox("Purple" + sel_color
                         + UOn::select / ucall((int)PURPLE, setForm))
             + ucheckbox("Yellow" + sel_color
                         + UOn::select / ucall((int)YELLOW, setForm))
             + ucheckbox("Cyan" + sel_color
                         + UOn::select / ucall((int)CYAN, setForm))
            )
    + uhbox(UBorder::shadowOut)
    + uhbox("Texture : " + UFont::plain
             + ucheckbox("Wood" + sel_tex
                         + UOn::select / ucall((int)WOOD, setForm)
                         ).setSelected()
             + ucheckbox("Marble" + sel_tex
                         + UOn::select / ucall((int)MARBLE, setForm))
             + ucheckbox("Stuc" + sel_tex
                         + UOn::select / ucall((int)STUC, setForm))
             + ucheckbox("Brick" + sel_tex
                         + UOn::select / ucall((int)BRICK, setForm))
             + ucheckbox("Grass" + sel_tex
                         + UOn::select / ucall((int)GRASS, setForm))
             + ucheckbox("Paper" + sel_tex
                         + UOn::select / ucall((int)PAPER, setForm))
             + ucheckbox("Water" + sel_tex
                         + UOn::select / ucall((int)WATER, setForm))
             + ucheckbox("Cloud" + sel_tex
                         + UOn::select / ucall((int)CLOUD, setForm))
             + ucheckbox("Check" + sel_tex
                         + UOn::select / ucall((int)CHECK, setForm))
            )
    + uhbox(UBorder::shadowOut)
    + uhbox("Alpha :  " + UFont::plain
             + ucheckbox("Opaque" + sel_alpha 
                         + UOn::select / ucall((int)OPAQUE, setForm)
                         ).setSelected()
             + ucheckbox(".8" + sel_alpha
                         + UOn::select / ucall((int)OPAQUE8, setForm))
             + ucheckbox(".6" + sel_alpha
                         + UOn::select / ucall((int)OPAQUE6, setForm))
             + ucheckbox(".4" + sel_alpha
                         + UOn::select / ucall((int)OPAQUE4, setForm))
             + ucheckbox(".2" + sel_alpha
                         + UOn::select / ucall((int)OPAQUE2, setForm))
             + ucheckbox("Translucid" + sel_alpha
                         + UOn::select / ucall((int)OPAQUE0, setForm))
            )
    + uhbox(UBorder::shadowOut)
    + uhbox("Size :   " + UFont::plain
             + ucheckbox("Tiny" + sel_size
                         + UOn::select / ucall((int)TINY, setForm))
             + ucheckbox("Small" + sel_size 
                         + UOn::select / ucall((int)SMALL, setForm)
                         ).setSelected()
             + ucheckbox("Normal" + sel_size
                         + UOn::select / ucall((int)MEDIUM, setForm))
             + ucheckbox("Big" + sel_size
                         + UOn::select / ucall((int)BIG, setForm))
             + ucheckbox("Huge" + sel_size
                         + UOn::select / ucall((int)HUGE, setForm))
            )
     + uhbox(UBorder::shadowOut)
     + UFont::bold
     + ulabel("Models")
     + UFont::plain
     + uhbox(UBorder::shadowOut)
     + uhbox("Model :  " + UFont::plain
             + ucheckbox("Car" + sel_model
                         + UOn::select / ucall((int)CAR, setForm))
             + ucheckbox("Man" + sel_model
                         + UOn::select / ucall((int)MAN, setForm))
             + ucheckbox("Shrub" + sel_model
                         + UOn::select / ucall((int)SHRUB, setForm))
             + ucheckbox("Tree" + sel_model
                         + UOn::select / ucall((int)TREE, setForm))
             + ucheckbox("Penguin" + sel_model
                         + UOn::select / ucall((int)PENGUIN, setForm))
            )
     + uhbox(UBorder::shadowOut)
     + UFont::bold
     + ulabel("Compound solids")
     + UFont::plain
     + uhbox(UBorder::shadowOut)
     + uhbox("Solids :  " + UFont::plain
             + ucheckbox("Chair wood" + sel_solid
                         + UOn::select / ucall((int)CHAIR_WOOD, setForm))
             + ucheckbox("Table wood" + sel_solid
                         + UOn::select / ucall((int)TABLE_WOOD, setForm))
             + ucheckbox("Table metal" + sel_solid
                         + UOn::select / ucall((int)TABLE_METAL, setForm))
             + ucheckbox("Table glass" + sel_solid
                         + UOn::select / ucall((int)TABLE_GLASS, setForm))
            )
    + uhbox(UBorder::shadowOut)
    + uhcenter()
    + uhbox(uhflex()
      + ubutton(UFont::bold + uhcenter() + " Add " 
                + ucall(this, &GuiWidgets::addNewObjectCB))
      + ubutton(UFont::bold + uhcenter() + " Cancel " + ucloseWin()))
    );

  return udialog(addobjBox);
}
