#ifndef UI_HPP
#define UI_HPP

#include "glui.h"

class Solid;
  /*
  class GLUI;
  class GLUI_EditText;
  class GLUI_StaticText;
  class GLUI_Translation;
  class GLUI_Rollout;
  class GLUI_Button;
  */

enum UIenum {
  UI_ADD_OBJECT,
  UI_GROUP,
  UI_UNGROUP,
  UI_DELETE,
  UI_OPEN,
  UI_SAVE,
  UI_QUIT,
  UI_OK_BTN,
  UI_CANCEL_BTN,
  UI_CENTER,
  UI_CENTER_X_BTN,
  UI_SIZE, 
  UI_ROT_Z,
  UI_RADIUS,
  UI_TEXTURE,
  APPEARANCE,
  TARGET_URL
};

enum UIDialogUsage {
  DLOG_OPEN_FILE,
  DLOG_SAVE_FILE
};

struct objectChain {
  Solid *current;
  struct objectChain *next;
};

class Camera;
class Vect;

class UI {
private:
 static int mainWindowID;
 static Solid *item;
 static struct objectChain *selected;
 static const char *objectTypesString[];
 static int currentObjectType;
 static float center[3], centerXButton;
 static float size[3];
 static float angleZ;
 static float radius;
 static char urlXp[128], urlXn[128], urlYp[128], urlYn[128], urlZp[128], urlZn[128];
 static float dif[3], amb[3], shi[3], spe[3];
 static char url[128], ipmc[32];
 static float viewRotate[16];
 static int preventCollision;
 static Camera *camera;
 static bool motionEnabled;
 static int mouseX, mouseY;
 static char *dialogString;
 static int dialogUsage;

 static GLUI *topSubwin, *rightSubwin, *bottomSubwin, *dialogGlui;
 static GLUI_EditText *centerField[3];
 static GLUI_EditText *sizeField[3];
 static GLUI_EditText *angleZField;
 static GLUI_EditText *radiusField;
 static GLUI_EditText *difField[3], *ambField[3], *shiField[3], *speField[3];
 static GLUI_EditText *texXp, *texXn, *texYp, *texYn, *texZp, *texZn, *texSph;
 static GLUI_EditText *urlField, *ipmcField;
 static GLUI_EditText *dialogEditText;
 static GLUI_Translation *objectTransXYButton;
 static GLUI_Translation *objectTransZButton;
 static GLUI_Translation *sizeButton[3];
 static GLUI_Translation *angleZButton;
 static GLUI_Rollout *texRollout;
 static GLUI_Rollout *appearanceRollout;
 static GLUI_Button *groupButton, *ungroupButton, *deleteButton;
 static GLUI_StaticText *objectClassDescription;

 static void controlEvent(int control);
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
 static void createNewObject();
 static void updateControls();
 static void askFor(const char *_question, int _dlogUsage, const char *_defaultAnswer="");
 static void stopAskingFor();

public:
  static void SetupUI(int argc, char *argv[]);

};

#endif
