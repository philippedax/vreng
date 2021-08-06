#ifndef UI_HPP
#define UI_HPP

#include "glui.h"


#define MIN_SOLID_SIZE  0.1

enum UIObjectTypes {
  WALL_TYPE,
  GATE_TYPE,
  STEP_TYPE,
  WEB_TYPE,
  BOARD_TYPE,
  HOST_TYPE,
  DOC_TYPE,
  MIRAGE_TYPE,
  THING_TYPE,
  EARTH_TYPE
};

enum UIenum {
  ADD_OBJECT,
  GROUP,
  UNGROUP,
  DELETE,
  OPEN,
  SAVE,
  QUIT,
  OK_BTN,
  CANCEL_BTN,
  CENTER_XY,
  CENTER_Z,
  SIZE, 
  ROT_Z,
  RADIUS,
  TEXTURE,
  APPEARANCE,
  TARGET_URL
};

class Solid;
class Camera;
class Vect;

enum UIDialogUsage {
  OPEN_FILE,
  SAVE_FILE
};

struct objectChain {
  Solid *current;
  struct objectChain *next;
};


class UI {

private:
 static int mainWin;
 static Solid *item;
 static struct objectChain *selected;
 static const char *objectTypes[];
 static int currObject;
 static float center[3], centerXButton;
 static float size[3];
 static float angleZ;
 static float radius;
 static char urlXp[128], urlXn[128], urlYp[128], urlYn[128], urlZp[128], urlZn[128];
 static float dif[3], amb[3], shi[3], spe[3];
 static char url[128];
 static char ipmc[32];
 static int collis;
 static Camera *camera;
 static bool motion;
 static int mouseX, mouseY;
 static char *dialStr;
 static int dialUsage;

 static GLUI *topWin, *rightWin, *botWin, *dialGlui;
 static GLUI_EditText *centerGlui[3];
 static GLUI_EditText *sizeGlui[3];
 static GLUI_EditText *angleZGlui;
 static GLUI_EditText *radiusGlui;
 static GLUI_EditText *difGlui[3], *ambGlui[3], *shiGlui[3], *speGlui[3];
 static GLUI_EditText *texXp, *texXn, *texYp, *texYn, *texZp, *texZn, *texSph;
 static GLUI_EditText *urlGlui, *ipmcGlui;
 static GLUI_EditText *dialText;
 static GLUI_Translation *transXYButton;
 static GLUI_Translation *transZButton;
 static GLUI_Translation *sizeButton[3];
 static GLUI_Translation *rotZButton;
 static GLUI_Rollout *texRollout;
 static GLUI_Panel *texPanel;
 static GLUI_Rollout *appRollout;
 static GLUI_Button *grpButton, *ungrpButton, *delButton;
 static GLUI_StaticText *objectDescr;

 static void control_cb(int event);
 static void myGlutIdle();
 static void myGlutMouse(int button, int button_state, int x, int y);
 static void myGlutMotion(int x, int y);
 static void myGlutKeyboard(unsigned char key, int x, int y);
 static void myGlutSpecialKeyboard(int key, int x, int y);
 static void myGlutReshape(int x, int y);
 static void myGlutDisplay(void);
 static void renderSolid(Solid *solid);
 static void drawGrid();
 static void vertex(const Vect& v);
 static void createObject();
 static void updateControls();
 static void openDial(const char *_question, int _dlogUsage, const char *_defaultAnswer="");
 static void closeDial();

public:
  static void setupUI(int argc, char *argv[]);

};

#endif
